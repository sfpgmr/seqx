#pragma once
/*
*/
// Windows Header Files:
#include "base_window.h"
#include "sf_windows.h"
#include "icon.h"
#include "taskbar.h"
#include "tab_dialog.h"

namespace sf
{

  struct config_tab_dialog : public tab_dialog_base
  {
    config_tab_dialog(
      sf::base_window& parent_window,
      HWND tab_hwnd,
      int tab_id,
      const std::wstring& menu_name,
      const std::wstring& name,
      HINSTANCE inst,
      LPCTSTR temp);
    virtual ~config_tab_dialog(){};
    virtual LRESULT window_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam);
    virtual void enable();
    virtual void disable();
  private:
    void init_config_dialog();
    void update_config_dialog();
    void enable_config_dialog(bool value);

    int current_output_driver_index_;
    int current_input_driver_index_;
 
    bool update_config_dialog_;
    bool edit_output_config_;
    bool edit_input_config_;

    wasapi_device_manager::device_info::params_t editing_output_params_;
    wasapi_device_manager::device_info::params_t editing_input_params_;

  };
}