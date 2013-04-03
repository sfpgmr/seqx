#pragma once
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
namespace sf{

  class audio_source
  {
  public:
    virtual ~audio_source() {};
    virtual bool seekable() = 0;
    virtual bool stream_status() = 0;
    virtual WAVEFORMATEXTENSIBLE &get_wave_format() = 0;
    virtual bool more_data_available() = 0;
    virtual void read_data(BYTE *buffer, uint64_t numbytes) = 0;
    virtual void reset_data_position() = 0;
    virtual uint64_t total_data_bytes() = 0;
    virtual void seek(uint64_t pos) = 0;
    virtual uint64_t data_bytes_remaining() = 0;
    virtual HANDLE raw_handle() = 0;
    virtual void wait(int timer = -1) = 0;
  };
}