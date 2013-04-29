/*
==============================================================================

This file is part of the async
Copyright 2005-10 by Satoshi Fujiwara.

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
#include "StdAfx.h"
#include <commctrl.h>

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "message_loop.h"
#include "sf_com.h"
#include "application.h"
#include "dout.h"
#include "async_reader.h"
#include "output_agent.h"


using namespace std;
using namespace boost;

namespace sf {

  /// WASAPIoutputスレッド
  void output_agent_t::run()
  {
    // COMの初期化
    sf::com_initialize init(0,multi_threaded);

    // MMCSSの初期化
    sf::av_mm_thread_characteristics avmm(wstring(L"Pro Audio"));
    avmm.set_priority(AVRT_PRIORITY_HIGH);
    //apply_config_(
    //  wasapi_device_manager::instance()->current_output_device_index(),
    //  wasapi_device_manager::instance()->current_output_device().params);
    BYTE* reader_buffer,*input_buffer,*mix_buffer;
    uint32_t status;
    size_t size_of_buffer = 0;
    size_t remaining_of_buffer =  0;
    BYTE* wasapi_buffer = 0;
    size_t remaining_of_wasapi_buffer = 0;
    memory_block_t mix_work;

    bool wasapi_ready = false;

    //if(wasapi_output_->is_enabled())
    //{
    //  change_status(status_processing);
    //} else {
      change_status(status_device_config);
    //}

    application& app(*application::instance());


    while(status = status_.load(std::memory_order_acquire), (status != status_exit ))
    { 
      try {
        switch (status)
        {
        case status_device_config:
          // outputデバイスの変更
          if(wasapi_output_ && wasapi_output_->is_start())
          {
            wasapi_output_->stop();
          }
          remaining_of_buffer = 0;
          change_status(status_device_config_ok);
          break;
        case status_process:
          mix_work.reset(reinterpret_cast<BYTE*>(_aligned_malloc(sizeof(uint8_t) * wasapi_output_->get_buffer_byte_size(),16))); 
          change_status(status_processing);
          // fall through
        case status_processing:
          {
            size_of_buffer = wasapi_output_->get_buffer_size();
            assert(wasapi_output_->is_enabled());
            // 処理されたバイト数
            if(remaining_of_buffer == 0) // data残り０なら
            {
              //リングバッファから新たにデータを読み込む
              //if(!app.mixer_ringbuffer().dequeue(reader_buffer)) // 読み込めない場合
              int size = wasapi_output_->get_buffer_byte_size();
              if(!app.reader_ringbuffer().pop(reader_buffer)) reader_buffer = 0;
              if(!app.input_ringbuffer().pop(input_buffer)) input_buffer = 0;
              if(input_buffer != 0 && reader_buffer != 0)
              {
                ::CopyMemory(mix_work.get(),input_buffer,size);
                size = size / app.output_device().get_frame_size();
                WORD *destw((WORD*)mix_work.get()),*srcw((WORD*)reader_buffer);
                for(int i = 0;i < size;++i)
                {
                  *destw++ += *srcw++;
                  *destw++ += *srcw++;
                }
              } else if(input_buffer != 0)
              {
                ::CopyMemory(mix_work.get(),input_buffer,size);
              } else if(reader_buffer != 0)
              {
                ::CopyMemory(mix_work.get(),reader_buffer,size);
              } else {
                get_buffer g(*wasapi_output_.get(),size_of_buffer);
				if(g.size() != 0){
					::ZeroMemory(g.get(),g.size_byte());
				}
                goto copy_end;
              }
              mix_buffer = mix_work.get();
              remaining_of_buffer = size_of_buffer;
            }

            // WASAPIからバッファアドレスを取得する
            { get_buffer g(*wasapi_output_.get(),size_of_buffer);
            wasapi_buffer = g;
            remaining_of_wasapi_buffer = g.size();// バッファのサイズ必ずしも
            // 要求したバイトが取得できるとはかぎらない

            if(remaining_of_buffer == remaining_of_wasapi_buffer)
            {
              // バッファの残りとWASAPIバッファの残りが同じとき
              const uint32_t size_copy(remaining_of_wasapi_buffer * wasapi_output_->get_frame_size());
              ::CopyMemory(wasapi_buffer,mix_buffer,size_copy);
              remaining_of_buffer = 0;
              remaining_of_wasapi_buffer = 0; 
            } else if(remaining_of_buffer > remaining_of_wasapi_buffer)
            {
              // バッファの残り>WASAPIバッファの残り
              const uint32_t size_copy(remaining_of_wasapi_buffer * wasapi_output_->get_frame_size());
              ::CopyMemory(wasapi_buffer,mix_buffer,size_copy);
              mix_buffer += size_copy;
              remaining_of_buffer -= remaining_of_wasapi_buffer;
              remaining_of_wasapi_buffer = 0;
            } else if(remaining_of_buffer < remaining_of_wasapi_buffer)
            {
              // バッファの残り<WASAPIバッファの残り
              const uint32_t size_copy(remaining_of_buffer * wasapi_output_->get_frame_size());
              ::CopyMemory(wasapi_buffer,mix_buffer,size_copy);
              wasapi_buffer +=  size_copy;
              remaining_of_wasapi_buffer -= remaining_of_buffer;

              int size = wasapi_output_->get_buffer_byte_size();
              if(!app.reader_ringbuffer().pop(reader_buffer)) reader_buffer = 0;
              if(!app.output_ringbuffer().pop(input_buffer)) input_buffer = 0;

              if(input_buffer != 0 && reader_buffer != 0)
              {
                ::CopyMemory(mix_work.get(),input_buffer,size_of_buffer);
                size = size_of_buffer / app.output_device().get_frame_size();
                WORD *destw((WORD*)mix_work.get()),*srcw((WORD*)reader_buffer);
                for(int i = 0;i < size;++i)
                {
                  *destw++ += *srcw++;
                  *destw++ += *srcw++;
                }
              } else if(input_buffer != 0)
              {
                ::CopyMemory(mix_work.get(),input_buffer,size);
              } else if(reader_buffer != 0)
              {
                ::CopyMemory(mix_work.get(),reader_buffer,size);
              } else {
                memset(wasapi_buffer,0,remaining_of_wasapi_buffer * wasapi_output_->get_frame_size());
                continue;
              }
              mix_buffer = mix_work.get();
              {
                const uint32_t size_copy(remaining_of_wasapi_buffer * wasapi_output_->get_frame_size());
                ::CopyMemory(wasapi_buffer,mix_buffer,size_copy);
                mix_buffer += size_copy;
                remaining_of_buffer = size_of_buffer - remaining_of_wasapi_buffer;
              }
            }
            }
copy_end:
            if(!wasapi_output_->is_start())
            {
              wasapi_output_->start();
            }

            if(wasapi_output_->get_current_padding() == size_of_buffer)
            {
              wasapi_output_->wait();
            } else {
              debug_out(L"XXXX No Wait !!!! XXXX\n");
            }
          }
          break;
        default:
        wait_event();
//        WaitForSingleObject(event_.get(),WAIT_TIMEOUT_DEFAULT);
          break;
        }
      } catch (win32_error_exception& e) {
        error_ = (wformat(L"outputスレッドでエラーが発生しました。:%s %x") % e.error() % e.hresult()).str();
        change_status(status_error);
      } catch (...) {
        error_ = L"outputスレッドで不明なエラーが発生しました。";
        change_status(status_error);
      }
    }
loop_end:
    if(wasapi_output_){
      if(wasapi_output_->is_start())
      {
        wasapi_output_->stop();
      }
      wasapi_output_.reset();
    }
    debug_out(L"***** output_threadは終了!\n");
    agent::done();
  };

  void output_agent_t::apply_config(int device_index,wasapi_device_manager::device_info::params_t& params)
  {
    if(status() != status_device_config_ok){ 
      change_and_wait(status_device_config,status_device_config_ok);
    }

    apply_config_(device_index,params);

    change_and_wait(status_process,status_processing);

    //if(reader_ && status_.load() == reader_ready){
    //  reader_->reset_data_position();
    //  reader_index_ = 0;
    //}
  }

  void output_agent_t::apply_config_(int device_index,wasapi_device_manager::device_info::params_t& params)
  {

    //  assert(status_.load() == reader_stop);

    // outputスレッドを設定変更状態にする

    WAVEFORMATEXTENSIBLE form;

    if(wasapi_output_)
    {
      if(wasapi_output_->is_start())
      {
        wasapi_output_->stop();
      }
      wasapi_output_.reset();
    };

    try {
      if(params.exclusive_mode)
      {
        if(params.event_mode){
          wasapi_output_.reset(new sf::wasapi_exclusive_event(device_index,params));
        } else {
          wasapi_output_.reset(new sf::wasapi_exclusive_timer(device_index,params));
        };
      } else {
        if(params.event_mode)
        {
          wasapi_output_.reset(new sf::wasapi_shared_event(device_index,params));
        } else {
          wasapi_output_.reset(new sf::wasapi_shared_timer(device_index,params));
        }
      }
    } catch (win32_error_exception& e)
    {

      //window_->message_box((boost::wformat(L"WASAPI初期化エラーが発生しました。設定パラメータを見なおしてください。%s") % e.error()).str(),wstring(L"WASAPI初期化エラー"));
      throw;
    }

    wasapi_device_manager::instance()->select_output_device(device_index);
    wasapi_device_manager::instance()->current_output_device().params = params;

    // バッファのアロケート
    init_buffer();


  }
}

