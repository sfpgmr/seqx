#pragma once
/** @file
*  @brief MIDI Outputのヘッダーファイル
*/
//#include "midi_message.h"
#include "exception.h"

namespace sf {

  /** デバイスエラー時に投げる例外 */
  struct midi_output_error : sf::exception 
  {
    midi_output_error(const uint32_t error) : error_id_(error),exception(get_error_string(error)){}
    const int error_id() const {return error_id_;}
  private:
    const int error_id_;
    const std::wstring get_error_string(uint32_t id);
  };
  
  /** デバイスが存在しない時に投げる例外 */
  struct midi_output_device_not_found : sf::exception 
  {
    explicit midi_output_device_not_found()  : exception(L"MIDI Input Deviceが見つかりません。"){}
  };

  inline void throw_midi_output_error(const uint32_t error)
  {
    if(MMSYSERR_NOERROR !=  error)
    {
      throw midi_output_error(error);
    }
  };

  /** MIDI Output Class */
  struct midi_output
  {
    
    midi_output(uint32_t id);
    virtual ~midi_output();

    //void id(const uint32_t id) { assert(id < device_infos_.size()); dev_id_ = id;}
    const uint32_t id() {return dev_id_;} 

    const uint32_t num_of_devices() {return ::midiOutGetNumDevs();}

    void open()
    {
      if(num_of_devices() > 0){
        int result = ::midiOutOpen(&hmidiout_,dev_id_,(LONG_PTR)(&midi_out_proc),(DWORD_PTR)this,CALLBACK_FUNCTION);
        assert(result == MMSYSERR_NOERROR);
      } else {
        throw midi_output_device_not_found();
      }
    }

    void reset()
    {
      assert(hmidiout_ != NULL);
      uint32_t result = midiOutReset(hmidiout_);
      assert(result == MMSYSERR_NOERROR);
      throw_midi_output_error(result);
    }

    void close()
    {
      if(hmidiout_ == NULL) return;

      uint32_t result = midiOutClose(hmidiout_);
      assert(result == MMSYSERR_NOERROR);
      hmidiout_ = NULL;
      throw_midi_output_error(result);
    }

    static void CALLBACK midi_out_proc(
      HMIDIOUT HMIDIOUT,  
      uint32_t wMsg,        
      DWORD dwInstance, 
      DWORD dwParam1,   
      DWORD dwParam2    
      ){
        ::OutputDebugStringW(L"output_proc\n");
        //reinterpret_cast<midi_output*>(dwInstance)->midi_message_arrived(wMsg,dwParam1,dwParam2);
    }
    static void enum_devices();
    HMIDIOUT handle() const {return hmidiout_;}

    /** send_midi_message */
    void send_midi_message(uint32_t data)
    {
      ::midiOutShortMsg(handle(),data);
    }
    /** エクスクルーシブ */
    void  send_midi_message(const std::vector<uint8_t>& exclusive_data)
    {
      BOOST_ASSERT(!exclusive_data.empty());
      MIDIHDR header;
      header.dwBufferLength = exclusive_data.size() ? 65536 : 0;
      // TODO: 頑張って実装する。
      ::midiOutLongMsg(handle(),&header,sizeof(MIDIHDR));
    }
  private:
//    uint32_t control_change_buffer_[midi_ch_num][control_change_num];
    HMIDIOUT hmidiout_;
    uint32_t dev_id_;
  };
}
