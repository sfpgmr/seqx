/*
==============================================================================

Copyright 2005-11 by Satoshi Fujiwara.

async can be redistributed and/or modified under the terms of the
GNU General Public License, as published by the Free Software Foundation;
either version 2 of the License, or (at your option) any later version.

async is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with async; if not, visit www.gnu.org/licenses or write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
Boston, MA 02111-1307 USA

==============================================================================
*/
/* ToDo

TODO: リサイズに対応する

*/

#include "stdafx.h"
#include "resource.h"
#define BOOST_ASSIGN_MAX_PARAMS 7
#include <boost/assign.hpp>
#include <boost/assign/ptr_list_of.hpp>
#include <boost/assign/ptr_list_inserter.hpp>
#include <boost/foreach.hpp>

#include "sf_windows.h"
#include "toplevel_window.h"
#include "CommDlg.h"
#include "icon.h"
#include "timer.h"
#include "exception.h"
#include "application.h"
#include "config_tab_dialog.h"
#include "info_tab_dialog.h"
#include "midi_config_tab_dialog.h"

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


//#include "seq_tab_dialog.h"

//#pragma comment( lib, "dxguid.lib" )
//#pragma comment( lib, "d3d11.lib" )
////#pragma comment( lib, "d3dx11.lib" )
//#pragma comment( lib, "dxgi.lib" )
//#pragma comment( lib, "d3dx9.lib" )   
//#pragma comment( lib, "Shlwapi.lib" ) 


#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

namespace sf 
{


  struct simple_vertex
  {
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 norm;
    DirectX::XMFLOAT2 tex;
  };

  struct cb_never_changes
  {
    DirectX::XMMATRIX mView;
    DirectX::XMFLOAT4 vLightDir;
  };

  struct cb_change_on_resize
  {
    DirectX::XMMATRIX mProjection;
  };

  struct cb_changes_every_frame
  {
    DirectX::XMMATRIX mWorld;
    DirectX::XMFLOAT4 vLightColor;

    //    DirectX::XMFLOAT4 vMeshColor;
  };

  struct slider : public subclass_window
  {
    slider() : subclass_window() {};
    void create() {};
    virtual LRESULT window_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam) 
    {
      //switch(message){
      //case WM_HSCROLL :
      //  {
      //    switch(wParam)
      //    {
      //    case TB_THUMBTRACK : 
      //       DWORD dwPos = SendMessage(GetDlgItem(hwnd_,IDC_SLIDER), TBM_GETPOS, 0, 0); 
      //       break; 
      //    }
      //  }
      /*   }*/
      return CallWindowProc(proc_backup_,hwnd,message,wParam,lParam);
    };
  };

  struct toplevel_window::impl : public base_win32_window_t
  {

    impl(const std::wstring& menu_name,const std::wstring& name,bool fit_to_display,float width = 160,float height = 100) 
      : base_win32_window_t(menu_name,name,fit_to_display,width,height) 
      , timer_(*this,100)
      , icon_(IDI_ICON1)
      /*,mesh_color_(0.7f, 0.7f, 0.7f, 1.0f)*/
      ,init_(false)
      ,thumb_start_(false)
    {

      on_render.connect(boost::bind(&impl::render,this));
    };

    ~impl(){
      //safe_release(dxgi_factory_);
    };


    // -------------------------------------------------------------------
    // ウィンドウプロシージャ
    // -------------------------------------------------------------------

    void init_control()
    {
      //      application::instance()->player_init_ok().connect(boost::bind(&toplevel_window::impl::player_init_ok,this));

      DWORD dwDlgBase = GetDialogBaseUnits();
      int cxMargin = LOWORD(dwDlgBase) / 4; 
      int cyMargin = HIWORD(dwDlgBase) / 8;

      //GetRect(

      //情報表示ダイアログの作成・表示
      //hwnd_info_ = CreateDialogW(HINST_THISCOMPONENT,MAKEINTRESOURCE(IDD_INFO),hwnd_,proc_info_);
      //SetWindowPos(hwnd_info_,HWND_TOP,pt.x + cxMargin,pt.y + cyMargin,r.right - cxMargin * 2 - 1,r.bottom - rci.bottom - cyMargin * 2 - 1,SWP_NOZORDER | SWP_SHOWWINDOW);

      //TODO: 設定ダイアログの作成・表示
      //hwnd_config_ = CreateDialogW(HINST_THISCOMPONENT,MAKEINTRESOURCEW(IDD_CONFIG),hwnd_,proc_config_);
      //SetWindowPos(hwnd_config_,HWND_TOP,pt.x + cxMargin,pt.y + cyMargin,r.right - cxMargin * 2 - 1,r.bottom - rci.bottom - cyMargin * 2 - 1,SWP_NOZORDER | SWP_SHOWWINDOW);
      ////MapDialogRect(hwnd_config_,&r);
      //ShowWindow(hwnd_config_,FALSE);
      //config_dialog_.reset();

      // 背景色の設定
      //get_dc tab_dc(tab);
      //get_dc info_dc(hwnd_info_);
      //get_dc config_dc(hwnd_config_);
      //SetBkMode(info_dc.get(),GetBkMode(tab_dc.get()));
      //SetBkMode(config_dc.get(),GetBkMode(tab_dc.get()));
      //SetBkColor(info_dc.get(),GetBkColor(tab_dc.get()));
      //SetBkColor(config_dc.get(),GetBkColor(tab_dc.get()));

      //slider_.attach(GetDlgItem(hwnd_,IDC_SLIDER));
    }

    virtual void create(){
      create_device_independent_resources();
      //    icon_ = ::LoadIconW(HINST_THISCOMPONENT,MAKEINTRESOURCE(IDI_ICON1));
      register_class(this->name_.c_str(),CS_HREDRAW | CS_VREDRAW ,0,0,icon_.get());
      create_window();
      text(name_);

      // 半透明ウィンドウを有効にする。
      //BOOL dwmEnable;
      //DwmIsCompositionEnabled (&dwmEnable); 
      //if (dwmEnable) EnableBlurBehind(*this);

    }

    void calc_client_size()
    {
      //クライアント領域の現在の幅、高さを求める
      RECT rc;
      GetClientRect( hwnd_, &rc );
      client_width_ = rc.right - rc.left;
      client_height_ = rc.bottom - rc.top;
    }

    void render(){
     if(activate_){
      d2d_context_->BeginDraw();
      d2d_context_->Clear();


      //thunk_proc_ = (WNDPROC)thunk_.getCode();
      D2D_RECT_F layout_rect_ = D2D1::RectF(0.0f,100.0f,400.0f,100.0f);
      // Text Formatの作成
      THROW_IF_ERR(write_factory_->CreateTextFormat(
        L"メイリオ",                // Font family name.
        NULL,                       // Font collection (NULL sets it to use the system font collection).
        DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        48.0f,
        L"ja-jp",
        &write_text_format_
        ));

      d2d_context_->SetTransform(D2D1::Matrix3x2F::Identity());
      ID2D1SolidColorBrushPtr brush,line_brush;
      d2d_context_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::OrangeRed), &brush);
      d2d_context_->CreateSolidColorBrush(D2D1::ColorF(1.0f,1.0f,1.0f,0.5f), &line_brush);

      {
        D2D_POINT_2F start,end;
        for(float i = 0;i < width_+1.0f;i += 16.0f)
        {
          start.x = end.x = i;
          end.y = height_;
          start.y = 0.0f;
          d2d_context_->DrawLine(start,end,line_brush.Get(),0.5f);
        }

        for(float i = 0;i < height_+1.0f;i += 16.0f)
        {
          start.y = end.y = i;
          end.x = width_;
          start.x = 0.0f;
          d2d_context_->DrawLine(start,end,line_brush.Get(),0.5f);
        }

      }

      static int count;
      count++;
      std::wstring m((boost::wformat(L"TEST表示 %d") % count).str());
      d2d_context_->DrawTextW(
        m.c_str(),
        m.size(),
        write_text_format_.Get(),
        layout_rect_, 
        brush.Get());

      d2d_context_->EndDraw();
      DXGI_PRESENT_PARAMETERS parameters = {};
      parameters.DirtyRectsCount = 0;
      parameters.pDirtyRects = nullptr;
      parameters.pScrollRect = nullptr;
      parameters.pScrollOffset = nullptr;
      if(FAILED(dxgi_swap_chain_->Present1(1,0,&parameters)))
      {
        resize_resources();
      };
      }
    }


    //void create_window()
    //{

    //  // Windowを作成する
    //  CreateWindowEx(
    //    WS_EX_APPWINDOW /* | WS_EX_TOPMOST*/,
    //    name_.c_str(),
    //    title_.c_str(),
    //    WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
    //    CW_USEDEFAULT,
    //    CW_USEDEFAULT,
    //    static_cast<uint32_t>(dpi_.scale_x(width_)),
    //    static_cast<uint32_t>(dpi_.scale_x(height_)),
    //    NULL,
    //    NULL,
    //    HINST_THISCOMPONENT,
    //    this
    //    );
    //};


    LRESULT other_window_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam)
    {
      return DefWindowProcW(hwnd,message,wParam,lParam);
    };

    void on_player_event(HWND wnd,UINT_PTR wParam)
    {
      //    application::instance()->Player()->HandleEvent(wParam);
    }

    sf::base_win32_window_t::result_t on_create(CREATESTRUCT *p)
    {
      create_device();
      timer_.start();
      return 0;
    }
    //LRESULT on_init_dialog(HWND default_focus_ctrl,LPARAM data)
    //{
    //  init_control();
    //  create_device();
    //  timer_.start();
    //  //MARGINS mgn = {-1};//left,right,top,bottom
    //  //HRESULT hr = DwmExtendFrameIntoClientArea(hwnd_, &mgn);
    //  return TRUE;
    //}

    LRESULT on_size(uint32_t flag,uint32_t width,uint32_t height)
    {
      // バックバッファなどに関係するインターフェースを解放する
      // バックバッファを解放する
      if(init_)
      {
        int height = client_height_;
        int width = client_width_;

        calc_client_size();

      }
      // バックバッファなどに関係するインターフェースを再作成する

      //if (render_target_)
      //{
      //	D2D1_SIZE_U size;
      //	size.width = lParam & 0xFFFF;
      //	size.height = (lParam >> 16) & 0xFFFF; ;

      //	// Note: This method can fail, but it's okay to ignore the
      //	// error here -- it will be repeated on the next call to
      //	// EndDraw.
      //	render_target_->Resize(size);
      //}
      return TRUE;
    }

    LRESULT on_display_change(uint32_t bpp,uint32_t h_resolution,uint32_t v_resolution)
    {
      invalidate_rect();
      return TRUE;
    }

    LRESULT on_erase_backgroud(HDC dc)
    {
      return FALSE;
    }

    LRESULT on_hscroll(uint32_t state,uint32_t position,HWND ctrl_hwnd)
    {
      return FALSE;
    }

    LRESULT on_destroy()
    {
      ::PostQuitMessage(0);
      return FALSE;
    }

    LRESULT on_close()
    {
      //slider_.detatch();
      timer_.player_stop();
      // 後始末
      discard_device();
      // レンダーターゲットのリリース
      //safe_release(dcr_);
      //      safe_release(render_target_);
      // Windowの破棄
      discard_device_independant_resources();
      BOOL ret(::DestroyWindow(hwnd_));
      BOOST_ASSERT(ret != 0);
      return TRUE;
    }

    LRESULT on_command(uint32_t wparam, uint32_t lparam)
    {
      return FALSE;
    }

    LRESULT on_timer(uint32_t timer_id)
    {
      // TODO:スレッドのエラーチェックも入れておく
      //update();
      //InvalidateRect(hwnd_,NULL,FALSE);
      invalidate_rect();
      return TRUE;
    }

  private:

    timer timer_;
    bool thumb_start_;
    //slider slider_;

    float client_width_,client_height_;

    //IDXGISwapChainPtr dxgi_swap_chain_;
    // std::wstring dxgi_info_;

    icon icon_;
    bool init_;

    D2D1_SIZE_U icon_size_;

    // thisとhwndをつなぐthunkクラス
    // メンバー関数を直接呼び出す。
    struct hwnd_this_thunk2 : public Xbyak::CodeGenerator {
      hwnd_this_thunk2(LONG_PTR this_addr,const void * proc)
      {
        // 引数の位置をひとつ後ろにずらす
        mov(r10,r9);
        mov(r9,r8);
        mov(r8,rdx);
        mov(rdx,rcx);
        // thisのアドレスをrcxに格納する
        mov(rcx,(LONG_PTR)this_addr);
        // 第5引数をスタックに格納
        push(r10);
        sub(rsp,32);
        mov(r10,(LONG_PTR)proc);
        // メンバ関数呼び出し
        call(r10);
        add(rsp,40);
        ret(0);
      }
    };

    //hwnd_this_thunk2 thunk_info_;
    //  hwnd_this_thunk2 thunk_config_;
    //proc_t proc_info_;
    //  proc_t proc_config_;

  };

  //
  // Creates a Direct2D bitmap from the specified
  // file name.
  //
  ID2D1BitmapPtr load_bitmap_from_file(
    ID2D1HwndRenderTargetPtr render_target,
    IWICImagingFactoryPtr wic_factory,
    std::wstring uri,
    uint32_t destination_width,
    uint32_t destination_height
    )
  {
    HRESULT hr = S_OK;

    IWICBitmapDecoderPtr decoder;
    IWICBitmapFrameDecodePtr decoder_source;
    IWICStreamPtr stream;
    IWICFormatConverterPtr converter;
    IWICBitmapScalerPtr scaler;
    ID2D1BitmapPtr bitmap;

    THROW_IF_ERR(wic_factory->CreateDecoderFromFilename(
      uri.c_str(),
      NULL,
      GENERIC_READ,
      WICDecodeMetadataCacheOnLoad,
      &decoder
      ));

    // Create the initial frame.
    THROW_IF_ERR(decoder->GetFrame(0, &decoder_source));

    // Convert the image format to 32bppPBGRA
    // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
    THROW_IF_ERR(hr = wic_factory->CreateFormatConverter(&converter));

    // If a new width or height was specified, create an
    // IWICBitmapScaler and use it to resize the image.
    if (destination_width != 0 || destination_height != 0)
    {
      uint32_t originalWidth, originalHeight;
      THROW_IF_ERR(decoder_source->GetSize((UINT*)&originalWidth, (UINT*)&originalHeight));
      if (destination_width == 0)
      {
        FLOAT scalar = static_cast<FLOAT>(destination_height) / static_cast<FLOAT>(originalHeight);
        destination_width = static_cast<uint32_t>(scalar * static_cast<FLOAT>(originalWidth));
      }
      else if (destination_height == 0)
      {
        FLOAT scalar = static_cast<FLOAT>(destination_width) / static_cast<FLOAT>(originalWidth);
        destination_height = static_cast<uint32_t>(scalar * static_cast<FLOAT>(originalHeight));
      }

      THROW_IF_ERR(wic_factory->CreateBitmapScaler(&scaler));
      THROW_IF_ERR(scaler->Initialize(
        decoder_source.Get(),
        destination_width,
        destination_height,
        WICBitmapInterpolationModeCubic
        ));
      THROW_IF_ERR(converter->Initialize(
        scaler.Get(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        NULL,
        0.f,
        WICBitmapPaletteTypeMedianCut
        ));
    }
    else // Don't scale the image.
    {
      THROW_IF_ERR(converter->Initialize(
        decoder_source.Get(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        NULL,
        0.f,
        WICBitmapPaletteTypeMedianCut
        ));
    }

    // Create a Direct2D bitmap from the WIC bitmap.
    THROW_IF_ERR(render_target->CreateBitmapFromWicBitmap(
      converter.Get(),
      NULL,
      bitmap.GetAddressOf()
      ));

    return bitmap;
  }


  toplevel_window::toplevel_window(const std::wstring& menu_name,const std::wstring& name,bool fit_to_display,float width ,float height)
    : impl_(new impl(menu_name,name,fit_to_display,width,height))
  {

  };

  void * toplevel_window::raw_handle() const {return impl_->raw_handle();};
  void toplevel_window::create(){impl_->create();};
  void toplevel_window::show(){impl_->show();};
  void toplevel_window::hide(){impl_->hide();};
  bool toplevel_window::is_show(){return impl_->is_show();};
  void toplevel_window::text(std::wstring& text){impl_->text(text);};
  void toplevel_window::message_box(const std::wstring& text,const std::wstring& caption,uint32_t type )
  {
    impl_->message_box(text,caption,type);
  };
  void toplevel_window::update(){impl_->update();};
  void toplevel_window::render(){impl_->render();};
  // void toplevel_window::player_ready(){impl_->player_ready();};


  toplevel_window_ptr create_toplevel_window
    (
    const std::wstring& menu_name,
    const std::wstring& name,
    const uint32_t show_flag,
    bool fit_to_display,
    float width,
    float height
    )
  {
    // クライアント領域のサイズからウィンドウサイズを設定
    RECT    rect    = { 0, 0, width, height };
    //::AdjustWindowRectEx( &rect, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME, FALSE, WS_EX_APPWINDOW | WS_EX_TOPMOST );
    toplevel_window* p = new toplevel_window(menu_name,name,fit_to_display,rect.right - rect.left,rect.bottom - rect.top);
    p->create();
    p->show();
    p->update();
    return toplevel_window_ptr(p);
  }
  void dialogbox
    (
    const std::wstring& menu_name,
    const std::wstring& name
    )
  {
    toplevel_window_ptr p(new toplevel_window(menu_name,name,false));
    p->create();
  }




}

