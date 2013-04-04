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
#include "config_tab_dialog.h"

#define THROW_IFERR(hres) \
  if (FAILED(hres)) { throw sf::win32_error_exception(hres); }

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

namespace sf 
{
    config_tab_dialog::config_tab_dialog(sf::base_window& parent_window,HWND tab_hwnd,int tab_id,const std::wstring& menu_name,const std::wstring& name,HINSTANCE inst,LPCTSTR temp)
      : tab_dialog_base(parent_window,tab_hwnd,tab_id,menu_name,name,inst,temp),current_output_driver_index_(-1),current_input_driver_index_(-1)
      ,update_config_dialog_(false)
      ,edit_output_config_(false)
      ,edit_input_config_(false)
    {
      create();
      hide();
    }

    LRESULT config_tab_dialog::window_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam)
    {
      switch(message)
      {
      case WM_INITDIALOG:
        init_config_dialog();
        return TRUE;
      case WM_COMMAND:
        {
          switch(LOWORD(wParam)){
            // ---------------------------------
            // 出力
            // ---------------------------------
          case IDC_ENDPOINT_DEVICE:
            // エンドポイントデバイス
            if(HIWORD(wParam) == CBN_SELCHANGE && !update_config_dialog_)
            {
              edit_output_config_ = true;
              current_output_driver_index_ = SendMessage(GetDlgItem(hwnd,IDC_ENDPOINT_DEVICE),CB_GETITEMDATA,SendMessage(GetDlgItem(hwnd,IDC_ENDPOINT_DEVICE),CB_GETCURSEL,0,0),0);
              update_config_dialog();
            }
            return TRUE;
          case IDC_SAMPLE_BIT:
            // サンプルビット数
            if(HIWORD(wParam) == CBN_SELCHANGE && !update_config_dialog_)
            {
              edit_output_config_ = true;
              editing_output_params_.bits =  SendMessage(GetDlgItem(hwnd,IDC_SAMPLE_BIT),CB_GETITEMDATA,SendMessage(GetDlgItem(hwnd,IDC_SAMPLE_BIT),CB_GETCURSEL,0,0),0);
              update_config_dialog();
            }
            return TRUE;
          case IDC_VALID_SAMPLE_BITS:
            // 実際のサンプルビット数
            if(HIWORD(wParam) == CBN_SELCHANGE &&  !update_config_dialog_)
            {
              edit_output_config_ = true;
              editing_output_params_.valid_bits =  SendMessage(GetDlgItem(hwnd,IDC_VALID_SAMPLE_BITS),CB_GETITEMDATA,SendMessage(GetDlgItem(hwnd,IDC_VALID_SAMPLE_BITS),CB_GETCURSEL,0,0),0);
              update_config_dialog();
            }
            return TRUE;
          case IDC_SAMPLE_RATE:
            // サンプル周波数
            if(HIWORD(wParam) == CBN_SELCHANGE &&  !update_config_dialog_)
            {
              edit_output_config_ = true;
              editing_output_params_.sample_rate =  SendMessage(GetDlgItem(hwnd,IDC_SAMPLE_RATE),CB_GETITEMDATA,SendMessage(GetDlgItem(hwnd,IDC_SAMPLE_RATE),CB_GETCURSEL,0,0),0);
              update_config_dialog();
            }
            return TRUE;
          case IDC_EXC_MODE:
            // 排他モード
            {
              if(!update_config_dialog_){
                edit_output_config_ = true;

                //application::instance()->exclusive_mode(::SendMessage(GetDlgItem(hwnd,IDC_EXC_MODE),BM_GETCHECK,0,0) == BST_CHECKED);
                editing_output_params_.exclusive_mode = ::SendMessage(GetDlgItem(hwnd,IDC_EXC_MODE),BM_GETCHECK,0,0) == BST_CHECKED;
                update_config_dialog();
              }
            }
            return TRUE;
          case IDC_EVENT_MODE:
            // イベントモード
            {
              if(!update_config_dialog_){
                //application::instance()->exclusive_mode(::SendMessage(GetDlgItem(hwnd,IDC_EXC_MODE),BM_GETCHECK,0,0) == BST_CHECKED);
                editing_output_params_.event_mode = ::SendMessage(GetDlgItem(hwnd,IDC_EVENT_MODE),BM_GETCHECK,0,0) == BST_CHECKED;
                edit_output_config_ = true;
                update_config_dialog();
              }
            }
            return TRUE;

          case IDC_LATENCY:
            // レイテンシ
            if(HIWORD(wParam) == EN_UPDATE  &&  !update_config_dialog_)
            {
              edit_output_config_ = true;
              enable_control(hwnd_,IDC_APPLY,true);
              enable_control(hwnd_,IDC_CANCEL,true);
              //BOOL success = FALSE;
              //int res = 0;
              //res = GetDlgItemInt(hwnd_,IDC_LATENCY,&success,TRUE);
              //if(success)
              //{
              //  editing_output_params_.latency = res;
              //} else {
              //  message_box(L"不正な値が設定されています。",L"エラー",MB_OK);
              //}
            }
            return TRUE;
            // ---------------------------------
            // 入力
            // ---------------------------------
          case IDC_INPUT_ENDPOINT_DEVICE:
            if(HIWORD(wParam) == CBN_SELCHANGE && !update_config_dialog_)
            {
              edit_input_config_ = true;
              current_input_driver_index_ 
                = SendMessage(
                GetDlgItem(hwnd,IDC_INPUT_ENDPOINT_DEVICE),
                CB_GETITEMDATA,
                SendMessage(GetDlgItem(hwnd,IDC_INPUT_ENDPOINT_DEVICE),CB_GETCURSEL,0,0),
                0);
              update_config_dialog();
            }
            return TRUE;
          case IDC_INPUT_SAMPLE_BIT:
            if(HIWORD(wParam) == CBN_SELCHANGE && !update_config_dialog_)
            {
              edit_input_config_ = true;
              editing_input_params_.bits 
                =  SendMessage(
                GetDlgItem(hwnd,IDC_INPUT_SAMPLE_BIT),
                CB_GETITEMDATA,
                SendMessage(GetDlgItem(hwnd,IDC_INPUT_SAMPLE_BIT),CB_GETCURSEL,0,0)
                ,0);
              update_config_dialog();
            }
            return TRUE;
          case IDC_INPUT_VALID_SAMPLE_BITS:
            if(HIWORD(wParam) == CBN_SELCHANGE && !update_config_dialog_)
            {
              edit_input_config_ = true;
              editing_input_params_.valid_bits 
                =  SendMessage(
                GetDlgItem(hwnd,IDC_INPUT_VALID_SAMPLE_BITS),
                CB_GETITEMDATA,
                SendMessage(GetDlgItem(hwnd,IDC_INPUT_VALID_SAMPLE_BITS),CB_GETCURSEL,0,0),
                0);
              update_config_dialog();
            }
            return TRUE;
          case IDC_INPUT_SAMPLE_RATE:
            if(HIWORD(wParam) == CBN_SELCHANGE &&  !update_config_dialog_)
            {
              edit_input_config_ = true;
              editing_input_params_.sample_rate
                =  SendMessage(
                GetDlgItem(hwnd,IDC_INPUT_SAMPLE_RATE),
                CB_GETITEMDATA,
                SendMessage(GetDlgItem(hwnd,IDC_INPUT_SAMPLE_RATE),CB_GETCURSEL,0,0),
                0);
              update_config_dialog();
            }
            return TRUE;
          case IDC_INPUT_EXC_MODE:
            {
              if(!update_config_dialog_){
                edit_input_config_ = true;

                //application::instance()->exclusive_mode(::SendMessage(GetDlgItem(hwnd,IDC_INPUT_EXC_MODE),BM_GETCHECK,0,0) == BST_CHECKED);
                editing_input_params_.exclusive_mode 
                  = ::SendMessage(GetDlgItem(hwnd,IDC_INPUT_EXC_MODE),BM_GETCHECK,0,0) 
                  == BST_CHECKED;
                update_config_dialog();
              }
            }
            return TRUE;
          case IDC_INPUT_EVENT_MODE:
            {
              if(!update_config_dialog_){
                //application::instance()->exclusive_mode(::SendMessage(GetDlgItem(hwnd,IDC_INPUT_EXC_MODE),BM_GETCHECK,0,0) == BST_CHECKED);
                editing_input_params_.event_mode 
                  = ::SendMessage(GetDlgItem(hwnd,IDC_INPUT_EVENT_MODE),BM_GETCHECK,0,0) 
                  == BST_CHECKED;
                edit_input_config_ = true;
                update_config_dialog();
              }
            }
            return TRUE;
          case IDC_INPUT_LATENCY:
            if(HIWORD(wParam) == EN_UPDATE  &&  !update_config_dialog_)
            {
              edit_input_config_ = true;
              enable_control(hwnd_,IDC_APPLY,true);
              enable_control(hwnd_,IDC_CANCEL,true);
              //BOOL success = FALSE;
              //int res = 0;
              //res = GetDlgItemInt(hwnd_,IDC_INPUT_LATENCY,&success,TRUE);
              //if(success)
              //{
              //  editing_input_params_.latency = res;
              //} else {
              //  message_box(L"不正な値が設定されています。",L"エラー",MB_OK);
              //}
            }
            return TRUE;
            // ---------------------------------
            // 設定反映
            // ---------------------------------
          case IDC_APPLY:
            {
              BOOL success = FALSE;
              REFERENCE_TIME res(0);

              // 出力デバイス設定

              if(edit_output_config_){
                res = GetDlgItemInt(hwnd_,IDC_LATENCY,&success,TRUE);
                if(success)
                {
                  editing_output_params_.latency = res * 10000;
                } else {
                  message_box(L"レイテンシに不正な値が設定されています。",L"エラー",MB_OK);
                }
                try {
                  application::instance()->apply_output_device_config
                    (current_output_driver_index_,editing_output_params_);
                  edit_output_config_ = false;
                } catch (...) 
                {
                  application::instance()->apply_output_device_config
                    (wasapi_device_manager::instance()->current_output_device_index(),
                    wasapi_device_manager::instance()->current_output_device().params);
                  edit_output_config_ = true;
                }
              }

              // 入力デバイス設定

              if(edit_input_config_){
                res = GetDlgItemInt(hwnd_,IDC_INPUT_LATENCY,&success,TRUE);
                if(success)
                {
                  editing_input_params_.latency = res * 10000;
                } else {
                  message_box(L"レイテンシに不正な値が設定されています。",L"エラー",MB_OK);
                }
                try {
                  application::instance()->apply_input_device_config
                    (current_input_driver_index_,editing_input_params_);
                  edit_input_config_ = false;
                } catch (...) 
                {
                  application::instance()->apply_input_device_config
                    (wasapi_device_manager::instance()->current_input_device_index(),
                    wasapi_device_manager::instance()->current_input_device().params);
                  edit_input_config_ = true;
                }
              }
              update_config_dialog();
            }
            return TRUE;
            // ---------------------------------
            // 設定キャンセル
            // ---------------------------------
          case IDC_CANCEL:
            {
              wasapi_device_manager::ptr m(sf::wasapi_device_manager::instance());

              if(edit_output_config_){
                editing_output_params_ = m->current_output_device().params;
                edit_output_config_ = false;
              }

              if(edit_input_config_){
                editing_input_params_ = m->current_input_device().params;
                edit_input_config_ = false;
              }

              update_config_dialog();
              return TRUE;
            }
          }
          return FALSE;
        }
      case WM_CTLCOLORDLG:
      case WM_CTLCOLORSTATIC:
      case WM_CTLCOLORBTN:
      case WM_CTLCOLOR:
        SetBkMode((HDC)wParam,TRANSPARENT);
        return  (LRESULT)GetStockObject( /* NULL_BRUSH */WHITE_BRUSH );
      case WM_ERASEBKGND:
        return FALSE;
      }
      return FALSE;  
    };

  void config_tab_dialog::init_config_dialog()
  {

    wasapi_device_manager::ptr m(sf::wasapi_device_manager::instance());

    // バックアップ
    {
      // 出力
      editing_output_params_ = m->current_output_device().params;
      wasapi_device_manager::device_infos_t& i(m->output_device_infos());

      for(int c = 0;c < i.size();++c)
      {
        if(i[c].is_selected_){
          current_output_driver_index_ = c;
        }
      }
    }

    {
      // 入力
      editing_input_params_ = m->current_input_device().params;
      wasapi_device_manager::device_infos_t& i(m->input_device_infos());
      for(int c = 0;c < i.size();++c)
      {
        if(i[c].is_selected_){
          current_input_driver_index_ = c;
        }
      }
//	  if(!i.empty() || current_input_driver_index_ == -1)
//	  {
//	  }
    }
    edit_output_config_ = false;
    edit_input_config_ = false;
    update_config_dialog();
    //i[current_index].support_formats

  }

 void config_tab_dialog::update_config_dialog()
  {
    update_config_dialog_ = true;
    application::ptr app(application::instance());

    // ----------------------------------------
    enable_control(hwnd_,IDC_APPLY,edit_output_config_ || edit_input_config_);
    enable_control(hwnd_,IDC_CANCEL,edit_output_config_ || edit_input_config_);


    // デバイスコンボボックスのセットアップ
    wasapi_device_manager::ptr m(sf::wasapi_device_manager::instance());

    // ==========================================
    // 出力設定
    // ==========================================
    { 
      wasapi_device_manager::device_infos_t& i(m->output_device_infos());

      HWND hwnd_endp = GetDlgItem(hwnd_,IDC_ENDPOINT_DEVICE);
      SendMessage(hwnd_endp,CB_RESETCONTENT,0,0);

      for(int c = 0,e = i.size(); c < e;++c)
      {
        SendMessage(hwnd_endp,CB_ADDSTRING,0,(LPARAM)i[c].display_name_.c_str());
        SendMessage(hwnd_endp,CB_SETITEMDATA,c,c);
        if(c == current_output_driver_index_)
        {
          SendMessage(hwnd_endp,CB_SETCURSEL,c,0);
        }
      }

      typedef wasapi_device_manager::format_map_type fmts_t;
      fmts_t& fmts = i[current_output_driver_index_].support_formats_;
      //// 
      HWND hwnd_smp_bits = GetDlgItem(hwnd_,IDC_SAMPLE_BIT);
    
      SendMessage(hwnd_smp_bits,CB_RESETCONTENT,0,0);
      uint32_t share_mode = AUDCLNT_SHAREMODE_SHARED;
      if(editing_output_params_.exclusive_mode)
      {
        share_mode = AUDCLNT_SHAREMODE_EXCLUSIVE;
        ::SendDlgItemMessage(hwnd_,IDC_EXC_MODE,BM_SETCHECK,BST_CHECKED ,0);
      } else {
        ::SendDlgItemMessage(hwnd_,IDC_EXC_MODE,BM_SETCHECK,BST_UNCHECKED ,0);
      }

      if(editing_output_params_.event_mode)
      {
        ::SendDlgItemMessage(hwnd_,IDC_EVENT_MODE,BM_SETCHECK,BST_CHECKED ,0);
      } else {
        ::SendDlgItemMessage(hwnd_,IDC_EVENT_MODE,BM_SETCHECK,BST_UNCHECKED ,0);
      }

      typedef fmts_t::mapped_type fsmpbits_t;
      fsmpbits_t& fsmpbits = fmts[share_mode];
      {
        int c = 0;
        std::for_each(fsmpbits.begin(),fsmpbits.end(),
          [&](fsmpbits_t::value_type & pair)
          {
            SendMessage(hwnd_smp_bits,CB_ADDSTRING,0,(LPARAM)(boost::lexical_cast<std::wstring>(pair.first).c_str()));
            SendMessage(hwnd_smp_bits,CB_SETITEMDATA,c,pair.first);
            if(editing_output_params_.bits == pair.first)
            {
              SendMessage(hwnd_smp_bits,CB_SETCURSEL,c,0);
            }
            ++c;
          }
        );
      }

      // ----------------------------------------
      // ----------------------------------------
    
      typedef wasapi_device_manager::format_map_type::mapped_type::mapped_type fmts_vbits_t;

      fmts_vbits_t& fmts_vbits = fmts[share_mode][editing_output_params_.bits];
      int cnt_vbits = 0;
      HWND hwnd_valid_bits = GetDlgItem(hwnd_,IDC_VALID_SAMPLE_BITS);
      SendMessage(hwnd_valid_bits,CB_RESETCONTENT,0,0);
      std::for_each(fmts_vbits.begin(),fmts_vbits.end(),
        [&](fmts_vbits_t::value_type& pair_vbits)
      {
        SendMessage(hwnd_valid_bits,CB_ADDSTRING,0,(LPARAM)(boost::lexical_cast<std::wstring>(pair_vbits.first).c_str()));
        SendMessage(hwnd_valid_bits,CB_SETITEMDATA,cnt_vbits,pair_vbits.first);
        if(pair_vbits.first == editing_output_params_.valid_bits)
        {
          SendMessage(hwnd_valid_bits,CB_SETCURSEL,cnt_vbits,0);
        }
        cnt_vbits++;
      }
      );

      // ----------------------------------------
      // ----------------------------------------
      typedef wasapi_device_manager::format_map_type::mapped_type::mapped_type::mapped_type fmts_channels_t;
      fmts_channels_t & 
        fmts_channels = fmts[share_mode][editing_output_params_.bits][editing_output_params_.valid_bits];
      //typedef decltype(fmts_channels) fmts_channels_t;
      int cnt_channels = 0;
      HWND hwnd_channels = GetDlgItem(hwnd_,IDC_CHANNEL);
      SendMessage(hwnd_channels,CB_RESETCONTENT,0,0);
      std::for_each(fmts_channels.begin(),fmts_channels.end(),
        [&](fmts_channels_t::value_type & pair_channel)
      {
        SendMessage(hwnd_channels,CB_ADDSTRING,0,(LPARAM)(boost::lexical_cast<std::wstring>(pair_channel.first).c_str()));
        SendMessage(hwnd_channels,CB_SETITEMDATA,cnt_channels,pair_channel.first);
        if(pair_channel.first == editing_output_params_.channel)
        {
          SendMessage(hwnd_channels,CB_SETCURSEL,cnt_channels,0);
        }
        cnt_channels++;
      }
      );

      // ----------------------------------------
      // ----------------------------------------

      typedef wasapi_device_manager::format_map_type::mapped_type::mapped_type::mapped_type::mapped_type fmts_sample_rates_t;
      fmts_sample_rates_t& fmts_sample_rates 
        = fmts[share_mode][editing_output_params_.bits][editing_output_params_.valid_bits][editing_output_params_.channel];
      int cnt_samples = 0;
      HWND hwnd_sample = GetDlgItem(hwnd_,IDC_SAMPLE_RATE);
      SendMessage(hwnd_sample,CB_RESETCONTENT,0,0);
      std::for_each(fmts_sample_rates.begin(),fmts_sample_rates.end(),
        [&](fmts_sample_rates_t::value_type & pair_sample)
      {
        SendMessage(hwnd_sample,CB_ADDSTRING,0,(LPARAM)(boost::lexical_cast<std::wstring>(pair_sample.first).c_str()));
        SendMessage(hwnd_sample,CB_SETITEMDATA,cnt_samples,pair_sample.first);
        if(pair_sample.first == editing_output_params_.sample_rate)
        {
          SendMessage(hwnd_sample,CB_SETCURSEL,cnt_samples,0);
        }
        cnt_samples++;
      }
      );


      // ----------------------------------------
      // ----------------------------------------
      SetDlgItemInt(hwnd_,IDC_LATENCY,editing_output_params_.latency/10000,TRUE);
      SetDlgItemText(hwnd_,IDC_MIN_MAX_LATENCY,(boost::wformat(L"(%dms ～ %dms)") 
        % (i[current_output_driver_index_].latency_minimum_ / 10000) 
        % (i[current_output_driver_index_].latency_default_ / 10000)).str().c_str());
    } // 出力ダイアログ

    // ===================================================================
    // 入力設定
    // ===================================================================
    { 
      wasapi_device_manager::device_infos_t& i(m->input_device_infos());

      HWND hwnd_endp = GetDlgItem(hwnd_,IDC_INPUT_ENDPOINT_DEVICE);
      SendMessage(hwnd_endp,CB_RESETCONTENT,0,0);

      for(int c = 0,e = i.size(); c < e;++c)
      {
        SendMessage(hwnd_endp,CB_ADDSTRING,0,(LPARAM)i[c].display_name_.c_str());
        SendMessage(hwnd_endp,CB_SETITEMDATA,c,c);
        if(c == current_input_driver_index_)
        {
          SendMessage(hwnd_endp,CB_SETCURSEL,c,0);
        }
      }

      typedef wasapi_device_manager::format_map_type fmts_t;
      fmts_t& fmts = i[current_input_driver_index_].support_formats_;
      //// 
      HWND hwnd_smp_bits = GetDlgItem(hwnd_,IDC_INPUT_SAMPLE_BIT);
    
      SendMessage(hwnd_smp_bits,CB_RESETCONTENT,0,0);
      uint32_t share_mode = AUDCLNT_SHAREMODE_SHARED;
      if(editing_input_params_.exclusive_mode)
      {
        share_mode = AUDCLNT_SHAREMODE_EXCLUSIVE;
        ::SendDlgItemMessage(hwnd_,IDC_INPUT_EXC_MODE,BM_SETCHECK,BST_CHECKED ,0);
      } else {
        ::SendDlgItemMessage(hwnd_,IDC_INPUT_EXC_MODE,BM_SETCHECK,BST_UNCHECKED ,0);
      }

      if(editing_input_params_.event_mode)
      {
        ::SendDlgItemMessage(hwnd_,IDC_INPUT_EVENT_MODE,BM_SETCHECK,BST_CHECKED ,0);
      } else {
        ::SendDlgItemMessage(hwnd_,IDC_INPUT_EVENT_MODE,BM_SETCHECK,BST_UNCHECKED ,0);
      }

      typedef fmts_t::mapped_type fsmpbits_t;
      fsmpbits_t& fsmpbits = fmts[share_mode];
      {
        int c = 0;
        std::for_each(fsmpbits.begin(),fsmpbits.end(),
          [&](fsmpbits_t::value_type & pair)
          {
            SendMessage(hwnd_smp_bits,CB_ADDSTRING,0,(LPARAM)(boost::lexical_cast<std::wstring>(pair.first).c_str()));
            SendMessage(hwnd_smp_bits,CB_SETITEMDATA,c,pair.first);
            if(editing_input_params_.bits == pair.first)
            {
              SendMessage(hwnd_smp_bits,CB_SETCURSEL,c,0);
            }
            ++c;
          }
        );
      }

      // ----------------------------------------
      // ----------------------------------------
    
      typedef wasapi_device_manager::format_map_type::mapped_type::mapped_type fmts_vbits_t;

      fmts_vbits_t& fmts_vbits = fmts[share_mode][editing_input_params_.bits];
      int cnt_vbits = 0;
      HWND hwnd_valid_bits = GetDlgItem(hwnd_,IDC_INPUT_VALID_SAMPLE_BITS);
      SendMessage(hwnd_valid_bits,CB_RESETCONTENT,0,0);
      std::for_each(fmts_vbits.begin(),fmts_vbits.end(),
        [&](fmts_vbits_t::value_type& pair_vbits)
      {
        SendMessage(hwnd_valid_bits,CB_ADDSTRING,0,(LPARAM)(boost::lexical_cast<std::wstring>(pair_vbits.first).c_str()));
        SendMessage(hwnd_valid_bits,CB_SETITEMDATA,cnt_vbits,pair_vbits.first);
        if(pair_vbits.first == editing_input_params_.valid_bits)
        {
          SendMessage(hwnd_valid_bits,CB_SETCURSEL,cnt_vbits,0);
        }
        cnt_vbits++;
      }
      );

      // ----------------------------------------
      // ----------------------------------------
      typedef wasapi_device_manager::format_map_type::mapped_type::mapped_type::mapped_type fmts_channels_t;
      fmts_channels_t & 
        fmts_channels = fmts[share_mode][editing_input_params_.bits][editing_input_params_.valid_bits];
      //typedef decltype(fmts_channels) fmts_channels_t;
      int cnt_channels = 0;
      HWND hwnd_channels = GetDlgItem(hwnd_,IDC_INPUT_CHANNEL);
      SendMessage(hwnd_channels,CB_RESETCONTENT,0,0);
      std::for_each(fmts_channels.begin(),fmts_channels.end(),
        [&](fmts_channels_t::value_type & pair_channel)
      {
        SendMessage(hwnd_channels,CB_ADDSTRING,0,(LPARAM)(boost::lexical_cast<std::wstring>(pair_channel.first).c_str()));
        SendMessage(hwnd_channels,CB_SETITEMDATA,cnt_channels,pair_channel.first);
        if(pair_channel.first == editing_input_params_.channel)
        {
          SendMessage(hwnd_channels,CB_SETCURSEL,cnt_channels,0);
        }
        cnt_channels++;
      }
      );

      // ----------------------------------------
      // ----------------------------------------

      typedef wasapi_device_manager::format_map_type::mapped_type::mapped_type::mapped_type::mapped_type fmts_sample_rates_t;
      fmts_sample_rates_t& fmts_sample_rates 
        = fmts[share_mode][editing_input_params_.bits][editing_input_params_.valid_bits][editing_input_params_.channel];
      int cnt_samples = 0;
      HWND hwnd_sample = GetDlgItem(hwnd_,IDC_INPUT_SAMPLE_RATE);
      SendMessage(hwnd_sample,CB_RESETCONTENT,0,0);
      std::for_each(fmts_sample_rates.begin(),fmts_sample_rates.end(),
        [&](fmts_sample_rates_t::value_type & pair_sample)
      {
        SendMessage(hwnd_sample,CB_ADDSTRING,0,(LPARAM)(boost::lexical_cast<std::wstring>(pair_sample.first).c_str()));
        SendMessage(hwnd_sample,CB_SETITEMDATA,cnt_samples,pair_sample.first);
        if(pair_sample.first == editing_input_params_.sample_rate)
        {
          SendMessage(hwnd_sample,CB_SETCURSEL,cnt_samples,0);
        }
        cnt_samples++;
      }
      );


      // ----------------------------------------
      // ----------------------------------------
      SetDlgItemInt(hwnd_,IDC_INPUT_LATENCY,editing_input_params_.latency/10000,TRUE);
      SetDlgItemText(hwnd_,IDC_INPUT_MIN_MAX_LATENCY,(boost::wformat(L"(%dms ～ %dms)") 
        % (i[current_input_driver_index_].latency_minimum_ / 10000) 
        % (i[current_input_driver_index_].latency_default_ / 10000)).str().c_str());
    } // 入力ダイアログ

    update_config_dialog_ = false;
  }

  void config_tab_dialog::enable_config_dialog(bool value)
  {
    // 出力部分
    enable_control(hwnd_,IDC_EXC_MODE,value);
    enable_control(hwnd_,IDC_EVENT_MODE,value);
    enable_control(hwnd_,IDC_ENDPOINT_DEVICE,value);
    enable_control(hwnd_,IDC_SAMPLE_BIT,value);
    enable_control(hwnd_,IDC_VALID_SAMPLE_BITS,value);
    enable_control(hwnd_,IDC_SAMPLE_RATE,value);
    enable_control(hwnd_,IDC_LATENCY,value);
    enable_control(hwnd_,IDC_CHANNEL,value);
    // 入力部分
    enable_control(hwnd_,IDC_INPUT_EXC_MODE,value);
    enable_control(hwnd_,IDC_INPUT_EVENT_MODE,value);
    enable_control(hwnd_,IDC_INPUT_ENDPOINT_DEVICE,value);
    enable_control(hwnd_,IDC_INPUT_SAMPLE_BIT,value);
    enable_control(hwnd_,IDC_INPUT_VALID_SAMPLE_BITS,value);
    enable_control(hwnd_,IDC_INPUT_SAMPLE_RATE,value);
    enable_control(hwnd_,IDC_INPUT_LATENCY,value);
    enable_control(hwnd_,IDC_INPUT_CHANNEL,value);
    // 設定反映・キャンセル部分
    if(value){
      enable_control(hwnd_,IDC_APPLY,edit_output_config_ || edit_input_config_);
      enable_control(hwnd_,IDC_CANCEL,edit_output_config_ || edit_input_config_);
    } else {
      enable_control(hwnd_,IDC_APPLY,false);
      enable_control(hwnd_,IDC_CANCEL,false);
    }

  }

  void config_tab_dialog::enable()
  {
    enable_config_dialog(true);
  }

  void config_tab_dialog::disable()
  {
    enable_config_dialog(false);
  }

}

