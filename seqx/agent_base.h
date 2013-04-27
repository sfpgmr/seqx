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
#include "singleton.h"
#include "exception.h"
#include "ring_buffer.h"
#include "sf_memory.h"
namespace sf 
{


  struct agent_base : public Concurrency::agent
  {
    struct wait_exception
      : public sf::exception 
    {
      wait_exception() : exception(L"wait_error") {}
    };

    agent_base()  // : event_((::CreateEventEx(NULL, NULL, 0,EVENT_MODIFY_STATE | SYNCHRONIZE))
    {
      status_.store(0, std::memory_order_seq_cst);
    }

    virtual ~agent_base(){};

    //void execute()
    //{
    //  //g.run(std::bind(&agent_base::thread_main,this));
    //  thread_ = std::thread(boost::bind(&agent_base::thread_main,this));
    //}


    //void join ()
    //{
    //  if(thread_.joinable())
    //  {
    //    thread_.join();
    //  }
    //}

    // èÛë‘ÇÃïœçX
    void change_status(int value,std::memory_order o = std::memory_order_seq_cst)
    {
      event_.reset();
      status_.store(value,o);
      event_.set();
      //DOUT(L"change_status()" << std::endl);
    }

    void wait_event()
    {
      event_.wait();
      event_.reset();
    }

    void change_and_wait(int value,int wait_value,std::memory_order o = std::memory_order_seq_cst)
    {
      change_status(value,o);
      //PulseEvent(event_.get());
      wait_status(wait_value);
    }

    Concurrency::agent_status agent_status(){
      return agent::status();
    }

    int status(std::memory_order o = std::memory_order_seq_cst)
    {
      return status_.load(o);
    };

    virtual void init_buffer();

    void wait_status(int value,int ms = 1,std::memory_order o = std::memory_order_seq_cst)
    {
     // DOUT(L"wait_status()" << std::endl);
      int s;
      while(s = status_.load(o),s != value)
      {
        if(s < STATUS_ERROR)
        {
          throw wait_exception();
        };
        Sleep(ms);
      }
    };

    ringbuffer_t& ringbuffer() {return ringbuffer_;}

    static const int STATUS_ERROR = -1;
    static const size_t Q_SIZE = 8;
    static const int WAIT_TIMEOUT_DEFAULT = 100; 

  protected:
    virtual void run() = 0;

    //std::thread thread_;
    std::atomic<int> status_;
    buffer_t buffer_;
    Concurrency::event event_;
    //handle_holder event_;
    uint32_t index_;
    ringbuffer_t  ringbuffer_;
    std::wstring error_;
  };


}
