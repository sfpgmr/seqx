/** @file
*  @brief MIDI Output 実装クラス
*/
#include "stdafx.h"
#include <mmsystem.h>
#include "midi_output.h"

namespace sf {
  
  const std::wstring midi_output_error::get_error_string(uint32_t id)
  {
    wchar_t buf[MAXCHAR] = {};
    ::midiOutGetErrorTextW(id,buf,MAXCHAR);
    return std::wstring(buf);
  }

  midi_output::midi_output(uint32_t id) 
    : dev_id_(id),hmidiout_(NULL)
  {
    //open();
  }

  midi_output::~midi_output()
  {
    close();
  }
 }
