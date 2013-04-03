#pragma once
/*
*/
// Windows Header Files:
#include "sf_windows.h"
#include "icon.h"
#include "base_window.h"
#include "taskbar.h"

namespace sf
{

  struct tab_dialog_base;
  typedef std::shared_ptr<tab_dialog_base> tab_dialog_ptr;

  /** toplevel ウィンドウクラス */
  /* このクラスは、create_tab_dialog 関数からのみ生成可能 */
  struct tab_dialog_base : public base_win32_dialog_t
  {
    tab_dialog_base(sf::base_window& parent_window,HWND tab_hwnd,int tab_id,const std::wstring& menu_name,const std::wstring& name,HINSTANCE inst,LPCTSTR temp);
    virtual ~tab_dialog_base(){};
    virtual void create() ;
    virtual void enable() = 0;
    virtual void disable() = 0;
    virtual void resize();
  protected:
    base_window& parent_window_;
  private:
    HWND tab_hwnd_;
    int tab_id_;
    HINSTANCE inst_;
    LPCTSTR temp_;
  };
}