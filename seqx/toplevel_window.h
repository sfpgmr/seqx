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

  struct toplevel_window;
  typedef std::shared_ptr<toplevel_window> toplevel_window_ptr;

  /** toplevel_window を生成する関数 */
  toplevel_window_ptr create_toplevel_window (
    const std::wstring& menu_name,
    const std::wstring& name,
    const uint32_t show_flag = SW_SHOW,
    bool fit_to_display = false,
    float width = 640,
    float height = 480
    );
  /** toplevel_window を生成する関数 */
  void dialogbox (
    const std::wstring& menu_name,
    const std::wstring& name
    );

  /** toplevel ウィンドウクラス */
  /* このクラスは、create_toplevel_window 関数からのみ生成可能 */
  struct toplevel_window : public base_window
  {

    friend   toplevel_window_ptr create_toplevel_window
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

    ~toplevel_window(){};
 
    void * raw_handle() const;
    void create();
    void toplevel_window::show();
    bool toplevel_window::is_show();
    void toplevel_window::hide();
    void message_box(const std::wstring& text,const std::wstring& caption,uint32_t type = MB_OK);
    void text(std::wstring& text);
    void update();
    void render();

    void player_ready();
    void player_read_file();
    void player_pause();
    void player_stop();
   
    void enable_control(uint32_t id,bool enable);

  private:
    struct impl;
    toplevel_window(const std::wstring& menu_name,const std::wstring& name,bool fit_to_display,float width = 800 ,float height = 600);
    // 実装部
    std::shared_ptr<impl> impl_;
  };
}