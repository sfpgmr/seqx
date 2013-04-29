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
#include "dout.h"
#include "async_reader.h"
#include "application.h"
#include "reader_agent.h"

using namespace std;

namespace sf {

  void reader_agent_t::run()
  {
    // COMの初期化
    sf::com_initialize init(0,multi_threaded);
    application& app(*application::instance().get());
    while(true)
    {
      switch(status_.load(std::memory_order_acquire))
      {
      case status_config:
        change_status(status_config_ok);
        break;
      case status_ready:
        debug_out(L"@@@@ reader_agent_t:ready @@@@  \n");
        change_status(status_ready_ok);
        break;
      case status_exit:
        debug_out(L"@@@@ reader_agent_t:exit @@@@  \n");
        goto loop_end;
        break;
      case status_play:
        {
//          init_buffer();
          change_status(status_play_ok);
        }
      case status_play_ok:
        {
          debug_out(L"@@@@ reader_agent_t:read_file @@@@  \n");
          audio_base& out(app.output_device());
          while(status_.load(std::memory_order_acquire) == status_play_ok)
          {
            if(reader_->more_data_available() || not_enqueue_)
            {
              if(!not_enqueue_){              
                uint32_t size = reader_->data_bytes_remaining() >  out.get_buffer_byte_size()
                  ?  out.get_buffer_byte_size() : reader_->data_bytes_remaining();
                if(size == 0 && repeat_mode_)
                {
                  reader_->reset_data_position();
                  size = (reader_->data_bytes_remaining() >  out.get_buffer_byte_size())
                    ?  out.get_buffer_byte_size() : reader_->data_bytes_remaining();
                }

                reader_->read_data(buffer_[index_].get(),sizeof(uint8_t) * size);
                //reader_->

                // ここに変換処理を入れる
                position_ += size;
                if(position_ > reader_->total_data_bytes() && repeat_mode_)
                {
                  position_ -= reader_->total_data_bytes();
                };

                reader_->wait();
                if(size < out.get_buffer_byte_size())
                {
                  memset(buffer_[index_].get() + size,0,out.get_buffer_byte_size() - size);
                }
              }

              not_enqueue_ = false;

              while(!ringbuffer_.push(buffer_[index_].get()))
              {
                //wdout << L"++++ queue max ++++ : " << index_ << std::endl;
                if(status_.load() != status_play_ok)
                {
                  if(status_.load(std::memory_order_relaxed) == status_pause_ok)
                  {
                    not_enqueue_ = true;
                  }
                  break;
                } else {
                  Sleep(1);
                }
              }

              ;
              //#ifdef _DEBUG
              //            wdout << boost::wformat(L"index:%d address:%x 差分:%x") 
              //            % index_ % buffer_[index_].get()
              //            % (buffer_[(index_ + 1) & (buffer_.size() - 1)].get() - buffer_[index_].get())
              //            << std::endl; 
              //#endif
              if(!not_enqueue_){
                index_ = (++index_) & (buffer_.size() - 1);
              }

            } else {
              status_.store(status_end);
              break;
            }
          }
        }
        break;
      case status_pause:
        debug_out(boost::wformat(L"**pause** index:%d address:%x") % index_ % buffer_[index_].get());
        change_status(status_pause_ok);
        break;
      case status_end:
        app.reader_end();
        reader_->reset_data_position();
        reset_ringbuffer(ringbuffer_);
        position_ = 0;
        change_status(status_end_ok);
        break;
      case status_seek:
        reader_->seek(position_);
        reset_ringbuffer(ringbuffer_);
        index_ = 0;
        change_status(status_seek_ok);
        break;
      case status_stop:
        debug_out(L"reader_agent_t **stop** \n");
        reader_->reset_data_position();
        reset_ringbuffer(ringbuffer_);
        position_ = 0;
        change_status(status_stop_ok);
        break;
      default:
        wait_event();
//        WaitForSingleObject(event_.get(),WAIT_TIMEOUT_DEFAULT);
        break;
      }
    }
loop_end:
    ;
    debug_out(L"##### reader_agent_tは終了！\n");
    agent::done();
  }

  void reader_agent_t::setup(const std::wstring& file_path)
  {
    change_and_wait(status_config,status_config_ok);
    source_file_path_ = file_path;
    reader_.reset(new async_reader(file_path,repeat_mode_));
    init_buffer();
    position_ = 0;
    change_and_wait(status_ready,status_ready_ok);
  };

  void reader_agent_t::position(uint64_t pos)
  {
    uint64_t  div = reader_->get_wave_format().Format.nChannels * reader_->get_wave_format().Format.wBitsPerSample / 8;
    position_ = (pos / div) * div;
    status_backup_ = status_.load();

    change_and_wait(status_seek,status_seek_ok);
    change_and_wait(status_backup_ - 1,status_backup_);
  }

  void reader_agent_t::pause()
  {
    if(status_.load() == status_pause_ok)
    {
      read_file();
    } else {
      change_and_wait(status_pause,status_pause_ok);
    }
  }

  void reader_agent_t::read_file()
  {
    change_and_wait(status_play,status_play_ok);
  }

  void reader_agent_t::stop()
  {
    change_and_wait(status_stop,status_stop_ok);
  }

}


