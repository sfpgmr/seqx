#pragma once
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
#include <audioclient.h>
#include "exception.h"
#include "audio_source.h"
#include "sf_memory.h"

namespace sf {
  class async_reader : public audio_source
  {
  public:
    struct file_error_exception : public sf::exception 
    {
      file_error_exception(const std::wstring& reason) : exception(reason) {};
      file_error_exception(const wchar_t* reason) : exception(std::wstring(reason)) {};
    };

    struct file_eof_exception : public sf::exception
    {
      file_eof_exception(const std::wstring& reason) : exception(reason) {};
      file_eof_exception(const wchar_t* reason) : exception(std::wstring(reason)) {};

    };
    async_reader(const std::wstring file_name, bool repeat_mode);
    ~async_reader();
    bool stream_status() { return stream_status_; };
    WAVEFORMATEXTENSIBLE &get_wave_format()
    {
      return  wfx_ ;
    };

    bool more_data_available()
    {
      return (stream_status_  &&
        (reader_repeat_mode_  || data_bytes_remaining_ > 0));
    };
    uint64_t total_data_bytes() { return total_data_bytes_; }
    uint64_t data_bytes_remaining() { return data_bytes_remaining_; }
    bool seekable() {return true;}
    void seek(uint64_t pos);
    void read_data(BYTE *buffer, uint64_t numbytes_to_copy);
    void read_data_sync(BYTE *buffer, uint64_t numbytes_to_copy);

    void reset_data_position();
    HANDLE raw_handle() { return file_.get();};
    void wait(int timer = INFINITE);
    bool repeat_mode() const  {return reader_repeat_mode_;}
    void repeat_mode(bool v) { reader_repeat_mode_ = v;}
  private:

    handle_holder file_;
    handle_holder event_;
    bool stream_status_;
    uint64_t data_chunk_position;
    uint64_t total_data_bytes_;
    uint64_t data_bytes_remaining_;
    uint64_t offset_;
    WAVEFORMATEXTENSIBLE wfx_;
    bool reader_repeat_mode_;
    bool async_reading_;
    OVERLAPPED overlapped_;

  };


}

