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

#include "StdAfx.h"

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "sf_windows.h"
#include "timer.h"

namespace sf {
timer::timer(sf::base_window& window,uint32_t timeout) : window_(window),timeout_(timeout),timer_id_(0)
{

}
void timer::start(){
  if(!::SetTimer(reinterpret_cast<HWND>(window_.raw_handle()),(UINT_PTR)&timer_id_,timeout_,NULL)){
    throw timer::exception();
  };
};

void timer::player_stop()
{
  if(timer_id_)
  {
    ::KillTimer(reinterpret_cast<HWND>(window_.raw_handle()),(UINT_PTR)&timer_id_);
    timer_id_ = 0;
  }
};

}
