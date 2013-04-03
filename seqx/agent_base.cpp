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
#include "agent_base.h"
#include "application.h"

using namespace std;

namespace sf {

    void agent_base::init_buffer()
    {
      audio_base& out(application::instance()->output_device());
      ringbuffer_.reset();
      for(int i = 0,size = buffer_.size();i < size;++i)
      {
        buffer_[i].reset(reinterpret_cast<uint8_t*>(_aligned_malloc(sizeof(uint8_t) * out.get_buffer_byte_size(),16)));
      }
      index_ = 0;
    }

}


