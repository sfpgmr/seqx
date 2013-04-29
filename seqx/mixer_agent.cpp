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
#include "mixer_agent.h"
#include "dout.h"
#include "async_reader.h"

using namespace std;

namespace sf {


void mixer_agent_t::run()
{
  // TODO:ミキサーの実装
  // COMの初期化
  sf::com_initialize init(0,multi_threaded);

  // MMCSSの初期化
  //av_mm_thread_characteristics avmm(wstring(L"Pro Audio"));
  //avmm.set_priority(AVRT_PRIORITY::AVRT_PRIORITY_HIGH);
  int status;
  application& app(*application::instance());

  // outputデバイスが稼働するまで待つ
  //app.output_thread().wait_status(output_agent_t::status_processing,10);
  // バッファ初期化
  //init_buffer();
  change_status(status_config);
  while(status = status_.load(),status !=status_exit)
  {
  switch(status)
  {
  case status_config:
    {
      change_status(status_config_ok);
    }
    break;
  case status_process:
    {
      init_buffer();
      change_status(status_processing);
    }
  case status_processing:
    {
      Sleep(app.output_device().get_buffer_duration() / 20000);
      BYTE *in(0),*reader(0);
      app.input_ringbuffer().pop(in);
      app.reader_ringbuffer().pop(reader);
      int size(app.output_device().get_buffer_byte_size());
      BYTE *dest(buffer_[index_].get());
      if(in != 0 && reader != 0)
      {
        ::CopyMemory(dest,in,size);
        size = size / app.output_device().get_frame_size();
        WORD *destw((WORD*)dest),*srcw((WORD*)reader);
        for(int i = 0;i < size;++i)
        {
          *destw++ += *srcw++;
          *destw++ += *srcw++;
        }
      } else if(in != 0)
      {
        ::CopyMemory(dest,in,size);
      } else if(reader != 0)
      {
        ::CopyMemory(dest,reader,size);
      } else {
        ::ZeroMemory(dest,size);
      }
      
      while(!ringbuffer_.push(buffer_[index_].get()))
      {
        Sleep(app.output_device().get_buffer_duration() / 20000);
      }

      index_ = (index_ + 1) & (buffer_.size() -1 );

    }
    break;
  case status_pause:
    change_status(status_pause_ok);
    break;
  default:
        wait_event();
//        WaitForSingleObject(event_.get(),WAIT_TIMEOUT_DEFAULT);
  }
  }
    debug_out(L"***** mixerは終了!\n");
  agent::done();
}
}

