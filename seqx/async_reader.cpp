/*
==============================================================================

This file is part of the async
Copyright 2005-11 by Satoshi Fujiwara.

async can be redistributed and/or modified under the terms of the
GNU General Public License, as published by the Free Software Foundation;
either version 2 of the License, or (at your option) any later version.

async is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with async; if not, visit www.gnu.org/licenses or write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
Boston, MA 02111-1307 USA

==============================================================================
*/

// SDKのサンプルを改造した作ったもの。内容はほぼそのままだが、非同期読み込みに
// 対応させた。

#include "stdafx.h"
#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#include <stdio.h>
#include <assert.h>
#include <wtypes.h>
#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>
#include "stdafx.h"
#include "async_reader.h"
using namespace std;

namespace sf
{
  typedef struct
  {
    ULONG Riff4CC;      // "RIFF" 4-character code
    ULONG FileSize;     // total file size in bytes
    ULONG Wave4CC;      // "WAVE" 4-character code
    ULONG Fmt4CC;       // "fmt " 4-character code
    ULONG FormatSize;   // wave format size in bytes
  } FileHeader;

  typedef struct
  {
    ULONG ChunkType;
    ULONG ChunkSize;
  } ChunkHeader;

  // Any file smaller than this cannot possibly contain wave data.
#define MIN_WAVE_FILE_SIZE (sizeof(FileHeader)+sizeof(PCMWAVEFORMAT)+sizeof(ChunkHeader)+1)

  // Macro to build FOURCC from first four characters in ASCII string
#define FOURCC(s)  ((ULONG)(s[0] | (s[1]<<8) | (s[2]<<16) | (s[3]<<24)))

  //
  // Constructor -- Open wave file and parse file header.
  //
  async_reader::async_reader(const std::wstring file_name, bool repeat_mode) 
    : 
    stream_status_(false),
    data_chunk_position(0),
    total_data_bytes_(0),
    data_bytes_remaining_(0),
    offset_(0),
    reader_repeat_mode_(repeat_mode),
    async_reading_(false)
  {

    // イベントオブジェクトの初期化
    ZeroMemory(&wfx_, sizeof(wfx_));
    event_.reset(::CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE));

    // OVERLAPPED構造体の初期化
    ZeroMemory(&overlapped_,sizeof(overlapped_));
    overlapped_.hEvent = event_.get();


    try{
      // ファイルオープン
      file_.reset(CreateFile(file_name.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY | FILE_FLAG_OVERLAPPED,NULL));
      if(file_.get() == INVALID_HANDLE_VALUE)
      {
        throw win32_error_exception(GetLastError());
      }

      // ファイルヘッダの読み込み
      FileHeader fileHdr;
      read_data_sync(reinterpret_cast<BYTE*>(&fileHdr),sizeof(fileHdr));

      // ヘッダのチェック
      if (fileHdr.Riff4CC != FOURCC("RIFF") ||
        fileHdr.FileSize < MIN_WAVE_FILE_SIZE ||
        fileHdr.Wave4CC != FOURCC("WAVE") ||
        fileHdr.Fmt4CC != FOURCC("fmt ") ||
        fileHdr.FormatSize < sizeof(PCMWAVEFORMAT))
      {
        throw file_error_exception(std::wstring(L"不明な.WAVファイルフォーマットです。"));
      }

      // フォーマットデスクリプタの読み込み
      read_data_sync(reinterpret_cast<BYTE*>(&wfx_),min(fileHdr.FormatSize,sizeof(wfx_)));
      //offset += min(fileHdr.FormatSize,sizeof(wfx_));

      // Skip over any padding at the end of the format in the format chunk.
      if (fileHdr.FormatSize > sizeof(wfx_))
      {
        offset_ += (fileHdr.FormatSize - sizeof(wfx_));
      }

      // If format type is PCMWAVEFORMAT, convert to valid WAVEFORMATEX structure.
      if (wfx_.Format.wFormatTag == WAVE_FORMAT_PCM)
      {
        wfx_.Format.cbSize = 0;
      }

      // If format type is WAVEFORMATEX, convert to WAVEFORMATEXTENSIBLE.
      if (wfx_.Format.wFormatTag == WAVE_FORMAT_PCM ||
        wfx_.Format.wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
      {
        if (wfx_.Format.wFormatTag == WAVE_FORMAT_PCM)
        {
          wfx_.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
        }
        else
        {
          wfx_.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
        }
        wfx_.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;

        // Note that the WAVEFORMATEX structure is valid for
        // representing wave formats with only 1 or 2 channels.
        if (wfx_.Format.nChannels == 1)
        {
          wfx_.dwChannelMask = SPEAKER_FRONT_CENTER;
        }
        else if (wfx_.Format.nChannels == 2)
        {
          wfx_.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
        }
        else
        {
          throw file_error_exception(std::wstring(L"サポートしていない.WAVファイルです。"));
        }
        wfx_.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
        wfx_.Samples.wValidBitsPerSample = wfx_.Format.wBitsPerSample;
      }

      // This wave file reader understands only PCM and IEEE float formats.
      if (wfx_.Format.wFormatTag != WAVE_FORMAT_EXTENSIBLE ||
        wfx_.SubFormat != KSDATAFORMAT_SUBTYPE_PCM &&
        wfx_.SubFormat != KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
      {
        throw file_error_exception(std::wstring(L"サポートしていない.WAVファイルです。"));
      }

      // データチャンクを検索する。その他のチャンクは読み飛ばす。
      ChunkHeader chunkHdr;   /// チャンクヘッダ
      for (;;)
      {
        // Read header at start of next chunk of file.
        data_chunk_position = offset_;
        read_data_sync(reinterpret_cast<BYTE*>(&chunkHdr),sizeof(ChunkHeader));
        if (chunkHdr.ChunkType == FOURCC("data"))
        {
          break;  // found start of data chunk
        }
      }

      // We've found the start of the data chunk. We're reader_ready to start
      // playing wave data...
      total_data_bytes_ = chunkHdr.ChunkSize;
      data_bytes_remaining_ = total_data_bytes_;
      if (total_data_bytes_ == 0)
      {
        throw file_error_exception(std::wstring(L"ファイルサイズが不正です。"));
      }
      stream_status_ = true;
    } catch (exception& e) {
      stream_status_ = false;
      throw;
    }
  }

  //
  // Destructor
  //
  async_reader::~async_reader()
  {
    // 残っているI/Oはキャンセルする
    if(async_reading_){
      DWORD numbytes_copied;
      DWORD res = GetOverlappedResult(file_.get(),&overlapped_,(LPDWORD)&numbytes_copied,FALSE);
      if(!res)
      {
        switch(GetLastError())
        {
        case ERROR_IO_INCOMPLETE:
        case ERROR_IO_PENDING:
          ::CancelIoEx(file_.get(),&overlapped_);
          WaitForSingleObject(event_.get(),INFINITE);
          break;
        }
      }
    }
    
    // ファイルをクローズする
    if (file_)
    {
      file_.release();
      //      fclose(file_);
    }
  }

  //
  // Reset the file pointer to the start of the wave data.
  //
  void async_reader::reset_data_position()
  {
    if (!stream_status_ )
    {
      throw file_error_exception(std::wstring(L"ファイルがオープンされていません。"));
    }

    // Read the header for the data chunk.
    ChunkHeader chunkHdr;
    offset_ = data_chunk_position;
    read_data_sync(reinterpret_cast<BYTE*>(&chunkHdr),sizeof(chunkHdr));

    // Sanity check: The chunk header shouldn't have changed.
    if (chunkHdr.ChunkType != FOURCC("data") ||
      chunkHdr.ChunkSize != total_data_bytes_)
    {
      throw file_error_exception(std::wstring(L"不正なWAVファイルです。"));
    }

    data_bytes_remaining_ = total_data_bytes_;

  }

  void async_reader::seek(uint64_t pos)
  {
    if (!stream_status_ )
    {
      throw file_error_exception(std::wstring(L"ファイルがオープンされていません。"));
    }
    pos = (pos / get_wave_format().Format.nBlockAlign) * get_wave_format().Format.nBlockAlign;
    data_bytes_remaining_ = total_data_bytes_ - pos;
    offset_ = pos + data_chunk_position + sizeof(ChunkHeader);
    overlapped_.Pointer = (PVOID)pos;
  }


  //
  // Load next block of wave data from file into playback buffer.
  // In repeat mode, when we reach the end of the wave data in the
  // file, we just reset the file pointer back to the start of the
  // data and continue filling the caller's buffer until it is full.
  // In single-reader_read_file mode, once we reach the end of the wave data in
  // the file, we just fill the buffer with silence instead of with
  // real data.
  //
  void async_reader::read_data(BYTE *buffer, uint64_t numbytes_to_copy)
  {
    assert(async_reading_ == false);

    if (!stream_status_ )
    {
      throw file_error_exception(std::wstring(L"ファイルがオープンされていません。"));
    }

    if (buffer == NULL)
    {
      throw file_error_exception(L"バッファアドレスが無効です。");
    }

    if (numbytes_to_copy == 0)
    {
      throw file_error_exception(L"読み込みバイト数の指定が0です。");
    }

    ULONG numbytes_copied;
    overlapped_.Pointer = (PVOID)offset_;
    int result = ReadFile(file_.get(),buffer,numbytes_to_copy,&numbytes_copied,&overlapped_);
    if(!result)
    {
      DWORD err = GetLastError(); 
      switch(err)
      {
      case ERROR_HANDLE_EOF:
        {
          if(numbytes_to_copy > numbytes_copied)
          {
            BYTE silence = (wfx_.Format.wBitsPerSample==8) ? 0x80 : 0;
            memset(buffer + numbytes_copied, silence, numbytes_to_copy - numbytes_copied);
          }
          offset_ += numbytes_copied; 
          async_reading_ = false;

          if(reader_repeat_mode_){
            reset_data_position();
          }
          throw file_eof_exception(std::wstring(L"ファイルの終わりに達したため、これ以上data読み込むことができません。"));
        }
        break;
      case ERROR_IO_PENDING:
        //{
        //  DWORD res = GetOverlappedResult(file_.get(),&overlapped_,&num_of_read,TRUE);
        //  if(!res)
        //  {
        //    throw file_error_exception(win32_error_exception::get_last_error_str());
        //  } else {
        //    if(res != numbytes_to_copy)
        //    {
        //      throw file_error_exception(std::wstring(L"読み込んだデータサイズが合いません。"));
        //    }
        //  }
        //}
        async_reading_ = true;
        break;
      default:
        throw file_error_exception(win32_error_exception::get_last_error_str(err));
        break;
      }
    } else {
      data_bytes_remaining_ -= numbytes_copied;
      offset_ += numbytes_copied;
      async_reading_ = false;
      //current += numbytes_copied;
    }


    //if(numbytes_copied > data_bytes_remaining_)
    //{
    //  if (fread(current, 1, data_bytes_remaining_, file_) != data_bytes_remaining_)
    //  {
    //    throw exception(L"不正なWAVファイルです。");
    //  }
    //  current += data_bytes_remaining_;
    //  numbytes_copied -= data_bytes_remaining_;
    //  data_bytes_remaining_ = 0;

    //  // The file pointer now sits at the end of the data chunk.
    //  // Are we operating in repeat mode?
    //  if (!reader_repeat_mode_)
    //  {
    //    // Nope, we're operating in single-reader_read_file mode. Fill
    //    // the rest of the buffer with silence and return.
    //    BYTE silence = (wfx_.Format.wBitsPerSample==8) ? 0x80 : 0;
    //    memset(current, silence, numbytes_copied);
    //    return;  // yup, we're done
    //  }
    //  // Yes, we're operating in repeat mode, so loop back to
    //  // the start of the wave data in the file's data chunk
    //  // and continue loading data into the caller's buffer.
    //  reset_data_position();

    //}

    //assert(numbytes_copied > 0);
    //assert(numbytes_copied <= data_bytes_remaining_);

    //// The remainder of the data chunk is big enough to
    //// completely fill the remainder of the caller's buffer.
    //if (fread(buffer, 1, numbytes_copied, file_) != numbytes_copied)
    //{
    //  throw exception(L"不正なWAVファイルです。");
    //}
    //data_bytes_remaining_ -= numbytes_copied;
    //current += numbytes_copied;
  }

  void async_reader::read_data_sync(BYTE *buffer, uint64_t numbytes_to_copy)
  {
    uint32_t numbytes_copied(0);
    overlapped_.Pointer = (PVOID)offset_;

    int result = ReadFile(file_.get(),buffer, numbytes_to_copy, (LPDWORD)&numbytes_copied, &overlapped_);

    if(!result)
    {
      uint32_t err = GetLastError(); 
      switch(err)
      {
      case ERROR_HANDLE_EOF:
        {
          offset_ += numbytes_copied;
          //if(reader_repeat_mode_){
          //  reset_data_position();
          //}
          throw file_eof_exception(std::wstring(L"ファイルの終わりに達したため、ヘッダを読み込むことができません。"));
        }
        break;
      case ERROR_IO_PENDING:
        {
          DWORD res = GetOverlappedResult(file_.get(),&overlapped_,(LPDWORD)&numbytes_copied,TRUE);
          if(!res)
          {
            throw file_error_exception(win32_error_exception::get_last_error_str());
          } else {
            if(numbytes_copied != numbytes_to_copy)
            {
              throw file_error_exception(std::wstring(L"読み込んだデータサイズが合いません。"));
            }
            offset_ += numbytes_copied;
          }
        }
        break;
      default:
          throw file_error_exception(win32_error_exception::get_last_error_str(err));
        break;
      }
    } else {
      offset_ += numbytes_copied;
    }
  };

  void async_reader::wait(int timer)
  {
    if(!async_reading_)
    {
      return;
    }

    uint32_t numbytes_copied(0);
    DWORD res = GetOverlappedResult(file_.get(),&overlapped_,(LPDWORD)&numbytes_copied,TRUE);
    // エラーチェック
    if(!res)
    {
      DWORD err = GetLastError();
      switch (err) 
      { 
      case ERROR_HANDLE_EOF: 
        { 
          // Handle an end of file
          //if(reader_repeat_mode_){
          //  reset_data_position();
          //}
          throw file_eof_exception(win32_error_exception::get_last_error_str(err));
          break;
        } 
      default:
        {
          throw file_error_exception(win32_error_exception::get_last_error_str(err));
        }
      }
    }

    ResetEvent(event_.get());
    offset_ += numbytes_copied;
    data_bytes_remaining_ -= numbytes_copied;
    if(data_bytes_remaining_ <= 0 && reader_repeat_mode_)
    {
      reset_data_position();
    }
    async_reading_ = false;
  };


}
