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
#include "audio_filter.h"
#include "samplerate.h"
namespace sf
{
class sample_rate_converter :
  public audio_filter
{
public:
   enum
      {    
          BEST_QUALITY       = 0,
          MEDIUM_QUALITY     = 1,
          SINC_FASTEST            = 2,
          ZERO_ORDER_HOLD         = 3,
          LINEAR                  = 4
      } ;
  sample_rate_converter(double input_rate,double output_rate,int channel,int converter_type = BEST_QUALITY);
  ~sample_rate_converter();
  void apply(float * in,float * out,size_t size);
  void end_of_data(bool value) {data_.end_of_input = value?1:0;}
  void reset(){ src_reset(state_);};
private:
  SRC_STATE* state_;
  SRC_DATA data_;
};
}

