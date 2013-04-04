#pragma once
/** @file
*  @brief MIDI Inputのヘッダーファイル
*/
//#include "midi_message.h"
#include "exception.h"

namespace sf {

  const int midi_ch_num = 16;
  const int control_change_num = 2;
  const int max_control_changes = 128;


  /** デバイスエラー時に投げる例外 */
  struct midi_input_error : sf::exception 
  {
    midi_input_error(const uint32_t error) : error_id_(error),exception(get_error_string(error)){}
    const int error_id() const {return error_id_;}
  private:
    const int error_id_;
    const std::wstring get_error_string(uint32_t id);
  };
  
  /** デバイスが存在しない時に投げる例外 */
  struct midi_input_device_not_found : sf::exception 
  {
    explicit midi_input_device_not_found()  : exception(L"MIDI Input Deviceが見つかりません。"){}
  };

  inline void throw_midi_input_error(const uint32_t error)
  {
    if(MMSYSERR_NOERROR !=  error)
    {
      throw midi_input_error(error);
    }
  };

  /** MIDI Input Class */
  struct midi_input
  {
    
    typedef boost::signals2::signal<void (uint32_t wMsg,uint32_t midi_message,uint32_t time_stamp) > midi_message_arrived_t;
    midi_message_arrived_t midi_message_arrived;


    /** MIDI IN インターフェース*/
    struct caps {
      caps(const MIDIINCAPS2& value,const uint32_t dev_id) :id_(dev_id),name_(value.szPname),device_info_(value)
      {

      }
      const uint32_t id() const { return id_;}
      const std::wstring& name() const  {return name_;}
    private:
      const uint32_t id_;
      const std::wstring name_;
      MIDIINCAPS2 device_info_;
    };

    typedef boost::ptr_vector<caps> device_infos_t;

    midi_input(uint32_t id);
    virtual ~midi_input();

    //void id(const uint32_t id) { assert(id < device_infos_.size()); dev_id_ = id;}
    const uint32_t id() {return dev_id_;} 

    const uint32_t num_of_devices() {return ::midiInGetNumDevs();}

    /** ドライバのopen */
    void open()
    {
      if(num_of_devices() > 0){
        int result = midiInOpen(&hmidiin_,dev_id_,(LONG_PTR)(&midi_in_proc),(DWORD_PTR)this,CALLBACK_FUNCTION);
        assert(result == MMSYSERR_NOERROR);
      } else {
        throw midi_input_device_not_found();
      }
      ::midiInPrepareHeader(hmidiin_,&midi_buffer_info_,sizeof(midi_buffer_info_));
    }

    /** start */
    void start()
    {
      assert(hmidiin_ != NULL);

//      memset(control_change_buffer_,0,sizeof(uint32_t) * midi_ch_num * control_change_num );

      uint32_t result = midiInStart(hmidiin_);
      assert(result == MMSYSERR_NOERROR);
      throw_midi_input_error(result);
    }

    /** stop */
    void stop()
    {
      if(hmidiin_ == NULL) return;
      assert(hmidiin_ != NULL);
      uint32_t result = midiInStop(hmidiin_);
      assert(result == MMSYSERR_NOERROR);
      throw_midi_input_error(result);
    }

    /** reset */
    void reset()
    {
      if(hmidiin_ == NULL) return;
      assert(hmidiin_ != NULL);
      uint32_t result = midiInReset(hmidiin_);
      assert(result == MMSYSERR_NOERROR);
      throw_midi_input_error(result);
    }

    /** close */
    void close()
    {
      if(hmidiin_ == NULL) return;

      uint32_t result = midiInClose(hmidiin_);
      assert(result == MMSYSERR_NOERROR);
      hmidiin_ = NULL;
      throw_midi_input_error(result);
      ::midiInUnprepareHeader(hmidiin_,&midi_buffer_info_,sizeof(midi_buffer_info_));
    }

    /** device_infos_のコンテナを返す */
    static const device_infos_t & device_infos(){return device_infos_;};

    /** MIDI IN コールバック */
    static void CALLBACK midi_in_proc(
      HMIDIIN hMidiIn,  
      uint32_t wMsg,        
      DWORD dwInstance, 
      DWORD dwParam1,   
      DWORD dwParam2    
      ){
        //::OutputDebugStringW(L"input_proc\n");
        reinterpret_cast<midi_input*>(dwInstance)->midi_message_arrived(wMsg,dwParam1,dwParam2);
    }
    /** device_infos_を列挙し、コンテナに情報を格納する */
    static void enum_devices();

    /** ハンドルを返す */
    HMIDIIN handle() const {return hmidiin_;}
  private:
    /** device_infos_が入るコンテナ */
    static device_infos_t device_infos_;
//    uint32_t control_change_buffer_[midi_ch_num][control_change_num];
    /** デバイスのhandle */
    HMIDIIN hmidiin_;
    /** デバイスのID */
    uint32_t dev_id_;

    static const int size_of_buffer_ = 16384;//16K
    ::MIDIHDR midi_buffer_info_;
    /** エクスクルーシブ用バッファ */
    uint8_t exclusive_data_buffer_[size_of_buffer_];
  };
}
