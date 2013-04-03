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
#pragma once
#include "base_window.h"
namespace sf{
struct timer
{
  struct exception
    : public sf::win32_error_exception 
  {
    exception(uint32_t hr) : win32_error_exception(hr) {};
    exception() : win32_error_exception() {} ;
  };

  timer(sf::base_window& window,uint32_t timeout);
  void start();
  void player_stop();
  ~timer(){player_stop();};
private:
  sf::base_window& window_;
  uint32_t timer_id_;
  uint32_t timeout_;
};
}

