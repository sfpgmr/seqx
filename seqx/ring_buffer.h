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
#include <malloc.h>
#include <atomic>
#include <boost/lockfree/spsc_queue.hpp>
namespace sf {

  template <typename T>
  void reset_ringbuffer(T& r)
  {
    T::value_type v;
    while(!r.empty())
    {
      r.pop(v);
    }
  }
  // アラインされたメモリの解放
  struct buffer_deleter{
    void operator()(uint8_t *mem)
    {
      _aligned_free(mem);
    };
  };

  // バッファメモリのスマートポインタ
  // アラインされたメモリを想定
  typedef std::unique_ptr<uint8_t,buffer_deleter> memory_block_t;

  // メモリブロックの配列
  // ストレージからのデータはここに一時的に格納する
  // この配列valueのunique_ptrをget()してリングバッファに渡す
  typedef boost::array<memory_block_t,16> buffer_t;

  // リングバッファ マルチスレッド対応
  typedef boost::lockfree::spsc_queue<uint8_t*,boost::lockfree::capacity<8> > ringbuffer_t;

}

