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
#include "input_agent.h"
#include "dout.h"
#include "async_reader.h"

using namespace std;

namespace sf {

  void input_agent_t::run()
  {
    // COMの初期化
    sf::com_initialize init(0,multi_threaded);

    // MMCSSの初期化
    sf::av_mm_thread_characteristics avmm(wstring(L"Pro Audio"));
    avmm.set_priority(AVRT_PRIORITY_HIGH);

    //// input デバイスの初期化
    //wasapi_device_manager::ptr m(wasapi_device_manager::instance());
    //apply_config_(
    // m->current_input_device_index(),
    // m->current_input_device().params
    //);
    change_status(status_pause);
    application& app(*application::instance());
    int status = STATUS_ERROR;
    BYTE *buffer = 0;
    index_ = 0;
    int source_counter ,dest_counter;
    source_counter  = dest_counter = 0;
    //// outputデバイスが稼働するまで待つ
    //app.output_thread().wait_status(output_agent_t::status_processing,10);
    //apply_config_(
    //wasapi_device_manager::instance()->current_input_device_index(),
    //wasapi_device_manager::instance()->current_input_device().params);
    change_status(status_device_config);
    while(status = status_.load(),status != status_exit)
    {
      // イベントを待つ
      switch(status)
      {
      case status_device_config:
        if(wasapi_input_)
        {
          wasapi_input_->stop();
          dest_counter = 0;
          change_status(status_device_config_ok);
        } else {
          change_status(status_nodevice);
        }
        break;
      case status_process:
        if(wasapi_input_){
        if(!wasapi_input_->is_start()){
          wasapi_input_->start();
        }
        change_status(status_processing);
        }
        break;
      case status_processing:

        {
          assert(wasapi_input_ != nullptr);
          wasapi_input_->wait();// 入力待ち
          try{
            get_buffer g(*wasapi_input_);// キャプチャバッファの取得
            if(g.size() && g != 0)
            {
              source_counter = 0;
              while(source_counter != g.size())
              {
                BYTE * src = g + source_counter * wasapi_input_->get_frame_size();
                int size_byte_src = g.size_byte() - source_counter * wasapi_input_->get_frame_size();
                int size_src = g.size() - source_counter;

                BYTE * dest = buffer_[index_].get() + dest_counter * app.output_device().get_frame_size();
                int size_byte_dest = app.output_device().get_buffer_byte_size() - dest_counter * app.output_device().get_frame_size();
                int size_dest = app.output_device().get_buffer_size() - dest_counter;

                if(size_src <= size_dest)
                {
                  ::CopyMemory(dest,src,size_byte_src);
                  source_counter += size_src;
                  dest_counter += size_src;
                } else if(size_src > size_dest)
                {
                  ::CopyMemory(dest,src,size_byte_dest);
                  source_counter += size_dest;
                  dest_counter += size_dest;
                }
                if(dest_counter == app.output_device().get_buffer_size())
                {
                  if(ringbuffer_.push(buffer_[index_].get()))
                  {
                    index_ = (index_ + 1) & (buffer_.size() - 1);
                  }
                  dest_counter = 0;
                }
              }
            }
          } catch (...) {
            ;
          }
        }

        break;
      case status_pause:
        if(wasapi_input_)
        {
          wasapi_input_->stop();
        }
        change_status(status_pause_ok);
        break;
      default:
        wait_event();
//        WaitForSingleObject(event_.get(),WAIT_TIMEOUT_DEFAULT);
        break;
      }
    }
loop_end:
    ;
    DOUT(L"##### input_threadは終了！" << endl);
    if(wasapi_input_ && wasapi_input_->is_start())
    {
      wasapi_input_->stop();
      wasapi_input_.reset();
    }

    agent::done();
  };

  void input_agent_t::apply_config(int device_index,wasapi_device_manager::device_info::params_t& params)
  {
    if(status() != status_device_config_ok && status() != status_nodevice)
    {
      change_and_wait(status_device_config,status_device_config_ok);
    }

    apply_config_(device_index,params);
    if(wasapi_input_)
    {
      change_and_wait(status_device_config,status_device_config_ok);
    }
    if(status() != status_nodevice)
    {
      init_buffer();
      change_and_wait(status_process,status_processing);
    }
  }

  void input_agent_t::apply_config_(int device_index,wasapi_device_manager::device_info::params_t& params)
  {

    WAVEFORMATEXTENSIBLE form;
    if(wasapi_input_)
    {
      if(wasapi_input_->is_start())
      {
        wasapi_input_->stop();
      }
      wasapi_input_.reset();
    };

    //bits_pair bits = {params.bits,params.valid_bits};
    //make_wave_format(form,params.sample_rate,params.channel,bits);

    try {
      if(params.exclusive_mode)
      {
        if(params.event_mode){
          wasapi_input_.reset(new sf::wasapi_capture_exclusive_event(device_index,params));
        } else {
          wasapi_input_.reset(new sf::wasapi_capture_exclusive_timer(device_index,params));
        };
      } else {
        if(params.event_mode)
        {
          wasapi_input_.reset(new sf::wasapi_capture_shared_event(device_index,params));
        } else {
          wasapi_input_.reset(new sf::wasapi_capture_shared_timer(device_index,params));
        }
      }
    } catch (win32_error_exception& e)
    {

      //window_->message_box((boost::wformat(L"WASAPI初期化エラーが発生しました。設定パラメータを見なおしてください。%s") % e.error()).str(),wstring(L"WASAPI初期化エラー"));
      throw;
    }

    wasapi_device_manager::instance()->select_input_device(device_index);
    wasapi_device_manager::instance()->current_input_device().params = params;

  }

  //void input_agent_t::init_buffer()
  //{
  //  audio_base& in(application::instance()->input_device());
  //  for(int i = 0,size = buffer_.size();i < size;++i)
  //  {
  //    buffer_[i].reset(reinterpret_cast<uint8_t*>(_aligned_malloc(sizeof(uint8_t) * in.get_buffer_byte_size(),16)));
  //  }
  //}


}

