#pragma once
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
#include "agent_base.h"

namespace sf {
struct mixer_agent_t : public agent_base
{
  mixer_agent_t(){};
  ~mixer_agent_t(){};
  enum mixer_status
  {
    status_config,
    status_config_ok,
    status_process,
    status_processing,
    status_pause,
    status_pause_ok,
    status_exit,
    status_error = STATUS_ERROR
  };
protected:
  void run();
private:
};
}

