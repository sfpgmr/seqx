#pragma once
/*
  ==============================================================================

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
#include "exception.h"
namespace sf {
struct audio_base : boost::noncopyable
{
  virtual ~audio_base() {};
  virtual bool is_enabled () const  = 0;
  /// イベントモードの待ち
  virtual void wait() = 0;
  /// サウンド再生処理
 // virtual size_t process(BYTE* buffer,size_t size) = 0;
  virtual bool is_start() = 0;
  virtual uint32_t get_buffer(BYTE** buffer,uint32_t size) = 0;
  virtual uint32_t get_buffer(BYTE** buffer) = 0;
  virtual void release_buffer(uint32_t num_byte_written) = 0;
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void reset() = 0;
  virtual uint32_t get_buffer_byte_size () const  = 0;
  virtual uint32_t get_frame_size() const = 0;
  virtual uint32_t get_buffer_size () const  = 0;
  virtual uint32_t get_current_padding()  = 0;
  virtual uint64_t get_position() const = 0;
  virtual uint64_t get_buffer_duration() = 0;
  virtual uint32_t get_channel() = 0;
  virtual win32_error_exception* const result()  = 0;
};

struct get_buffer 
{
  get_buffer(audio_base& b,uint32_t s) : audio_base_(b),buffer_size_(0)
  {
    buffer_size_ = b.get_buffer(&buffer_ptr_,s);
  }

  get_buffer(audio_base& b) : audio_base_(b),buffer_size_(0)
  {
    buffer_size_ = b.get_buffer(&buffer_ptr_);
  }

  ~get_buffer()
  {
    if(buffer_ptr_ != nullptr && buffer_size_ != 0){
      audio_base_.release_buffer(buffer_size_);
    }
  }
 
  operator BYTE*(){return buffer_ptr_;}
  BYTE* get() {return buffer_ptr_;}
  const uint32_t size() const { return buffer_size_;}
  const uint32_t size_byte() const {
    return buffer_size_ * audio_base_.get_frame_size();
//    return audio_base_.get_buffer_byte_size();
  }
private:
  uint32_t buffer_size_;
  BYTE* buffer_ptr_;
  audio_base& audio_base_;
};
}

