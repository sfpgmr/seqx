#include "stdafx.h"

#include "midi_device_manager.h"

namespace sf{

midi_device_manager::midi_device_manager()
{
  enum_device_infos();
}

void midi_device_manager::enum_device_infos()
{
  // MIDI IN デバイスの列挙
  {
    const uint32_t num_devs_ = midiInGetNumDevs();
    if(num_devs_ > 0)
    {
      midi_input_device_infos_.clear();
      for( uint32_t dev_id_ = 0;dev_id_ < num_devs_;++dev_id_)
      {
        MIDIINCAPS2 caps2_;
        uint32_t result = midiInGetDevCaps(dev_id_,reinterpret_cast<LPMIDIINCAPS>(&caps2_),sizeof(MIDIINCAPS2));
        if(result != MMSYSERR_NOERROR)
        {
           throw midi_input_error(result);
        } else {
          midi_input_device_infos_.push_back(new midi_input_device_t(caps2_,dev_id_));
        }
      }
    }
  }

  // MIDI OUT デバイスの列挙
  {
    const uint32_t num_devs_ = midiOutGetNumDevs();
    if(num_devs_ > 0)
    {
      midi_output_device_infos_.clear();
      for( uint32_t dev_id_ = 0;dev_id_ < num_devs_;++dev_id_)
      {
        MIDIOUTCAPS2 device_infos_2_;
        uint32_t result = midiOutGetDevCaps(dev_id_,reinterpret_cast<LPMIDIOUTCAPS>(&device_infos_2_),sizeof(MIDIOUTCAPS2));
        if(result != MMSYSERR_NOERROR)
        {
           throw midi_output_error(result);
        } else {
          midi_output_device_infos_
            .push_back(new midi_output_device_t(device_infos_2_,dev_id_));
        }
      }
    }

  }

}

midi_device_manager::~midi_device_manager()
{
}

}
