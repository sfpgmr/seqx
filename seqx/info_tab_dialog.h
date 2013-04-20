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

  struct info_tab_dialog : public tab_dialog_base
  {
    info_tab_dialog(
      sf::base_window& parent_window,
      HWND tab_hwnd,
      int tab_id,
      const std::wstring& menu_name,
      const std::wstring& name,
      HINSTANCE inst,
      LPCTSTR temp);
    virtual ~info_tab_dialog();
    virtual LRESULT window_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam);
    virtual void enable(){};
    virtual void disable(){};
  private:

    void create_device_independent_resources();
    void create_device();
    void discard_device();
    void render();

    ID2D1FactoryPtr d2d_factory_;
    ID2D1HwndRenderTargetPtr render_target_;
    IDWriteFactoryPtr write_factory_;
    IWICImagingFactoryPtr wic_imaging_factory_;
    IDWriteTextFormatPtr write_text_format_;
    D2D1_RECT_F layout_rect_;

  };
}