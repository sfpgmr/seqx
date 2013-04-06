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


#include "seq_tab_dialog.h"

//#pragma comment( lib, "dxguid.lib" )
//#pragma comment( lib, "d3d11.lib" )
////#pragma comment( lib, "d3dx11.lib" )
//#pragma comment( lib, "dxgi.lib" )
//#pragma comment( lib, "d3dx9.lib" )   
//#pragma comment( lib, "Shlwapi.lib" ) 

#define THROW_IFERR(hres) \
  if (FAILED(hres)) { throw sf::win32_error_exception(hres); }

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

namespace sf 
{

  //HRESULT EnableBlurBehind(HWND hwnd)
  //{
  //  HRESULT hr = S_OK;

  //  //Create and populate the BlurBehind structre
  //  DWM_BLURBEHIND bb = {0};
  //  //Enable Blur Behind and Blur Region;
  //  bb.dwFlags = DWM_BB_ENABLE;
  //  bb.fEnable = true;
  //  bb.hRgnBlur = NULL;

  //  //Enable Blur Behind
  //  hr = DwmEnableBlurBehindWindow(hwnd, &bb);
  //  if (SUCCEEDED(hr))
  //  {
  //    //do more things
  //  }
  //  return hr;
  ////}

  //// 汎用情報格納用
  //struct mode_info 
  //{
  //  mode_info(const std::wstring& n,const std::wstring& d) : name(n),description(d) {}
  //  std::wstring name;
  //  std::wstring description;
  //};

  //// ディスプレイモード
  //struct display_mode 
  //{
  //  display_mode(const std::wstring& n,const std::wstring& d) : name(n),description(d) {}
  //  std::wstring name;
  //  std::wstring description;
  //};

  //std::vector<mode_info> display_modes = 
  //  boost::assign::list_of<mode_info>
  //  (L"DXGI_FORMAT_UNKNOWN",L"フォーマットが不明")
  //  (L"DXGI_FORMAT_R32G32B32A32_TYPELESS",L"4 成分、128 ビット型なしフォーマット 1")
  //  (L"DXGI_FORMAT_R32G32B32A32_FLOAT",L"4 成分、128 ビット浮動小数点フォーマット 1")
  //  (L"DXGI_FORMAT_R32G32B32A32_UINT",L"4 成分、128 ビット符号なし整数フォーマット 1")
  //  (L"DXGI_FORMAT_R32G32B32A32_SINT",L"4 成分、128 ビット符号付き整数フォーマット 1")
  //  (L"DXGI_FORMAT_R32G32B32_TYPELESS",L"3 成分、96 ビット型なしフォーマット")
  //  (L"DXGI_FORMAT_R32G32B32_FLOAT",L"3 成分、96 ビット浮動小数点フォーマット")
  //  (L"DXGI_FORMAT_R32G32B32_UINT",L"3 成分、96 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R32G32B32_SINT",L"3 成分、96 ビット符号付き整数フォーマット")
  //  (L"DXGI_FORMAT_R16G16B16A16_TYPELESS",L"4 成分、64 ビット型なしフォーマット")
  //  (L"DXGI_FORMAT_R16G16B16A16_FLOAT",L"4 成分、64 ビット浮動小数点フォーマット")
  //  (L"DXGI_FORMAT_R16G16B16A16_UNORM",L"4 成分、64 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R16G16B16A16_UINT",L"4 成分、64 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R16G16B16A16_SNORM",L"4 成分、64 ビット符号付き整数フォーマット")
  //  (L"DXGI_FORMAT_R16G16B16A16_SINT",L"4 成分、64 ビット符号付き整数フォーマット")
  //  (L"DXGI_FORMAT_R32G32_TYPELESS",L"2 成分、64 ビット型なしフォーマット")
  //  (L"DXGI_FORMAT_R32G32_FLOAT",L"2 成分、64 ビット浮動小数点フォーマット")
  //  (L"DXGI_FORMAT_R32G32_UINT",L"2 成分、64 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R32G32_SINT",L"2 成分、64 ビット符号付き整数フォーマット")
  //  (L"DXGI_FORMAT_R32G8X24_TYPELESS",L"2 成分、64 ビット型なしフォーマット")
  //  (L"DXGI_FORMAT_D32_FLOAT_S8X24_UINT",L"32 ビット浮動小数点成分、および 2 つの符号なし整数成分です (追加の 32 ビットを含む)。")
  //  (L"DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS",L"32 ビット浮動小数点成分、および 2 つの型なし成分です (追加の 32 ビットを含む)。")
  //  (L"DXGI_FORMAT_X32_TYPELESS_G8X24_UINT",L"32 ビット型なし成分、および 2 つの符号なし整数成分です (追加の 32 ビットを含む)。")
  //  (L"DXGI_FORMAT_R10G10B10A2_TYPELESS",L"4 成分、32 ビット型なしフォーマット")
  //  (L"DXGI_FORMAT_R10G10B10A2_UNORM",L"4 成分、32 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R10G10B10A2_UINT",L"4 成分、32 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R11G11B10_FLOAT",L"3 成分、32 ビット浮動小数点フォーマット")
  //  (L"DXGI_FORMAT_R8G8B8A8_TYPELESS",L"3 成分、32 ビット型なしフォーマット")
  //  (L"DXGI_FORMAT_R8G8B8A8_UNORM",L"4 成分、32 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R8G8B8A8_UNORM_SRGB",L"4 成分、32 ビット符号なし正規化整数 sRGB フォーマット")
  //  (L"DXGI_FORMAT_R8G8B8A8_UINT",L"4 成分、32 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R8G8B8A8_SNORM",L"3 成分、32 ビット符号付き整数フォーマット")
  //  (L"DXGI_FORMAT_R8G8B8A8_SINT",L"3 成分、32 ビット符号付き整数フォーマット")
  //  (L"DXGI_FORMAT_R16G16_TYPELESS",L"2 成分、32 ビット型なしフォーマット")
  //  (L"DXGI_FORMAT_R16G16_FLOAT",L"2 成分、32 ビット浮動小数点フォーマット")
  //  (L"DXGI_FORMAT_R16G16_UNORM",L"2 成分、32 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R16G16_UINT",L"2 成分、32 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R16G16_SNORM",L"2 成分、32 ビット符号付き整数フォーマット")
  //  (L"DXGI_FORMAT_R16G16_SINT",L"2 成分、32 ビット符号付き整数フォーマット")
  //  (L"DXGI_FORMAT_R32_TYPELESS",L"1 成分、32 ビット型なしフォーマット")
  //  (L"DXGI_FORMAT_D32_FLOAT",L"1 成分、32 ビット浮動小数点フォーマット")
  //  (L"DXGI_FORMAT_R32_FLOAT",L"1 成分、32 ビット浮動小数点フォーマット")
  //  (L"DXGI_FORMAT_R32_UINT",L"1 成分、32 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R32_SINT",L"1 成分、32 ビット符号付き整数フォーマット")
  //  (L"DXGI_FORMAT_R24G8_TYPELESS",L"2 成分、32 ビット型なしフォーマット")
  //  (L"DXGI_FORMAT_D24_UNORM_S8_UINT",L"深度チャンネルに 24 ビット、ステンシル チャンネルに 8 ビットを使用する 32 ビット Z バッファー フォーマット")
  //  (L"DXGI_FORMAT_R24_UNORM_X8_TYPELESS",L"1 成分、24 ビット符号なし正規化整数と追加の型なし 8 ビットを含む、32 ビット フォーマット")
  //  (L"DXGI_FORMAT_X24_TYPELESS_G8_UINT",L"1 成分、24 ビット型なしフォーマットと追加の 8 ビット符号なし整数成分を含む、32 ビット フォーマット")
  //  (L"DXGI_FORMAT_R8G8_TYPELESS",L"2 成分、16 ビット型なしフォーマット")
  //  (L"DXGI_FORMAT_R8G8_UNORM",L"2 成分、16 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R8G8_UINT",L"2 成分、16 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R8G8_SNORM",L"2 成分、16 ビット符号付き整数フォーマット")
  //  (L"DXGI_FORMAT_R8G8_SINT",L"2 成分、16 ビット符号付き整数フォーマット")
  //  (L"DXGI_FORMAT_R16_TYPELESS",L"1 成分、16 ビット型なしフォーマット")
  //  (L"DXGI_FORMAT_R16_FLOAT",L"1 成分、16 ビット浮動小数点フォーマット")
  //  (L"DXGI_FORMAT_D16_UNORM",L"1 成分、16 ビット符号なし正規化整数フォーマット")
  //  (L"DXGI_FORMAT_R16_UNORM",L"1 成分、16 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R16_UINT",L"1 成分、16 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R16_SNORM",L"1 成分、16 ビット符号付き整数フォーマット")
  //  (L"DXGI_FORMAT_R16_SINT",L"1 成分、16 ビット符号付き整数フォーマット")
  //  (L"DXGI_FORMAT_R8_TYPELESS",L"1 成分、8 ビット型なしフォーマット")
  //  (L"DXGI_FORMAT_R8_UNORM",L"1 成分、8 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R8_UINT",L"1 成分、8 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R8_SNORM",L"1 成分、8 ビット符号付き整数フォーマット")
  //  (L"DXGI_FORMAT_R8_SINT",L"1 成分、8 ビット符号付き整数フォーマット")
  //  (L"DXGI_FORMAT_A8_UNORM",L"1 成分、8 ビット符号なし整数フォーマット")
  //  (L"DXGI_FORMAT_R1_UNORM",L"1 成分、1 ビット符号なし正規化整数フォーマット 2.")
  //  (L"DXGI_FORMAT_R9G9B9E5_SHAREDEXP",L"4 成分、32 ビット浮動小数点フォーマット 2.")
  //  (L"DXGI_FORMAT_R8G8_B8G8_UNORM",L"4 成分、32 ビット符号なし正規化整数フォーマット 3")
  //  (L"DXGI_FORMAT_G8R8_G8B8_UNORM",L"4 成分、32 ビット符号なし正規化整数フォーマット 3")
  //  (L"DXGI_FORMAT_BC1_TYPELESS",L"4 成分、型なしブロック圧縮フォーマット")
  //  (L"DXGI_FORMAT_BC1_UNORM",L"4 成分、ブロック圧縮フォーマット")
  //  (L"DXGI_FORMAT_BC1_UNORM_SRGB",L"sRGB data用の 4 成分、ブロック圧縮フォーマット")
  //  (L"DXGI_FORMAT_BC2_TYPELESS",L"4 成分、型なしブロック圧縮フォーマット")
  //  (L"DXGI_FORMAT_BC2_UNORM",L"4 成分、ブロック圧縮フォーマット")
  //  (L"DXGI_FORMAT_BC2_UNORM_SRGB",L"sRGB data用の 4 成分、ブロック圧縮フォーマット")
  //  (L"DXGI_FORMAT_BC3_TYPELESS",L"4 成分、型なしブロック圧縮フォーマット")
  //  (L"DXGI_FORMAT_BC3_UNORM",L"4 成分、ブロック圧縮フォーマット")
  //  (L"DXGI_FORMAT_BC3_UNORM_SRGB",L"sRGB data用の 4 成分、ブロック圧縮フォーマット")
  //  (L"DXGI_FORMAT_BC4_TYPELESS",L"1 成分、型なしブロック圧縮フォーマット")
  //  (L"DXGI_FORMAT_BC4_UNORM",L"1 成分、ブロック圧縮フォーマット")
  //  (L"DXGI_FORMAT_BC4_SNORM",L"1 成分、ブロック圧縮フォーマット")
  //  (L"DXGI_FORMAT_BC5_TYPELESS",L"2 成分、型なしブロック圧縮フォーマット")
  //  (L"DXGI_FORMAT_BC5_UNORM",L"2 成分、ブロック圧縮フォーマット")
  //  (L"DXGI_FORMAT_BC5_SNORM",L"2 成分、ブロック圧縮フォーマット")
  //  (L"DXGI_FORMAT_B5G6R5_UNORM",L"3 成分、16 ビット符号なし正規化整数フォーマット")
  //  (L"DXGI_FORMAT_B5G5R5A1_UNORM",L"1 ビット アルファをサポートする 4 成分、16 ビット符号なし正規化整数フォーマット")
  //  (L"DXGI_FORMAT_B8G8R8A8_UNORM",L"8 ビット アルファをサポートする 4 成分、16 ビット符号なし正規化整数フォーマット")
  //  (L"DXGI_FORMAT_B8G8R8X8_UNORM",L"4 成分、16 ビット符号なし正規化整数フォーマット")
  //  (L"DXGI_FORMAT_FORCE_UINT",L"コンパイル時に、この列挙型のサイズを 32 ビットにするために定義されています。このvalueを指定しない場合、一部のコンパイラでは列挙型を 32 ビット以外のサイズでコンパイル可能この定数が使用されることはありません。");

  //// スキャンライン情報

  //std::vector<mode_info> scanline_orders = 
  //  boost::assign::list_of<mode_info>
  //  (L"DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED",L"走査線の順序が指定されていません。")
  //  (L"DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE",L"イメージは先頭の走査線～最後の走査線から作成され、スキップされる走査線はありません。")
  //  (L"DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST",L"イメージが上部のフィールドから作成されます。")
  //  (L"DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST",L"イメージが下部のフィールドから作成されます。");

  //// スケーリングパラメータ
  //std::vector<mode_info> scalings = boost::assign::list_of<mode_info>
  //  (L"DXGI_MODE_SCALING_UNSPECIFIED",L"スケーリングが指定されていません。")
  //  (L"DXGI_MODE_SCALING_CENTERED",L"スケーリングなしを指定します。イメージはディスプレイの中央に配置されます。通常、このフラグは固定ドットピッチ ディスプレイ (LED ディスプレイなど) に使用します。")
  //  (L"DXGI_MODE_SCALING_STRETCHED",L"拡大スケーリングを指定します。");

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

  struct toplevel_window::impl : public base_win32_dialog_t
  {

    static const uint32_t SLIDER_MAX = 30000;
    static const int TAB_INFO = 0;
    static const int TAB_CONFIG = 1;
    static const int TAB_SEQ = 2;
    static const int TAB_MIDI_CONFIG = 3;


    impl(const std::wstring& menu_name,const std::wstring& name,bool fit_to_display,float width = 160,float height = 100) 
      : base_win32_dialog_t(menu_name,name,fit_to_display,width,height) 
      , timer_(*this,100)
      , icon_(IDI_ICON1)
      /*,mesh_color_(0.7f, 0.7f, 0.7f, 1.0f)*/
      ,init_(false)
      ,thumb_start_(false)
    {

      file_name_.reserve(MAX_PATH + 1);
      dir_name_.reserve(MAX_PATH + 1);
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
      SendMessage(GetDlgItem(hwnd_,IDC_SLIDER), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, SLIDER_MAX));
//      application::instance()->player_init_ok().connect(boost::bind(&toplevel_window::impl::player_init_ok,this));

      DWORD dwDlgBase = GetDialogBaseUnits();
      int cxMargin = LOWORD(dwDlgBase) / 4; 
      int cyMargin = HIWORD(dwDlgBase) / 8;

      //タブの初期設定
      TCITEM tie; 
      tie.mask = TCIF_TEXT | TCIF_IMAGE; 
      tie.iImage = -1; 
      HWND tab = GetDlgItem(hwnd_,IDC_TAB);
      // 情報タブの挿入
      tie.pszText = L"情報";
      TabCtrl_InsertItem(tab,TAB_INFO,&tie);
      tab_dialogs_.push_back(
        new info_tab_dialog(*this,tab,TAB_INFO,L"info",L"info",HINST_THISCOMPONENT,MAKEINTRESOURCEW(IDD_INFO))
        );
      tab_dialogs_[TAB_INFO].show();

      // 設定タブの挿入
      tie.pszText = L"設定";
      TabCtrl_InsertItem(tab,TAB_CONFIG,&tie);
      tab_dialogs_.push_back(
        new config_tab_dialog(*this,tab,TAB_CONFIG,L"config",L"config",HINST_THISCOMPONENT,MAKEINTRESOURCEW(IDD_CONFIG)) 
        );
      TabCtrl_SetCurSel(tab,0);
      RECT rci;
      TabCtrl_GetItemRect(tab,0,&rci);
      RECT r;
      //TabCtrl_GetItemRect(tab,0,&r);
      GetClientRect(tab,&r);
      RECT rw;
      GetWindowRect(tab,&rw);
      POINT pt = {rw.left,rw.top + rci.bottom};
      ScreenToClient(hwnd_,&pt);

      // シーケンサタブの挿入
      tie.pszText = L"シーケンサ";
      TabCtrl_InsertItem(tab,TAB_SEQ,&tie);
      tab_dialogs_.push_back(
        new seq_tab_dialog(application::instance()->sequencer(),*this,tab,TAB_SEQ,L"sequencer",L"sequencer",HINST_THISCOMPONENT,MAKEINTRESOURCEW(IDD_SEQ))
        );

      // MIDI設定タブの挿入
      tie.pszText = L"MIDI設定";
      TabCtrl_InsertItem(tab,TAB_MIDI_CONFIG,&tie);
      tab_dialogs_.push_back(
        new midi_config_tab_dialog(application::instance()->sequencer(),*this,tab,TAB_SEQ,L"MIDI設定",L"MIDI設定",HINST_THISCOMPONENT,MAKEINTRESOURCEW(IDD_MIDI_CONFIG))
        );

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

      tab_page_ = 0;
      //slider_.attach(GetDlgItem(hwnd_,IDC_SLIDER));
    }

    // プレイヤーの初期設定完了時の処理
    void player_init_ok()
    {
      // ファイルオープン完了後の処理
/*      sf::player::PlayerPtr player = application::instance()->Player();
      player->OnOpenComplete().connect([this](sf::player::Player &) -> void
      {
        player_ready();
      }
        );

      // 再生時イベントの処理
      player->OnStart().connect([this](sf::player::Player &) -> void 
      {
        play_();
      }
        );

      // Pauseイベントの処理
      player->OnPause().connect([this](sf::player::Player &) -> void 
      {
        pause_();
      }
        );

      // 停止イベントの処理
      player->OnStop().connect([this](sf::player::Player &) -> void 
      {
        stop_();
      }
        );

        */
      // ファイルオープンボタンを有効化する
      enable_control(IDC_FILE,true);
    }

    void player_ready()
    {
      // プレイボタンは有効化
      enable_control(IDC_PLAY,true);
      focus(IDC_PLAY);
      // highlight(IDC_PLAY);
      // その他のボタンは無効化
      enable_control(IDC_REPEAT_CHECK,true);
      enable_control(IDC_PAUSE,false);
      enable_control(IDC_SLIDER,true);
      enable_control(IDC_STOP,false);
      //    enable_control(hwnd_config_,IDC_EXC_MODE,true);
    }

    void player_read_file()
    {
      post_message(WM_PLAY_PLAY,0,0);
    }

    void play_()
    {
      enable_control(IDC_PLAY,false);
      enable_control(IDC_STOP,true);
      focus(IDC_STOP);
      //highlight(IDC_STOP);
      enable_control(IDC_REPEAT_CHECK,true);
      enable_control(IDC_PAUSE,true);
      ::SetWindowText(GetDlgItem(hwnd_,IDC_PAUSE),L"一時停止");
      enable_control(IDC_SLIDER,true);
      enable_control(IDC_FILE,false);

      // 設定ダイアログ
      // EnableWindow(hwnd_config_,FALSE);
      // tab_dialogs_[TAB_CONFIG].disable();
      //    enable_config_dialog(false);

    }

    void player_stop()
    {
      post_message(WM_PLAY_STOP,0,0);
    }

    void stop_()
    {
      enable_control(IDC_PLAY,true);
      focus(IDC_PLAY);
      //highlight(IDC_PLAY);
      enable_control(IDC_REPEAT_CHECK,true);
      enable_control(IDC_STOP,false);
      enable_control(IDC_PAUSE,false);
      enable_control(IDC_SLIDER,true);
      ::SendMessage(GetDlgItem(hwnd_,IDC_SLIDER), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)0);
      enable_control(IDC_FILE,true);

      // 設定ダイアログ
      // EnableWindow(hwnd_config_,TRUE);
      // tab_dialogs_[TAB_CONFIG].enable();
      //    enable_config_dialog(true);

    }


    void player_pause()
    {
      send_message(WM_PLAY_PAUSE,0,0);
    }

    void pause_()
    {
      enable_control(IDC_PLAY,false);
      enable_control(IDC_STOP,false);
      enable_control(IDC_PAUSE,true);
      enable_control(IDC_REPEAT_CHECK,true);
      focus(IDC_PAUSE);
      //highlight(IDC_PAUSE);
      ::SetWindowText(GetDlgItem(hwnd_,IDC_PAUSE),L"再開");
      enable_control(IDC_SLIDER,true);
      enable_control(IDC_FILE,false);

      // 設定ダイアログ
      //tab_dialogs_[TAB_CONFIG].disable();
      //    enable_config_dialog(false);
    }

    virtual void create(){
      create_device_independent_resources();
      //    icon_ = ::LoadIconW(HINST_THISCOMPONENT,MAKEINTRESOURCE(IDI_ICON1));
      register_class(this->name_.c_str(),CS_HREDRAW | CS_VREDRAW ,0,DLGWINDOWEXTRA,icon_.get());
      create_dialog();

      // 半透明ウィンドウを有効にする。
      //BOOL dwmEnable;
      //DwmIsCompositionEnabled (&dwmEnable); 
      //if (dwmEnable) EnableBlurBehind(*this);

    }

    void create_dialog()
    {
      hwnd_ = ::CreateDialog(HINST_THISCOMPONENT,MAKEINTRESOURCE(IDD_MAINDIALOG),::GetDesktopWindow(),thunk_proc_);
      // ::DialogBox(HINST_THISCOMPONENT,MAKEINTRESOURCE(IDD_MAINDIALOG),0,thunk_proc_);
    }

    virtual void discard_device()
    {}

    void calc_client_size()
    {
      //クライアント領域の現在の幅、高さを求める
      RECT rc;
      GetClientRect( hwnd_, &rc );
      client_width_ = rc.right - rc.left;
      client_height_ = rc.bottom - rc.top;
    }

    virtual void create_device()
    {
      calc_client_size();
      //HRESULT hr = S_OK;
      init_ = false;
      init_ = true;// 初期化完了
    }


    virtual void create_device_independent_resources()
    {

    }

    void render(){

    }


    void create_window()
    {

      // Windowを作成する
      CreateWindowEx(
        WS_EX_APPWINDOW | WS_EX_TOPMOST,
        name_.c_str(),
        title_.c_str(),
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        static_cast<uint32_t>(dpi_.scale_x(width_)),
        static_cast<uint32_t>(dpi_.scale_x(height_)),
        NULL,
        NULL,
        HINST_THISCOMPONENT,
        this
        );
    };

    void get_dxgi_information()
    {
      //    int i = 0;
      //
      //    while(1){
      //      IDXGIAdapter1Ptr adapter;
      //      HRESULT hr = dxgi_factory_->EnumAdapters1(i,&adapter);
      //      if(hr == DXGI_ERROR_NOT_FOUND)
      //      {
      //        break;
      //      }
      //      DXGI_ADAPTER_DESC1 desc;
      //      adapter->GetDesc1(&desc);
      //      //adapter->CheckInterfaceSupport();
      //      
      //      wdout << desc.Description << std::endl;
      //      wdout << desc.DedicatedVideoMemory << std::endl;
      //      IDXGIDevice1Ptr device;
      //
      //      uint32_t oi = 0;
      //
      //
      //      while(1)
      //      {
      //        IDXGIOutputPtr output;        
      //        if(adapter->EnumOutputs(oi,&output) == DXGI_ERROR_NOT_FOUND){
      //          break;
      //        }
      //        DXGI_OUTPUT_DESC output_desc;
      //        output->GetDesc(&output_desc); 
      //        UINT num = 0;
      //        DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
      //        UINT flags         = DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING;
      //
      //        output->GetDisplayModeList(format,flags,&num,0);
      //        boost::shared_array<DXGI_MODE_DESC> disp_modes(new DXGI_MODE_DESC[num]);
      //        output->GetDisplayModeList(format,flags,&num,&disp_modes[0]);
      //        //output->GetFrameStatistics
      //        for(uint32_t mode_index = 0;mode_index < num;++mode_index)
      //        {
      //          DXGI_MODE_DESC& mode(disp_modes[mode_index]);
      //          wdout << boost::wformat(L"Format: %s %s \n Width: %d Height: %d RefleshRate: %d/%d Scaling:%s %s \n Scanline: %s %s ")
      //            %  display_modes[mode.Format].name % display_modes[mode.Format].description
      //            %  mode.Width % mode.Height 
      //            %  mode.RefreshRate.Numerator % mode.RefreshRate.Denominator
      //            %  scalings[mode.Scaling].name %  scalings[mode.Scaling].description
      //            %  scanline_orders[mode.ScanlineOrdering].name
      //            %  scanline_orders[mode.ScanlineOrdering].description
      //            << std::endl;
      //        }
      ////        output->
      //        wdout << output_desc.DeviceName << std::endl; 
      //        oi++;
      //      }
      //
      //      adapter.Release();
      //      ++i;
      //    }
    }

    // コントロールにフォーカスを移動する
    // id ... control_id
    void focus(uint32_t id)
    {
      // 間違い
      // ::SetFocus(GetDlgItem(hwnd_,id));

      // 正解
      post_message(WM_NEXTDLGCTL,(WPARAM)GetDlgItem(hwnd_,id),TRUE);
    }

    void highlight(uint32_t id)
    {
      Button_SetState(GetDlgItem(hwnd_,id),TRUE);
    }

  LRESULT other_window_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam)

  {
    switch (message)
    {
    case WM_PLAY_PLAY:
      play_();
      return TRUE;
    case WM_PLAY_STOP:
      stop_();
      return TRUE;
    case WM_PLAY_PAUSE:
      pause_();
      return TRUE;
//    case WM_APP_PLAYER_EVENT:
//      on_player_event(hwnd,wParam);
//      return FALSE;
      //::SetTimer(hwnd_,(UINT_PTR)&timer_id_,1000,NULL);
    }

    return FALSE;
  };

  void on_player_event(HWND wnd,UINT_PTR wParam)
  {
//    application::instance()->Player()->HandleEvent(wParam);
  }

  LRESULT on_init_dialog(HWND default_focus_ctrl,LPARAM data)
  {
    init_control();
    create_device();
    timer_.start();
    //MARGINS mgn = {-1};//left,right,top,bottom
    //HRESULT hr = DwmExtendFrameIntoClientArea(hwnd_, &mgn);
    return TRUE;
  }

  LRESULT on_size(uint32_t flag,uint32_t width,uint32_t height)
  {
    // バックバッファなどに関係するインターフェースを解放する
    // バックバッファを解放する
    if(init_)
    {
      int height = client_height_;
      int width = client_width_;

      calc_client_size();

      int delta_height = client_height_ - height; 

      HWND hwnd_tab = GetDlgItem(hwnd_,IDC_TAB);
      RECT rect_tab;
      GetWindowRect(hwnd_tab,&rect_tab);


      int tab_width(rect_tab.right - rect_tab.left),
        tab_height(rect_tab.bottom - rect_tab.top);
      int delta_width = width - tab_width; 

      SetWindowPos(hwnd_tab,0,0,0,client_width_ - delta_width,tab_height + delta_height,SWP_NOZORDER | SWP_NOMOVE);

      for(int i = 0;i < tab_dialogs_.size();++i)
      {
        tab_dialogs_[i].resize();
      }

      //client_width_
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

  LRESULT on_paint()
  {
    //create_device();

    { 
      paint_struct begin_paint(hwnd_);
      //CloseHandle(cb);
      // 描画コードの呼び出し
      /*render();*/

    }
    return FALSE;
    //        ::ShowWindow(hwnd_,SW_MINIMIZE);

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
    switch(state)
    {
    case TB_THUMBTRACK :

#ifdef _DEBUG
      wdout << L"TB_THUMBTRACK" << std::endl;
#endif
      thumb_start_ = true;
      break;
    case TB_ENDTRACK :
#ifdef _DEBUG
      wdout << L"TB_ENDTRACK" << std::endl;
#endif
//      application::instance()->reader_position(SendMessage(GetDlgItem(hwnd_,IDC_SLIDER), TBM_GETPOS, 0, 0) * application::instance()->reader_data_size() / SLIDER_MAX); 

      thumb_start_ = false;
      break;
    case TB_PAGEUP:
      thumb_start_ = true;
      break;
    case TB_PAGEDOWN:
      thumb_start_ = true;
      break;
    default:
#ifdef _DEBUG
      wdout << boost::wformat(L"LOWORD(wParam):%d") % state << std::endl;
#endif
      break;
    }
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
    BOOL ret(::DestroyWindow(hwnd_));
    BOOST_ASSERT(ret != 0);
    return TRUE;
  }

  LRESULT on_command(uint32_t wparam, uint32_t lparam)
  {
    switch(LOWORD(wparam)){
    case IDC_FILE:
      {
        BOOL ret =  get_file_path();
        if(ret){
//          sf::player::ev::OpenURL url(file_name_);
//          application::instance()->Player()->process_event(url);
          //player_ready();
        }
      }
       return TRUE;
    case IDC_SLIDER:
      //::MessageBox( hwnd_,L"SLIDERが動作しました。",L"IDC_SLIDER",MB_OK);
      return TRUE;
    case IDC_PLAY:
      //         ::MessageBox( hwnd_,L"player_read_fileボタンが押されました。",L"IDC_PLAY",MB_OK);
//      application::instance()->Player()->process_event(player::ev::Play());
      return TRUE;
    case IDC_STOP:
      //::MessageBox( hwnd_,L"player_stopボタンが押されました。",L"IDC_STOP",MB_OK);
  //    application::instance()->Player()->process_event(player::ev::Stop());
      return TRUE;
    case IDC_PAUSE:
      //::MessageBox( hwnd_,L"player_pauseボタンが押されました。",L"IDC_PAUSE",MB_OK);
  //    application::instance()->Player()->process_event(player::ev::Pause());
      return TRUE;
    case IDC_REPEAT_CHECK:
  //    application::instance()->reader_repeat_mode(::SendMessage(GetDlgItem(hwnd_,IDC_REPEAT_CHECK),BM_GETCHECK,0,0) == BST_CHECKED);
      return TRUE;
    }

    //if(HIWORD(wParam) == THBN_CLICKED ){
    //  switch(LOWORD(wParam)){
    //  case THUMB_START:
    //    {
    //      if(status_ != player_stop)
    //      {
    //        update_status(player_stop);
    //      } else {
    //        update_status(active);
    //      }
    //    }
    //    break;
    //  }
    //}
    return FALSE;
  }

  LRESULT on_timer(uint32_t timer_id)
  {
    // TODO:スレッドのエラーチェックも入れておく
    if(!thumb_start_){
      //int status = application::instance()->reader_status(std::memory_order_relaxed);
      //if( status == reader_agent_t::status_play_ok || status == reader_agent_t::status_end_ok)
      //{
      //  uint64_t pos = application::instance()->reader_position() * SLIDER_MAX / application::instance()->reader_data_size(); 
      //  SendMessage(GetDlgItem(hwnd_,IDC_SLIDER), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)pos);
      //}
    }
    return TRUE;

  }

  LRESULT on_notify(NMHDR* nmhdr)
  {
    switch (nmhdr->code)
    {
    case TCN_SELCHANGE:
      {
        tab_page_ = TabCtrl_GetCurSel(nmhdr->hwndFrom);
        for(int i = 0;i < tab_dialogs_.size();++ i)
        {
          if( i == tab_page_)
          {
            tab_dialogs_[i].show();
          } else {
            tab_dialogs_[i].hide();
          }
        }
        return TRUE;
        break;
      }
    }
    return FALSE;
  }

  private:

    void read_start() {};
    BOOL get_file_path ()
    {
      WCHAR file_name[MAX_PATH]  = L"";  // ファイル名
      WCHAR dir_name[MAX_PATH]   = L"";  // ディレクトリ

      WCHAR file_title[MAX_PATH] = L"";  // ダイアログのタイトル
      OPENFILENAME ofn;              

      if (dir_name_.empty())
      {
        GetWindowsDirectory(dir_name, MAX_PATH);
        dir_name_.assign(dir_name);
      }

      if(!file_name_.empty())
      {
        wcscpy_s(file_name,MAX_PATH,dir_name_.c_str());
      }

      // Set up structure for file dialog
      ZeroMemory(&ofn, sizeof(ofn));
      ofn.lStructSize = sizeof(OPENFILENAME);
      ofn.hwndOwner = ::GetDesktopWindow();
//      ofn.lpstrFilter = L"Media File (*.*)\0*.*\0";
      ofn.lpstrFilter = L"Standard MIDI File (*.*)\0*.*\0";
      ofn.lpstrCustomFilter = NULL;
      ofn.nFilterIndex = 1;
      ofn.lpstrFile = file_name;
      ofn.nMaxFile = MAX_PATH;
      ofn.lpstrFileTitle = file_title;
      ofn.nMaxFileTitle = MAX_PATH;
      ofn.lpstrInitialDir = dir_name;
//      ofn.lpstrTitle = L"Mediaファイルを指定してください。";
      ofn.lpstrTitle = L"Standard MIDIファイルを指定してください。";
//      ofn.lpstrDefExt = L"mp4";
      ofn.lpstrDefExt = L"mid";
      ofn.Flags = OFN_SHAREAWARE | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

      // Put up Open File dialog to get file name.
      if (GetOpenFileName(&ofn))
      {
        file_name_.assign(file_name);
//        sf::application::instance()->reader_setup(file_name);
      }
      return TRUE;
    }

    //long wm_task_bar_create_;
    //sf::taskbar taskbar_;

    timer timer_;
    bool thumb_start_;
    //slider slider_;

    //IDXGIFactory1Ptr dxgi_factory_;
    //IDXGIAdapter1Ptr adapter_;
    //IDXGIOutputPtr output_;
    //ID3D11DevicePtr d3d_device_;
    //ID3D11DeviceContextPtr d3d_context_;
    //ID3D11Texture2DPtr texture_;
    //ID3D11RenderTargetViewPtr view_;
    //ID3D11Texture2DPtr depth_texture_;
    //ID3D11DepthStencilViewPtr depth_view_;
    //ID3D11VertexShaderPtr v_shader_;
    //ID3D11InputLayoutPtr input_layout_;
    //ID3D11PixelShaderPtr p_shader_;
    //ID3D11BufferPtr v_buffer_;
    //ID3D11BufferPtr i_buffer_;
    //ID3D11BufferPtr cb_never_changes_;
    //ID3D11BufferPtr cb_change_on_resize_;
    //ID3D11BufferPtr cb_changes_every_frame_;
    //ID3D11ShaderResourceViewPtr shader_res_view_;
    //ID3D11SamplerStatePtr sampler_state_;

    //DirectX::XMMATRIX                            mat_world_;
    //DirectX::XMMATRIX                            mat_view_;
    //DirectX::XMMATRIX                            mat_projection_;
    //DirectX::XMFLOAT4                            mesh_color_;

    float client_width_,client_height_;

    //IDXGISwapChainPtr; 
    //IDXGISwapChainPtr swap_chain_;
    // std::wstring dxgi_info_;

    std::wstring file_name_;
    std::wstring dir_name_;
    icon icon_;
    bool init_;

    D2D1_SIZE_U icon_size_;

    //  HWND hwnd_config_;
    // HWND hwnd_info_;

    boost::ptr_vector<tab_dialog_base> tab_dialogs_;
    //tab_dialog_ptr config_dialog_;

    int tab_page_;


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

    THROW_IFERR(wic_factory->CreateDecoderFromFilename(
      uri.c_str(),
      NULL,
      GENERIC_READ,
      WICDecodeMetadataCacheOnLoad,
      &decoder
      ));

    // Create the initial frame.
    THROW_IFERR(decoder->GetFrame(0, &decoder_source));

    // Convert the image format to 32bppPBGRA
    // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
    THROW_IFERR(hr = wic_factory->CreateFormatConverter(&converter));

    // If a new width or height was specified, create an
    // IWICBitmapScaler and use it to resize the image.
    if (destination_width != 0 || destination_height != 0)
    {
      uint32_t originalWidth, originalHeight;
      THROW_IFERR(decoder_source->GetSize((UINT*)&originalWidth, (UINT*)&originalHeight));
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

      THROW_IFERR(wic_factory->CreateBitmapScaler(&scaler));
      THROW_IFERR(scaler->Initialize(
        decoder_source.Get(),
        destination_width,
        destination_height,
        WICBitmapInterpolationModeCubic
        ));
      THROW_IFERR(converter->Initialize(
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
      THROW_IFERR(converter->Initialize(
        decoder_source.Get(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        NULL,
        0.f,
        WICBitmapPaletteTypeMedianCut
        ));
    }

    // Create a Direct2D bitmap from the WIC bitmap.
    THROW_IFERR(render_target->CreateBitmapFromWicBitmap(
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
  void toplevel_window::player_read_file(){impl_->player_read_file();};
  void toplevel_window::player_pause(){impl_->player_pause();};
  void toplevel_window::player_stop(){impl_->player_stop();};

  void toplevel_window::enable_control(uint32_t id,bool enable)
  {
    impl_->enable_control(id,enable);
  };


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
    ::AdjustWindowRectEx( &rect, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME, FALSE, 0 );
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

