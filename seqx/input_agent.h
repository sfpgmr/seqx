﻿#pragma once
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
struct input_agent_t : public agent_base
{
  input_agent_t(){};
  ~input_agent_t(){};
// -----------------------------  
// WASAPI入力関係
// -----------------------------  
  enum input_status
  {
    status_device_config,
    status_device_config_ok,
    status_wait_output_active,
    status_wait_output_active_ok,
    status_process,
    status_processing,
    status_pause,
    status_pause_ok,
    status_exit,
    status_nodevice,
    status_error = STATUS_ERROR
  };

  void apply_config(int device_index,wasapi_device_manager::device_info::params_t& params);
  //void init_buffer();

  audio_base& device() {return *wasapi_input_.get();}
protected:
  void run();
private:
  // 入力デバイスの設定・変更
  void apply_config_(int device_index,wasapi_device_manager::device_info::params_t& params);
  std::unique_ptr<audio_base> wasapi_input_;
};
}

