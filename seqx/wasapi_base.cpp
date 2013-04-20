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

#include "stdafx.h"
#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#include "audio_base.h"
#include "wasapi.h"
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Avrt.lib")
#pragma comment(lib, "Mmdevapi.lib")

using namespace std;
using namespace Microsoft::WRL;

namespace sf{

  // ==================================
  // コンストラクタ
  // ==================================

  // 共有/排他・タイマー・再生
  template<typename ShareModePolicy,typename DrivenPolicy,typename IOClientPolicy> 
  wasapi_base<ShareModePolicy,DrivenPolicy,IOClientPolicy>::wasapi_base(int device_index,wasapi_device_manager::device_info::params_t& params)
    : is_enabled_(false),position_(0),is_start_(false),driven_policy_(latency_)
  {
    init(wasapi_device_manager::instance()->output_device_infos().at(device_index),params);
  }

  // 共有・イベント・再生
  template<> 
  wasapi_base<wasapi_shared_policy,wasapi_event_policy,render_client_policy>::wasapi_base(int device_index,wasapi_device_manager::device_info::params_t& params)
    : is_enabled_(false),position_(0),is_start_(false),driven_policy_()
  {
    init(wasapi_device_manager::instance()->output_device_infos().at(device_index),params);
    audio_client_->SetEventHandle(driven_policy_.event_handle());
  }

  // 排他・イベント・再生
  template<> 
  wasapi_base<wasapi_exclusive_policy,wasapi_event_policy,render_client_policy>::wasapi_base(int device_index,wasapi_device_manager::device_info::params_t& params)
    : is_enabled_(false),position_(0),is_start_(false),driven_policy_()
  {
    init(wasapi_device_manager::instance()->output_device_infos().at(device_index),params);
    audio_client_->SetEventHandle(driven_policy_.event_handle());
  }

  // 共有・タイマ・キャプチャ
  template<> 
  wasapi_base<wasapi_shared_policy,wasapi_timer_policy,capture_client_policy>::wasapi_base(int device_index,wasapi_device_manager::device_info::params_t& params)
    : is_enabled_(false),position_(0),is_start_(false),driven_policy_(latency_)
  {
    init(wasapi_device_manager::instance()->input_device_infos().at(device_index),params);
  }

  // 排他・タイマ・キャプチャ
  template<> 
  wasapi_base<wasapi_exclusive_policy,wasapi_timer_policy,capture_client_policy>::wasapi_base(int device_index,wasapi_device_manager::device_info::params_t& params)
    : is_enabled_(false),position_(0),is_start_(false),driven_policy_(latency_)
  {
    init(wasapi_device_manager::instance()->input_device_infos().at(device_index),params);
  }

  // 共有・イベント・キャプチャ
  template<> 
  wasapi_base<wasapi_shared_policy,wasapi_event_policy,capture_client_policy>::wasapi_base(int device_index,wasapi_device_manager::device_info::params_t& params)
    : is_enabled_(false),position_(0),is_start_(false),driven_policy_()
  {
    init(wasapi_device_manager::instance()->input_device_infos().at(device_index),params);
    audio_client_->SetEventHandle(driven_policy_.event_handle());
  }

  // 排他・イベント・キャプチャ
  template<> 
  wasapi_base<wasapi_exclusive_policy,wasapi_event_policy,capture_client_policy>::wasapi_base(int device_index,wasapi_device_manager::device_info::params_t& params)
    : is_enabled_(false),position_(0),is_start_(false),driven_policy_()
  {
    init(wasapi_device_manager::instance()->input_device_infos().at(device_index),params);
    audio_client_->SetEventHandle(driven_policy_.event_handle());
  }

  // ========================================
  // デフォルトのオーディオクライアントの取得
  // ========================================

  template<typename ShareModePolicy,typename DrivenPolicy,typename IOClientPolicy> 
  void wasapi_base<ShareModePolicy,DrivenPolicy,IOClientPolicy>::get_default_audio_client()
  {

    current_device_ = &(wasapi_device_manager::instance()->current_output_device());
      {
        ComPtr<IActivateAudioInterfaceAsyncOperation> asyncOpPtr;
        IActivateAudioInterfaceAsyncOperation* asyncOp;
        ComPtr<ActivateAudioInterfaceCompletionHandler> handler(Make<ActivateAudioInterfaceCompletionHandler>());
        HRESULT hr = ActivateAudioInterfaceAsync(current_device_->id_.c_str(),__uuidof(IAudioClient2),nullptr,handler.Get(),&asyncOp);
        asyncOpPtr.Attach(asyncOp);
        if(SUCCEEDED(hr)){
          handler->wait();
          if(handler->ResultCode() == S_OK)
          {
            audio_client_ = handler->AudioClient();
          } else {
            throw win32_error_exception(handler->ResultCode());
          }
        } else {
          throw win32_error_exception(handler->ResultCode());
        }
      }

//	ActivateAudioInterface(current_device_->id_.c_str(),__uuidof(IAudioClient2),&audio_client_);
  }

  template<> 
  void wasapi_base<wasapi_exclusive_policy,wasapi_event_policy,capture_client_policy>
    ::get_default_audio_client()
  {
    current_device_ = &(wasapi_device_manager::instance()->current_output_device());
//	ActivateAudioInterface(current_device_->id_.c_str(),__uuidof(IAudioClient2),&audio_client_);
  }

  // ==================================
  // 初期化
  // ==================================

  template<typename ShareModePolicy,typename DrivenPolicy,typename IOClientPolicy> 
  void wasapi_base<ShareModePolicy,DrivenPolicy,IOClientPolicy>::init(wasapi_device_manager::device_info& device,wasapi_device_manager::device_info::params_t& params)
  {
    WAVEFORMATEXTENSIBLE wfx;
    bits_pair bits = {params.bits,params.valid_bits};
    make_wave_format(wfx,params.sample_rate,params.channel,bits);
    latency_ =  params.latency;//wasapi_device_manager::instance()->current_output_device().latency_default_;

    // 代替フォーマット定義
    sf::co_task_memory<WAVEFORMATEX>  alt_format;
      {
        ComPtr<IActivateAudioInterfaceAsyncOperation> asyncOpPtr;
        IActivateAudioInterfaceAsyncOperation* asyncOp;
        ComPtr<ActivateAudioInterfaceCompletionHandler> handler(Make<ActivateAudioInterfaceCompletionHandler>());
        HRESULT hr = ActivateAudioInterfaceAsync(device.id_.c_str(),__uuidof(IAudioClient2),nullptr,handler.Get(),&asyncOp);
        asyncOpPtr.Attach(asyncOp);
        if(SUCCEEDED(hr)){
          handler->wait();
          if(handler->ResultCode() == S_OK)
          {
            audio_client_ = handler->AudioClient();
          } else {
            throw win32_error_exception(handler->ResultCode());
          }
        } else {
          throw win32_error_exception(handler->ResultCode());
        }
      }
    current_device_ = &device;

    // 読みこもうとしているWAVファイルのフォーマットをサポートしているか？
    THROW_IF_ERR(audio_client_->IsFormatSupported(
      ShareModePolicy::SHARE_MODE ,&wfx.Format,&alt_format));
    //if(hr != S_OK) //  S_FALSEならそのフォーマットをサポートしていないらしい..
    //{
    //  // サンプルレートのコンバート
    //  if(alt_format->nSamplesPerSec != wfx.Format.nSamplesPerSec)
    //  {
    //    // 本来のサンプルレートをバックアップする
    //    sample_rate_backup = wfx.Format.nSamplesPerSec;
    //    // 代替フォーマットのサンプルレートをセットする
    //    wfx.Format.nSamplesPerSec = alt_format->nSamplesPerSec;
    //    // 再計算する
    //    wfx.Format.nAvgBytesPerSec = alt_format->nSamplesPerSec * wfx.Format.nBlockAlign;
    //    // もう一回チェックする。
    //    // サンプルレート以外でサポートしていない点があれば例外が発生する。
    //    THROW_IF_ERR(audio_client_->IsFormatSupported(
    //      ShareModePolicy::SHARE_MODE ,&wfx.Format,&alt_format));
    //    // フラグをセットする
    //    //          sample_rate_convert = true;
    //  } else {
    //    // サンプルレート以外であれば例外を出す。
    //    throw win32_error_exception(hr);
    //  }
    //}

    // クライアントの初期化

    try {
      init_audio_client(wfx);
    } catch (win32_error_exception& e)
    {
      if(e.hresult() == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED)
      {
        THROW_IF_ERR(audio_client_->GetBufferSize(&buffer_size_));
        latency_ = actual_latency_ = 
        (double)10000000 * (buffer_size_) / wfx.Format.nSamplesPerSec;
        audio_client_.Reset();
      {
        ComPtr<IActivateAudioInterfaceAsyncOperation> asyncOpPtr;
        IActivateAudioInterfaceAsyncOperation* asyncOp;
        ComPtr<ActivateAudioInterfaceCompletionHandler> handler(Make<ActivateAudioInterfaceCompletionHandler>());
        HRESULT hr = ActivateAudioInterfaceAsync(device.id_.c_str(),__uuidof(IAudioClient2),nullptr,handler.Get(),&asyncOp);
        asyncOpPtr.Attach(asyncOp);
        if(SUCCEEDED(hr)){
          handler->wait();
          if(handler->ResultCode() == S_OK)
          {
            audio_client_ = handler->AudioClient();
          } else {
            throw win32_error_exception(handler->ResultCode());
          }
        } else {
          throw win32_error_exception(handler->ResultCode());
        }
      }
        init_audio_client(wfx);

      } else {
        throw;
      }
    }
    actual_latency_ = latency_;

    // バッファサイズの取得
    THROW_IF_ERR(audio_client_->GetBufferSize(&buffer_size_));

    init_io_client(boost::type<IOClientPolicy>());

    // Audio Clock 2
    THROW_IF_ERR(audio_client_->GetService(IID_PPV_ARGS(&audio_clock2_)));
    REFERENCE_TIME psl;
    THROW_IF_ERR(audio_client_->GetStreamLatency(&psl));
#ifdef _DEBUG
    debug_out(boost::wformat(L"StreamLatency: %d \n") % psl);
#endif
    //      wdout

    // サンプルレートコンバータ
    //if(sample_rate_convert)
    //{
    //  THROW_IF_ERR(audio_client_->GetService(IID_PPV_ARGS(&audio_clock_adjustment_)));
    //  // 本来のサンプルレートをセットする
    //  audio_clock_adjustment_->SetSampleRate(sample_rate_backup);
    //}

    num_of_frames_ = wfx.Format.nBlockAlign;
    mix_format_ = wfx;
    //inc_  = (buffer_size_ * num_of_frames_) / (sizeof(short) * periods_per_buffer_);
    buffer_in_periods_ = buffer_size_ / periods_per_buffer_;

    is_enabled_ = true;
  }

  // =========================
  // AudioClientの初期化
  // =========================
  // デフォルト
  template<typename ShareModePolicy,typename DrivenPolicy,typename IOClientPolicy> 
  void wasapi_base<ShareModePolicy,DrivenPolicy,IOClientPolicy>::init_audio_client(WAVEFORMATEXTENSIBLE& wfx)
  {
      REFERENCE_TIME buffer_duration =  latency_;

      THROW_IF_ERR(audio_client_->Initialize(ShareModePolicy::SHARE_MODE  , 
      AUDCLNT_STREAMFLAGS_NOPERSIST | DrivenPolicy::STREAM_FLAG /*| AUDCLNT_STREAMFLAGS_RATEADJUST*/, 
      buffer_duration, 
      0,
      &(wfx.Format), 
      NULL));
  }

  // 排他・イベント・output
  template<> 
  void wasapi_base<wasapi_exclusive_policy,wasapi_event_policy,render_client_policy>::init_audio_client(WAVEFORMATEXTENSIBLE& wfx)
  {
    REFERENCE_TIME buffer_duration =  latency_;

    THROW_IF_ERR(audio_client_->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE  , 
      AUDCLNT_STREAMFLAGS_NOPERSIST | wasapi_event_policy::STREAM_FLAG /*| AUDCLNT_STREAMFLAGS_RATEADJUST*/, 
      buffer_duration, 
      buffer_duration,
      &(wfx.Format), 
      NULL));  
  }

  // 排他・タイマー・output
  template<> 
  void wasapi_base<wasapi_exclusive_policy,wasapi_timer_policy,render_client_policy>::init_audio_client(WAVEFORMATEXTENSIBLE& wfx)
  {
    REFERENCE_TIME buffer_period =  latency_;
    REFERENCE_TIME buffer_duration = buffer_period * periods_per_buffer_;

    THROW_IF_ERR(audio_client_->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE  , 
      AUDCLNT_STREAMFLAGS_NOPERSIST /*| AUDCLNT_STREAMFLAGS_RATEADJUST*/, 
      buffer_duration, 
      buffer_period,
      &(wfx.Format), 
      NULL));  
  }

  // 排他・イベント・キャプチャ
  template<> 
  void wasapi_base<wasapi_exclusive_policy,wasapi_event_policy,capture_client_policy>::init_audio_client(WAVEFORMATEXTENSIBLE& wfx)
  {
    REFERENCE_TIME buffer_duration =  latency_;

    THROW_IF_ERR(audio_client_->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE  , 
      AUDCLNT_STREAMFLAGS_NOPERSIST | wasapi_event_policy::STREAM_FLAG /*| AUDCLNT_STREAMFLAGS_RATEADJUST*/, 
      buffer_duration, 
      buffer_duration,
      &(wfx.Format), 
      NULL));  
  }

  // 排他・タイマー・キャプチャ
  template<> 
  void wasapi_base<wasapi_exclusive_policy,wasapi_timer_policy,capture_client_policy>::init_audio_client(WAVEFORMATEXTENSIBLE& wfx)
  {
    REFERENCE_TIME buffer_period =  latency_;
    REFERENCE_TIME buffer_duration = buffer_period * periods_per_buffer_;

    THROW_IF_ERR(audio_client_->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE  , 
      AUDCLNT_STREAMFLAGS_NOPERSIST /*| AUDCLNT_STREAMFLAGS_RATEADJUST*/, 
      buffer_duration, 
      buffer_period,
      &(wfx.Format), 
      NULL));  
  }
  // ================================
  // デストラクタ
  // ================================
  // デフォルト
  template<typename ShareModePolicy,typename DrivenPolicy,typename IOClientPolicy>
  wasapi_base<ShareModePolicy,DrivenPolicy,IOClientPolicy>::~wasapi_base()
  {
    //    safe_release(audio_clock_adjustment_);

    // WASAPIの終了処理
    if(audio_client_)
    {
      audio_client_->Stop();
      audio_client_->Reset();
      audio_client_.Reset();
    }
    safe_release(render_client_);
    safe_release(capture_client_);
    //safe_release(current_device_);
    //    safe_release(device_collection_);
  }

  // ================================
  // テスト用サイン波形テーブルの作成
  // ================================
  template<typename ShareModePolicy,typename DrivenPolicy,typename IOClientPolicy> 
  void wasapi_base<ShareModePolicy,DrivenPolicy,IOClientPolicy>::create_wave_data(){
    // サイン波の生成
    uint32_t buffer_size_in_bytes = buffer_size_ * mix_format_.Format.nBlockAlign;
    size_t render_data_length = mix_format_.Format.nSamplesPerSec * 10 /* 秒 */ * mix_format_.Format.nBlockAlign / sizeof(short);
    tone_buffer_.reserve(render_data_length);

    double sampleIncrement = (440 /* Hz */ * (M_PI * 2.0)) / (double)mix_format_.Format.nSamplesPerSec;
    double theta = 0.0;

    for (size_t i = 0 ; i < render_data_length ; i += mix_format_.Format.nChannels)
    {
      double sinValue = sin( theta );
      for(size_t j = 0 ;j < mix_format_.Format.nChannels; j++)
      {
        tone_buffer_.push_back((short)(sinValue * _I16_MAX));
      }
      theta += sampleIncrement;
    }
  };

  // ===========================
  // バッファを取得する
  // ===========================
  template<typename ShareModePolicy,typename DrivenPolicy,typename IOClientPolicy> 
  uint32_t wasapi_base<ShareModePolicy,DrivenPolicy,IOClientPolicy>::get_buffer(BYTE** buffer,uint32_t size)
  {

    uint32_t padding;
    uint32_t frames_available;

    THROW_IF_ERR(audio_client_->GetCurrentPadding(&padding));
    frames_available = size - padding;
    THROW_IF_ERR(render_client_->GetBuffer(frames_available,buffer));
    return frames_available;
  }
 
  // 排他・イベント・output
  template<> 
  uint32_t wasapi_base<wasapi_exclusive_policy,wasapi_event_policy,render_client_policy>::get_buffer(BYTE** buffer,uint32_t size)
  {
    THROW_IF_ERR(render_client_->GetBuffer(size,buffer));
    return size;
  }

  // キャプチャ ---------------------------------
  template<typename ShareModePolicy,typename DrivenPolicy,typename IOClientPolicy> 
  uint32_t wasapi_base<ShareModePolicy,DrivenPolicy,IOClientPolicy>::get_buffer(BYTE** buffer)
  {
    UINT32 size;
    DWORD flags;
    HRESULT hr = capture_client_->GetBuffer(buffer,&size,&flags,0,0);
    if(hr == AUDCLNT_S_BUFFER_EMPTY)
    {
      buffer = 0;
      size = 0;
    } else {
      THROW_IF_ERR(hr);
    }

    if(flags == AUDCLNT_BUFFERFLAGS_SILENT)
    {
      buffer = 0;
      size = 0;
    }
    return size;
  }

  // ===========================
  // バッファのリリース
  // ===========================
  template<typename ShareModePolicy,typename DrivenPolicy,typename IOClientPolicy> 
  void wasapi_base<ShareModePolicy,DrivenPolicy,IOClientPolicy>::release_buffer(uint32_t  num_byte_written)
  {
    THROW_IF_ERR(render_client_->ReleaseBuffer(num_byte_written,0));
  }

  template<> 
  void wasapi_base<wasapi_shared_policy,wasapi_event_policy,capture_client_policy>::release_buffer(uint32_t  num_byte_written)
  {
    THROW_IF_ERR(capture_client_->ReleaseBuffer(num_byte_written));
  }

  template<> 
  void wasapi_base<wasapi_shared_policy,wasapi_timer_policy,capture_client_policy>::release_buffer(uint32_t  num_byte_written)
  {
    THROW_IF_ERR(capture_client_->ReleaseBuffer(num_byte_written));
  }

  template<> 
  void wasapi_base<wasapi_exclusive_policy,wasapi_event_policy,capture_client_policy>::release_buffer(uint32_t  num_byte_written)
  {
    THROW_IF_ERR(capture_client_->ReleaseBuffer(num_byte_written));
  }

  template<> 
  void wasapi_base<wasapi_exclusive_policy,wasapi_timer_policy,capture_client_policy>::release_buffer(uint32_t  num_byte_written)
  {
    THROW_IF_ERR(capture_client_->ReleaseBuffer(num_byte_written));
  }

  // ===========================
  // バッファのスタート
  // ===========================

  template<typename ShareModePolicy,typename DrivenPolicy,typename IOClientPolicy> 
  void wasapi_base<ShareModePolicy,DrivenPolicy,IOClientPolicy>::start()
  {
    THROW_IF_ERR(audio_client_->Start());
    is_start_ = true;
  }

  // ===========================
  // バッファのリセット
  // ===========================

  template<typename ShareModePolicy,typename DrivenPolicy,typename IOClientPolicy> 
    void wasapi_base<ShareModePolicy,DrivenPolicy,IOClientPolicy>::reset()
  {
    THROW_IF_ERR(audio_client_->Reset());
  }

  // ===========================
  // バッファの停止
  // ===========================

  template<typename ShareModePolicy,typename DrivenPolicy,typename IOClientPolicy> 
  void wasapi_base<ShareModePolicy,DrivenPolicy,IOClientPolicy>::stop() {
    //再生停止
    if(is_start_)
    {
      THROW_IF_ERR(audio_client_->Stop());
      reset();
      is_start_ = false;

    };
  }

  // ===========================
  // パディングの取得
  // ===========================

  template<typename ShareModePolicy,typename DrivenPolicy,typename IOClientPolicy> 
  uint32_t wasapi_base<ShareModePolicy,DrivenPolicy,IOClientPolicy>::get_current_padding()
  {
    uint32_t padding;
    THROW_IF_ERR(audio_client_->GetCurrentPadding(&padding));
    return padding;
  }

  template<> 
  uint32_t wasapi_base<wasapi_exclusive_policy,wasapi_event_policy,render_client_policy>::get_current_padding()
  {
    return buffer_size_;
  }

  template struct wasapi_base<>;
  template struct wasapi_base<wasapi_shared_policy,wasapi_event_policy>;
  template struct wasapi_base<wasapi_exclusive_policy,wasapi_timer_policy>;
  template struct wasapi_base<wasapi_exclusive_policy,wasapi_event_policy>;
  template struct wasapi_base<wasapi_shared_policy,wasapi_timer_policy,capture_client_policy>;
  template struct wasapi_base<wasapi_shared_policy,wasapi_event_policy,capture_client_policy>;
  template struct wasapi_base<wasapi_exclusive_policy,wasapi_timer_policy,capture_client_policy>;
  template struct wasapi_base<wasapi_exclusive_policy,wasapi_event_policy,capture_client_policy>;

}


