#pragma once

namespace sf 
{
  struct midi_device_manager;
  template <typename CAPS,typename MIDIDeviceClass>
  struct device_info_base {
    friend class midi_device_manager;
    device_info_base(const CAPS& caps,const uint32 id) : id_(id),name_(caps.szPname)
    {
      device_ptr_.reset(new MIDIDeviceClass(id));
    }
    const uint32 id() const {return id_;}
    const std::wstring& name() const {return name_;}
    const CAPS& caps() const {return caps_:}
    const std::unique_ptr<MIDIDeviceClass>& device_ptr() const {return device_ptr_;}
  private:
    const uint32 id_;
    const std::wstring name_;
    const CAPS caps_;
    mutable std::unique_ptr<MIDIDeviceClass> device_ptr_;
  };
}