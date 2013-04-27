#pragma once
/*
*/
// Windows Header Files:
#include "sf_windows.h"
#include "icon.h"
#include "base_window.h"
#include "taskbar.h"

#define WM_PLAY_STOP WM_USER + 1
#define WM_PLAY_PAUSE WM_USER + 2
#define WM_PLAY_PLAY WM_USER + 3

namespace sf
{

  struct dcomposition_window;
  typedef std::shared_ptr<dcomposition_window> dcomposition_window_ptr;

  /** dcomposition_window を生成する関数 */
  dcomposition_window_ptr create_dcomposition_window (
    const std::wstring& menu_name,
    const std::wstring& name,
    const uint32_t show_flag = SW_SHOW,
    bool fit_to_display = false,
    float width = 640,
    float height = 480
    );
  /** dcomposition_window を生成する関数 */
  void dialogbox (
    const std::wstring& menu_name,
    const std::wstring& name
    );

  /** toplevel ウィンドウクラス */
  /* このクラスは、create_dcltoplevel_window 関数からのみ生成可能 */
  struct dcomposition_window : public base_window
  {

    friend   dcomposition_window_ptr create_dcomposition_window
      (
      const std::wstring& menu_name,
      const std::wstring& name,
      const uint32_t show_flag,
      bool fit_to_display,
      float width ,
      float height
      );

    friend void dialogbox (
      const std::wstring& menu_name,
      const std::wstring& name
    );

    ~dcomposition_window(){};
 
    void * raw_handle() const;
    void create();
    void dcomposition_window::show();
    bool dcomposition_window::is_show();
    void dcomposition_window::hide();
    void message_box(const std::wstring& text,const std::wstring& caption,uint32_t type = MB_OK);
    void text(std::wstring& text);
    void update();
    void render();

  private:
    struct impl;
    dcomposition_window(const std::wstring& menu_name,const std::wstring& name,bool fit_to_display,float width = 800 ,float height = 600);
    // 実装部
    std::shared_ptr<impl> impl_;
  };
}