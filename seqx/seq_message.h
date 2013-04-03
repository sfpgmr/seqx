#pragma once
/** @file
 *  @brief MIDI 
 */
#include "midi_message.h"
namespace sf {
   
  typedef boost::uint32_t delta_time_t;

  /** シーケンス・メッセージのベースクラス */
  struct seq_msg_base
  {
    seq_msg_base(const delta_time_t step_time_value) 
      : step_time_(step_time_value) {}

    seq_msg_base(const seq_msg_base& src ): step_time_(src.step_time_) {}

    const uint64_t step_time() const { return step_time_;}
    void step_time(const boost::uint64_t value) {step_time_ = value;}

    // メッセージのname
    virtual const std::wstring& name()  = 0;
    // description
    virtual const std::wstring& description()  = 0;
  protected:
    delta_time_t step_time_;
  };

  struct note_msg :  public seq_msg_base
  {
      note_msg(delta_time_t step_time,uint8_t note,uint8_t velocity,delta_time_t gate_time) 
          : seq_msg_base(step_time),note_(note),velocity_(velocity)
      {
      }
  private:
    delta_time_t gate_time_;
    uint8_t note_;
    uint8_t velocity_;
  };

  struct bar_msg : public seq_msg_base
  {
    bar_msg(delta_time_t step_time) : seq_msg_base(step_time){}
  };
}
