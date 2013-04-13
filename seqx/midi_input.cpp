/** @file
*  @brief MIDI Input 実装クラス
*/
#include "stdafx.h"
//#include "seq_message.h"
#include <mmsystem.h>
#include "midi_input.h"

namespace sf {
  
  const std::wstring midi_input_error::get_error_string(uint32_t id)
  {
    wchar_t buf[MAXCHAR] = {};

    ::midiInGetErrorTextW(id,buf,MAXCHAR);
    return std::wstring(buf);
  }

  /** コンストラクタ */
  midi_input::midi_input(uint32_t id) 
    : dev_id_(id),hmidiin_(NULL)
  {
    memset(&midi_buffer_info_,0,sizeof(midi_buffer_info_));
    midi_buffer_info_.lpData  = (LPSTR)exclusive_data_buffer_;
    midi_buffer_info_.dwBufferLength = size_of_buffer_;
//    open();
  }

  /** デストラクタ */
  midi_input::~midi_input()
  {
    reset();
    stop();
    close();
  }

 }
