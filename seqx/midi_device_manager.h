#pragma once
#include "midi_base.h"
#include "midi_input.h"
#include "midi_output.h"
namespace sf {

 typedef device_info_base<MIDIINCAPS2W,midi_input> midi_input_device_t;
 typedef boost::ptr_vector<midi_input_device_t> midi_input_device_infos_t;
 typedef device_info_base<MIDIOUTCAPS2W,midi_output> midi_output_device_t;
 typedef boost::ptr_vector<midi_output_device_t> midi_output_device_infos_t;

class midi_device_manager : public singleton<midi_device_manager>
{
public:
  midi_device_manager();
  ~midi_device_manager();

  const midi_input_device_infos_t& midi_input_device_infos(){return midi_input_device_infos_;};
  const midi_output_device_infos_t& midi_output_device_infos(){return midi_output_device_infos_;};

private:

  void enum_device_infos();

  midi_input_device_infos_t midi_input_device_infos_;
  midi_output_device_infos_t midi_output_device_infos_;
};

}

