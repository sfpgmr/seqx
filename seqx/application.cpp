/*
==============================================================================

This file is part of the async
Copyright 2005-10 by Satoshi Fujiwara.

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
#include "StdAfx.h"
#include <commctrl.h>


#include "message_loop.h"
#include "sf_com.h"
#include "application.h"
#include "dout.h"
#include "midi_input.h"
#include "midi_output.h"

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif


using namespace std;

namespace sf {
#ifdef _DEBUG
  std::wstring application::app_id_(L"SF.async_debug");
#else
  std::wstring application::app_id_(L"SF.async");
#endif

  application::application()
  {
    instance_handle_ = HINST_THISCOMPONENT;
    // ベースディレクトリの取得
    wchar_t dir[MAX_PATH];
    ::GetCurrentDirectoryW(MAX_PATH,dir);
    base_directory_.assign(dir);
  }

  application::~application()
  {
  };

  // アプリケーションを実行する
  int application::execute(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
  {
#ifdef _DEBUG
    ::_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
#endif
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // wcoutで文字化けしないように、ロケールをデフォルト言語に設定する
    std::wcout.imbue(std::locale(""));
    wdout.imbue(std::locale(""));

    // 2重起動の防止処理
    SECURITY_DESCRIPTOR sd;
    InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);	    
    SECURITY_ATTRIBUTES sec_attr;
    sec_attr.nLength = sizeof (sec_attr);
    sec_attr.lpSecurityDescriptor = &sd;
    sec_attr.bInheritHandle = TRUE; 
#ifdef _DEBUG 
    sf::handle_holder handle(::CreateMutex(&sec_attr, FALSE, _T("async_mutex_debug")));
#else
    sf::handle_holder handle(::CreateMutex(&sec_attr, FALSE, _T("async_mutex")));
#endif
    if(NULL == handle.get() || ::GetLastError() == ERROR_ALREADY_EXISTS)
    {
      return 0;
    }

    // プロセスの優先度を高に設定する
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    // コモンコントロールの初期化 
    static const INITCOMMONCONTROLSEX common_ctrls =
    {
      sizeof(INITCOMMONCONTROLSEX),
      ICC_STANDARD_CLASSES | ICC_BAR_CLASSES
    };
    InitCommonControlsEx(&common_ctrls);


    // COMをマルチスレッドで初期化
    sf::com_initialize init(0,multi_threaded);

    // Win RTの初期化
    RoInitialize(RO_INIT_MULTITHREADED);

    // プロセスの優先順位をPro Audioに変更する
    sf::av_mm_thread_characteristics avmm(wstring(L"Pro Audio"));
    avmm.set_priority(AVRT_PRIORITY_HIGH);

    // アプリケーションIDの登録
    sf::throw_if_err<application::exception>()(SetCurrentProcessExplicitAppUserModelID(app_id_.c_str()));

    // マルチメディアタイマの精度を1msに設定する
    timeBeginPeriod(1);

    // デバイス監視の開始
    sf::wasapi_device_manager::instance()->start_watching();

    // デバイス列挙を待つ
    sf::wasapi_device_manager::instance()->wait_enum_devices();
    // sf::wasapi_device_manager::instance()->current_output_device();

    // MIDIデバイスの列挙
    const midi_device_manager::ptr& ptr = midi_device_manager::instance();

    // ダイアログウィンドウを作成する
    window_ = sf::create_toplevel_window(
      std::wstring(L"Media Player サンプル"),std::wstring(L"Media Player サンプル"));
    ////  ファイルリーダーエージェントの起動
    //reader_agent_.start();
    //// キャプチャエージェントの起動
    //input_agent_.start();
    //// wasapi入力スレッドの起動
    //input_agent_.wait_event();

    // デフォルトの入出力デバイスをセットする
    try {
      // wasapi出力スレッドの起動
      output_agent_.start();
      output_agent_.wait_status(output_agent_t::status_device_config_ok);
      apply_output_device_config
        (wasapi_device_manager::instance()->current_output_device_index(),
        wasapi_device_manager::instance()->current_output_device().params);
      //apply_input_device_config
      //  (wasapi_device_manager::instance()->current_input_device_index(),
      //  wasapi_device_manager::instance()->current_input_device().params);
    } catch (...) {

    }


    // メッセージ処理ループ
    WPARAM ret = sf::dialog_message_loop(reinterpret_cast<HWND>(window_->raw_handle()))();

    // マルチメディアタイマの粒度を元に戻す。
    timeEndPeriod(1);

    output_agent_.change_status(output_agent_t::status_exit);

    //  スレッド終了待ち
    //std::array<Concurrency::agent*,3> agents = {/*&mixer_agent_,*/&reader_agent_,&output_agent_,&input_agent_};
    //Concurrency::agent::wait_for_all(agents.size(),&agents[0]);
    Concurrency::agent::wait(&output_agent_);

    // プレイヤーのシャットダウン処理を行いリソースを解放する
//    player_->process_event(sf::player::ev::Close());

    return ret;
  }

  void application::reader_setup(const std::wstring& file_path)
  {
    try {
      reader_agent_.setup(file_path);
      //window_->reader_ready();
    } catch (win32_error_exception& e) {
      window_->message_box((boost::wformat(L"ファイル読み込み時にエラーが発生しました。%s") % e.error()).str(),wstring(L"ファイル読込エラー"));
    }
  }

  void application::reader_read_file()
  {
    try {
      reader_agent_.read_file();
      //window_->reader_read_file();
    } catch (win32_error_exception& e) {
      window_->message_box((boost::wformat(L"再生開始時にエラーが発生しました。%s") % e.error()).str(),wstring(L"再生エラー"));
    }
  }

  void application::reader_pause()
  {
    try {
      reader_agent_.pause();
      if(reader_agent_.status() == reader_agent_t::status_pause_ok)
      {
        //window_->reader_pause();
      } else {
        //window_->reader_read_file();
      }
    } catch (win32_error_exception& e) {
      window_->message_box((boost::wformat(L"一時停止時にエラーが発生しました。%s") % e.error()).str(),wstring(L"一時停止エラー"));
    }
  }

  void application::reader_stop()
  {
    try {
      reader_agent_.stop();
      //window_->reader_stop();
    } catch (win32_error_exception& e) {
      window_->message_box((boost::wformat(L"停止時にエラーが発生しました。%s") % e.error()).str(),wstring(L"停止エラー"));
    }
  }

  void application::reader_end()
  {
    //window_->reader_stop();
  }

  void application::apply_output_device_config(int device_index,wasapi_device_manager::device_info::params_t& params)
  {
    // スレッドの休止
    if( reader_agent_.agent_status() == Concurrency::agent_status::agent_runnable &&
      reader_agent_.agent_status() == Concurrency::agent_status::agent_started)
    {
      int reader_status = reader_agent_.status();
      if(reader_status != reader_agent_t::status_none){
        reader_agent_.change_and_wait(reader_agent_t::status_config,reader_agent_t::status_config_ok);
      }
    }

    // int mixer_status = mixer_agent_.status();
    // mixer_agent_.change_and_wait(mixer_agent_t::status_config,mixer_agent_t::status_config_ok);

    if( input_agent_.agent_status() == Concurrency::agent_status::agent_runnable &&
      input_agent_.agent_status() == Concurrency::agent_status::agent_started)
    {
      input_agent_.change_and_wait(input_agent_t::status_pause,input_agent_t::status_pause_ok);
    }
    
    // outputの変更
    output_agent_.apply_config(device_index,params);

    //  mixer_agent_.init_buffer();
    //  mixer_agent_.change_and_wait(mixer_agent_t::status_process,mixer_agent_t::status_processing);
    if( reader_agent_.agent_status() == Concurrency::agent_status::agent_runnable &&
      reader_agent_.agent_status() == Concurrency::agent_status::agent_started){
      reader_agent_.init_buffer();
      reader_agent_.change_and_wait(reader_agent_t::status_ready,reader_agent_t::status_ready_ok);
    }

    if( input_agent_.agent_status() == Concurrency::agent_status::agent_runnable &&
      input_agent_.agent_status() == Concurrency::agent_status::agent_started)
    {
      int input_status = input_agent_.status();
      input_agent_.init_buffer();
      //input_agent_.apply_config(
      //wasapi_device_manager::instance()->current_input_device_index(),
      //wasapi_device_manager::instance()->current_input_device().params);
      input_agent_.change_and_wait(input_status - 1,input_status);
    }

  }

  // Playerを初期化する。
  //void application::init_player(HWND vhwnd,HWND hwnd)
  //{
  //  player_ = player::CreatePlayer(vhwnd,hwnd);
  //  player_init_ok_();
  //}
  
}

