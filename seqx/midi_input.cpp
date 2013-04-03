/** @file
*  @brief MIDI Input 実装クラス
*/
#include "stdafx.h"
//#include "seq_message.h"
#include <mmsystem.h>
#include "midi_input.h"

namespace sf {
  
  midi_input::device_infos_t midi_input::device_infos_;

  const std::wstring midi_input_error::get_error_string(boost::uint32_t id)
  {
    wchar_t buf[MAXCHAR] = {};

    ::midiInGetErrorTextW(id,buf,MAXCHAR);
    return std::wstring(buf);
  }

  /** デバイスを列挙して、情報をコンテナに収める */
  void midi_input::enum_devices()
  {
    const boost::uint32_t num_devs_ = midiInGetNumDevs();
    if(num_devs_ > 0)
    {
      device_infos_.clear();
      for( boost::uint32_t dev_id_ = 0;dev_id_ < num_devs_;++dev_id_)
      {
        MIDIINCAPS2 caps2_;
        boost::uint32_t result = midiInGetDevCaps(dev_id_,reinterpret_cast<LPMIDIINCAPS>(&caps2_),sizeof(MIDIINCAPS2));
        if(result != MMSYSERR_NOERROR)
        {
           throw midi_input_error(result);
        } else {
          midi_input::device_infos_.push_back(new midi_input::caps(caps2_,dev_id_));
        }
      }
    }

  }

  /** コンストラクタ */
  midi_input::midi_input(boost::uint32_t id) 
    : dev_id_(id),hmidiin_(NULL)
  {
    memset(&midi_buffer_info_,0,sizeof(midi_buffer_info_));
    midi_buffer_info_.lpData  = (LPSTR)exclusive_data_buffer_;
    midi_buffer_info_.dwBufferLength = size_of_buffer_;
    open();
  }

  /** デストラクタ */
  midi_input::~midi_input()
  {
    reset();
    stop();
    close();
  }

 
 }
