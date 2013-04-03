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
struct reader_agent_t : public agent_base
{
public:
  struct exception
    : public sf::win32_error_exception 
  {
    exception(uint32_t hr) : win32_error_exception(hr) {};
    exception() : win32_error_exception() {} ;
  };

  reader_agent_t() : repeat_mode_(false) {};
  ~reader_agent_t(){};


//  void init_buffer(buffer_t& buffer);
  
//---------------------------------------
// ファイル読取り関係
//---------------------------------------
public:
  enum reader_status
  {
    status_config,
    status_config_ok,
    status_ready,
    status_ready_ok,
    status_stop,
    status_stop_ok,
    status_play,
    status_play_ok,
    status_end,
    status_end_ok,
    status_pause,
    status_pause_ok,
    status_seek,
    status_seek_ok,
    status_rew,
    status_rew_ok,
    status_ff,
    status_ff_ok,
    status_exit,
    status_fail = STATUS_ERROR
  }; 

  void setup(const std::wstring& file_path);
  void read_file();
  void pause();
  void stop();
  uint64_t position() { return position_;};
  void position(uint64_t pos);
  uint64_t size() { return reader_->total_data_bytes(); }
  void repeat_mode(bool v) { repeat_mode_ = v ;reader_->repeat_mode(v);}
  const bool repeat_mode() const {return repeat_mode_;}
protected:
  void run();
private:
  std::wstring source_file_path_;
  std::unique_ptr<async_reader> reader_;
  uint64_t position_;//,output_counter_;
  uint32_t status_backup_;
  bool repeat_mode_;// リピートモード
  bool not_enqueue_;
};
}

