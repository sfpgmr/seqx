/** @file
*  @brief MIDI Output 実装クラス
*/
#include "stdafx.h"
#include <mmsystem.h>
#include "midi_output.h"

namespace sf {
  
  midi_output::device_infos_t midi_output::devices_;

  const std::wstring midi_output_error::get_error_string(uint32_t id)
  {
    wchar_t buf[MAXCHAR] = {};

    ::midiOutGetErrorTextW(id,buf,MAXCHAR);
    return std::wstring(buf);
  }



  void midi_output::enum_devices()
  {
    const uint32_t num_devs_ = midiOutGetNumDevs();
    if(num_devs_ > 0)
    {
      devices_.clear();
      for( uint32_t dev_id_ = 0;dev_id_ < num_devs_;++dev_id_)
      {
        MIDIOUTCAPS2 device_infos_2_;
        uint32_t result = midiOutGetDevCaps(dev_id_,reinterpret_cast<LPMIDIOUTCAPS>(&device_infos_2_),sizeof(MIDIOUTCAPS2));
        if(result != MMSYSERR_NOERROR)
        {
           throw midi_output_error(result);
        } else {
          midi_output::devices_
            .push_back(new midi_output::device_infos_(device_infos_2_,dev_id_));
        }
      }
    }

  }
  midi_output::midi_output(uint32_t id) 
    : dev_id_(id),hmidiout_(NULL)
  {
    open();
  }

  midi_output::~midi_output()
  {
    close();
  }
 }
