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

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "sf_windows.h"
#include "CommDlg.h"
#include "icon.h"
#include "timer.h"
#include "exception.h"
#include "application.h"
#include "info_tab_dialog.h"

#define THROW_IFERR(hres) \
  if (FAILED(hres)) { throw sf::win32_error_exception(hres); }

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

namespace sf 
{
    info_tab_dialog::info_tab_dialog(base_window& parent_window,HWND tab_hwnd,int tab_id,const std::wstring& menu_name,const std::wstring& name,HINSTANCE inst,LPCTSTR temp)
      : tab_dialog_base(parent_window,tab_hwnd,tab_id,menu_name,name,inst,temp)
    {
      create_device_independent_resources();
      create();
      hide();
    }

    info_tab_dialog::~info_tab_dialog()
    {
      discard_device();
      safe_release(d2d_factory_);
      safe_release(write_factory_);
    };

    LRESULT info_tab_dialog::window_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam)
    {
      switch(message)
      {
      case WM_INITDIALOG:
        create_device();
//        application::instance()->init_player(GetDlgItem(hwnd,IDC_VIDEO),(HWND)parent_window_.raw_handle());
        break;
      case WM_CTLCOLORDLG:
      case WM_CTLCOLORSTATIC:
      case WM_CTLCOLOREDIT:
      case WM_CTLCOLORLISTBOX:
      case WM_CTLCOLORBTN:
        SetBkMode((HDC)wParam,TRANSPARENT);
        return  (LRESULT)GetStockObject( NULL_BRUSH ); ;
      case WM_ERASEBKGND:
        return FALSE;
      case WM_PAINT:
        {
          //create_device();

          { 
            paint_struct begin_paint(hwnd);
            //CloseHandle(cb);
            // 描画コードの呼び出し
            render();

          }
        }
        break;
      case WM_DESTROY:
        discard_device();
        return FALSE;
      }
      return FALSE;

    };

    void info_tab_dialog::create_device_independent_resources()
    {
      // Direct2DFactory の生成
      if(!d2d_factory_){
#if defined(DEBUG) || defined(_DEBUG)
        D2D1_FACTORY_OPTIONS options;
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION ;
        THROW_IFERR(D2D1CreateFactory(
          D2D1_FACTORY_TYPE_SINGLE_THREADED,
          options,
          d2d_factory_.GetAddressOf()
          ));
#else
        THROW_IFERR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory_));
#endif

      }

      if(!write_factory_){
        THROW_IFERR(::DWriteCreateFactory(
          DWRITE_FACTORY_TYPE_SHARED,
          __uuidof(IDWriteFactory),
          reinterpret_cast<IUnknown**>(write_factory_.GetAddressOf())
          ));
      }


      //wic_imaging_factory_.CreateInstance(CLSID_WICImagingFactory);

      //thunk_proc_ = (WNDPROC)thunk_.getCode();
      layout_rect_ = D2D1::RectF(0.0f,100.0f,400.0f,100.0f);
      // Text Formatの作成
      THROW_IFERR(write_factory_->CreateTextFormat(
        L"メイリオ",                // Font family name.
        NULL,                       // Font collection (NULL sets it to use the system font collection).
        DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        24.0f,
        L"ja-jp",
        &write_text_format_
        ));

    }

    void info_tab_dialog::create_device()
    {
      if(!render_target_)
      {

        HWND waveform_hwnd = GetDlgItem(hwnd_,IDC_WAVEFORM);
        RECT rc;
        GetClientRect(waveform_hwnd, &rc);
        //  GetClientRect(hwnd_, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
          rc.right - rc.left,
          rc.bottom - rc.top
          );

        const D2D1_PIXEL_FORMAT format =
          D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
          D2D1_ALPHA_MODE_PREMULTIPLIED);

        const D2D1_RENDER_TARGET_PROPERTIES target_prop = 
          D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,format);

        THROW_IFERR(d2d_factory_->CreateHwndRenderTarget(
          target_prop,
          D2D1::HwndRenderTargetProperties(waveform_hwnd, size,D2D1_PRESENT_OPTIONS_IMMEDIATELY),
          &render_target_
          ));
        //D2D1::HwndRenderTargetProperties(hwnd_, size,D2D1_PRESENT_OPTIONS_IMMEDIATELY),
        //&render_target_
        //));
      }
    }

    void info_tab_dialog::discard_device()
    {
    //safe_release(sampler_state_);
    //safe_release(shader_res_view_);
    //safe_release(cb_changes_every_frame_);
    //safe_release(cb_change_on_resize_);
    //safe_release(cb_never_changes_);
    //safe_release(i_buffer_);
    //safe_release(v_buffer_);
    //safe_release(p_shader_);
    //safe_release(input_layout_);
    //safe_release(v_shader_);
    // discard_swap_chain_dependent_resources();
    safe_release(render_target_);
 /*   safe_release(dxgi_swap_chain_);
    safe_release(d3d_context_);
    safe_release(d3d_device_);
    safe_release(dxgi_adapter_);
 */ 
    }

    void info_tab_dialog::render()
    {
    if (render_target_)
    {
      // Retrieve the size of the render target.
      D2D1_SIZE_F renderTargetSize = render_target_->GetSize();
      try {
        render_target_->BeginDraw();
//        render_target_->PushAxisAlignedClip(layout_rect_,D2D1_ANTIALIAS_MODE_ALIASED);
      //  render_target_->Clear(D2D1::ColorF(D2D1::ColorF::AliceBlue));
        //render_target_->FillRectangle(D2D1::RectF(0.0f,0.0f,renderTargetSize.width,renderTargetSize.height),);
        render_target_->SetTransform(D2D1::Matrix3x2F::Identity());
        ID2D1SolidColorBrushPtr brush;
        render_target_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::OrangeRed), &brush);

        std::wstring m(L"WAVE FORMを描くところ！");
        render_target_->DrawTextW(
          m.c_str(),
          m.size(),
          write_text_format_.Get(),
          layout_rect_, 
          brush.Get());
 //       render_target_->PopAxisAlignedClip();
        THROW_IFERR(render_target_->EndDraw());

      } catch(sf::win32_error_exception& err)
      {
        if(err.hresult() == D2DERR_RECREATE_TARGET)
        {
          discard_device();
          create_device();
        } else {
          throw;
        }
      } catch(...) {
        throw;
      }
    }
    }

}

