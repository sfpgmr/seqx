#pragma once
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
// WASAPI
#include "boost/mpl/if.hpp"
#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type.hpp>
#include "exception.h"
#include "sf_memory.h"
#include <mmdeviceapi.h>
#include <AudioClient.h>
#include <audiopolicy.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <limits.h>

#include "audio_base.h"
//#include "audio_source.h"
//#include "Functiondiscoverykeys_devpkey.h"

// COM Smart Pointer 定義

//typedef Microsoft::WRL::ComPtr<IPropertyStore> IPropertyStorePtr;
typedef Microsoft::WRL::ComPtr<IAudioClient> IAudioClientPtr;
typedef Microsoft::WRL::ComPtr<IAudioClient2> IAudioClient2Ptr;
typedef Microsoft::WRL::ComPtr<IAudioClock> IAudioClockPtr;
typedef Microsoft::WRL::ComPtr<IAudioClock2> IAudioClock2Ptr;
typedef Microsoft::WRL::ComPtr<IAudioRenderClient> IAudioRenderClientPtr;
typedef Microsoft::WRL::ComPtr<IAudioCaptureClient> IAudioCaptureClientPtr;
typedef Microsoft::WRL::ComPtr<IAudioClockAdjustment> IAudioClockAdjustmentPtr;

namespace sf {
	namespace  mpl = boost::mpl;

	template <typename TSender,typename TResult>
	ref class typed_event_handler_adapter 
	{
	internal:
		typedef std::function<void (TSender,TResult)> wrapped_method_type; 
		typed_event_handler_adapter(wrapped_method_type wrapped_method)
			: wrapped_method_(wrapped_method) 
		{
			proxy_handler_ = 
				ref new  Windows::Foundation::TypedEventHandler<TSender, TResult>
				(this,&typed_event_handler_adapter::proxy_handler);
		}

		void proxy_handler(TSender sender,TResult result)
		{
			wrapped_method_(sender,result);
		}

		Windows::Foundation::TypedEventHandler<TSender, TResult>^ get()
		{
			return proxy_handler_;
		}
	private:
		wrapped_method_type wrapped_method_;
		Windows::Foundation::TypedEventHandler<TSender, TResult>^ proxy_handler_;
	};

	namespace en = Windows::Devices::Enumeration;
	namespace f = Windows::Foundation;

	ref class DeviceWatcherAdapter 
	{
  public:
    virtual ~DeviceWatcherAdapter()
    {
      added_.disconnect_all_slots();
    };
	internal:
		DeviceWatcherAdapter(en::DeviceClass deviceClass = en::DeviceClass::AudioRender)
		{
      watcher_ = en::DeviceInformation::CreateWatcher(deviceClass);
			watcher_->Added += ref new f::TypedEventHandler<en::DeviceWatcher^,en::DeviceInformation^>(this,&DeviceWatcherAdapter::Added);
			watcher_->EnumerationCompleted += ref new f::TypedEventHandler<en::DeviceWatcher^ , Platform::Object^ >(this,&DeviceWatcherAdapter::EnumerationCompleted);
			watcher_->Updated += ref new f::TypedEventHandler<en::DeviceWatcher^ , en::DeviceInformationUpdate^ >(this,&DeviceWatcherAdapter::Updated);
			watcher_->Removed += ref new f::TypedEventHandler<en::DeviceWatcher^ , en::DeviceInformationUpdate^ >(this,&DeviceWatcherAdapter::Removed);
			watcher_->Stopped += ref new f::TypedEventHandler<en::DeviceWatcher^ , Platform::Object^ >(this,&DeviceWatcherAdapter::Stopped);
		}
		boost::signals2::signal<void (en::DeviceWatcher^ sender
			, en::DeviceInformation^ deviceInterface) >& added(){return added_;}
		boost::signals2::signal<void (en::DeviceWatcher^ sender,
			Platform::Object^ obj) > &enumration_completed(){return enumeration_completed_;}
		boost::signals2::signal<void (en::DeviceWatcher^ sender,
			en::DeviceInformationUpdate^ deviceInfo) >& updated(){return updated_;}
		boost::signals2::signal<void (en::DeviceWatcher^ sender,
			en::DeviceInformationUpdate^ deviceInfo) >& removed() { return removed_;}
		boost::signals2::signal<void (en::DeviceWatcher^ sender,
			Platform::Object^ obj) >& stopped() {return stopped_;}
		void start()
		{
  			watcher_->Start();
		}
		void stop()
		{
  		watcher_->Stop();
		}
	private:
		void Added(en::DeviceWatcher^ sender, en::DeviceInformation^ deviceInterface)
		{
			if(!added_.empty()){
				added_(sender,deviceInterface);
			}
		}
		void EnumerationCompleted(en::DeviceWatcher^ sender, Platform::Object^ obj)
		{
			if(!enumeration_completed_.empty()){
				enumeration_completed_(sender,obj);
			}
		}
		void Updated(en::DeviceWatcher^ sender, en::DeviceInformationUpdate^ deviceInfo)
		{
			if(!updated_.empty()){
				updated_(sender,deviceInfo);
			}
		}
		void Removed(en::DeviceWatcher^ sender, en::DeviceInformationUpdate^ deviceInfo)
		{
			if(!removed_.empty()){
				removed_(sender,deviceInfo);
			}
		}
		void Stopped(en::DeviceWatcher^ sender, Platform::Object^ obj)
		{
			if(!stopped_.empty()){
				stopped_(sender,obj);
			}
		}
		en::DeviceWatcher^ watcher_;
		boost::signals2::signal<void (en::DeviceWatcher^ sender
			, en::DeviceInformation^ deviceInterface) > added_;
		boost::signals2::signal<void (en::DeviceWatcher^ sender, Platform::Object^ obj) > enumeration_completed_;
		boost::signals2::signal<void (en::DeviceWatcher^ sender, en::DeviceInformationUpdate^ deviceInfo) > updated_;
		boost::signals2::signal<void (en::DeviceWatcher^ sender, en::DeviceInformationUpdate^ deviceInfo) > removed_;
		boost::signals2::signal<void (en::DeviceWatcher^ sender, Platform::Object^ obj) > stopped_;
	};

	struct bits_pair 
	{
		int bits_per_sample;
		int valid_bits_per_sample;
	};

	void make_wave_format(WAVEFORMATEXTENSIBLE& format,
		int sample_rate,int channels,bits_pair bits,
		uint32_t type = WAVE_FORMAT_EXTENSIBLE,
		const GUID& sub_type = KSDATAFORMAT_SUBTYPE_PCM);

	void make_wave_format
		(WAVEFORMATEX& format,int sample_rate,int channels,
		int bits,uint32_t type = WAVE_FORMAT_PCM);


	struct wasapi_device_manager : singleton<wasapi_device_manager>
	{

    static const int NUM_SAMPLE_RATE = 12;
		static const int NUM_SAMPLE_BITS = 6;
		static const int sample_rates[NUM_SAMPLE_RATE];
		static const bits_pair sample_bits[NUM_SAMPLE_BITS];
		typedef std::map<int,std::map<int,std::map<int,std::map<int,std::map<int,int> > > > > format_map_type;
		struct device_info 
		{

			device_info(Windows::Devices::Enumeration::DeviceInformation^ info);
			~device_info();

			std::wstring id_;
			std::wstring name_;
			std::wstring display_name_;

			bool is_enabled_;
			bool is_selected_;
			bool is_default_;

			format_map_type support_formats_;
			REFERENCE_TIME latency_minimum_;
			REFERENCE_TIME latency_default_;
			struct params_t
			{
				params_t()
					: exclusive_mode(false),event_mode(false),bits(16),valid_bits(16),sample_rate(44100),latency(0),channel(2)
				{}
				bool exclusive_mode;
				bool event_mode;
				int bits;
				int valid_bits;
				int sample_rate;
				REFERENCE_TIME latency;
				int channel;
			private:
				friend class boost::serialization::access;
				template<class Archive>
				void serialize(Archive& ar, const unsigned int version)
				{
					ar & BOOST_SERIALIZATION_NVP(exclusive_mode);
					ar & BOOST_SERIALIZATION_NVP(event_mode);
					ar & BOOST_SERIALIZATION_NVP(bits);
					ar & BOOST_SERIALIZATION_NVP(valid_bits);
					ar & BOOST_SERIALIZATION_NVP(sample_rate);
					ar & BOOST_SERIALIZATION_NVP(latency);
					ar & BOOST_SERIALIZATION_NVP(channel);
				}
			};
			params_t params;
		private:
			Windows::Devices::Enumeration::DeviceInformation^ device_info_;
		};

		typedef std::vector<device_info> device_infos_t;

    typedef boost::signals2::signal<void (device_info& info) > device_added_t;
		typedef boost::signals2::signal<void () > device_enumeration_completed_t;
		typedef boost::signals2::signal<void (int device_index,device_info& info) > device_updated_t;
		typedef boost::signals2::signal<void (int device_index) > device_removed_t;

		wasapi_device_manager();
		~wasapi_device_manager();

		device_info& current_output_device(){return output_device_infos_[output_device_index_];};
		device_info& current_input_device() {return input_device_infos_[input_device_index_];};

		device_infos_t& output_device_infos() {return output_device_infos_;}
		device_infos_t& input_device_infos() {return input_device_infos_;}

		void select_output_device(int index) { assert(index < output_device_infos_.size()) ; output_device_index_ = index;}
		void select_input_device(int index) { assert(index < input_device_infos_.size()) ; input_device_index_ = index;}
		int current_output_device_index() const {return output_device_index_;}
		int current_input_device_index() const {return input_device_index_;}
		static const std::wstring & base_directory() {return base_directory_;};

    // 列挙が関連するまで待つ
    void wait_enum_devices()
    {
      while(!output_enum_completed_ || !input_enum_completed_)
      {
        Sleep(1);
      }
    }
    // デバイス監視を開始する
    void start_watching()
    {
      output_watcher_adapter_->start();
      input_watcher_adapter_->start();
    }
    // デバイス監視を停止する
    void stop_watching();
    // デバイス情報を保存する
    void save_params();

    device_added_t& device_added() {return device_added_;}
    device_enumeration_completed_t& device_enumeration_completed()  {return device_enumeration_completed_;}
    device_updated_t& device_updated(){return device_updated_;} 
    device_removed_t& device_removed(){return device_removed_;}

  private:
		void output_added(Windows::Devices::Enumeration::DeviceWatcher^ sender, Windows::Devices::Enumeration::DeviceInformation^ deviceInfo);
    void output_enumeration_completed(en::DeviceWatcher^ sender, Platform::Object^ obj);
		void output_updated(en::DeviceWatcher^ sender, en::DeviceInformationUpdate^ deviceInfo);
    void output_removed(en::DeviceWatcher^ sender, en::DeviceInformationUpdate^ deviceInfo);
		void output_stopped(en::DeviceWatcher^ sender, Platform::Object^ obj);

		void input_added(Windows::Devices::Enumeration::DeviceWatcher^ sender, Windows::Devices::Enumeration::DeviceInformation^ deviceInfo);
    void input_enumeration_completed(en::DeviceWatcher^ sender, Platform::Object^ obj);
		void input_updated(en::DeviceWatcher^ sender, en::DeviceInformationUpdate^ deviceInfo);
    void input_removed(en::DeviceWatcher^ sender, en::DeviceInformationUpdate^ deviceInfo);
		void input_stopped(en::DeviceWatcher^ sender, Platform::Object^ obj);

		int get_device_infos(Windows::Devices::Enumeration::DeviceClass data_flow, std::vector<device_info>& infos,const std::wstring& idd);

    int output_device_index_;// 現在選択しているoutputデバイスIndex
		int input_device_index_;// 現在選択している入力デバイスIndex
    // outputデバイス情報格納コンテナ
		std::vector<device_info> output_device_infos_;
    // 入力デバイス情報格納コンテナ
    std::vector<device_info> input_device_infos_;
    // 各デバイスの保存用ディレクトリ
		static const std::wstring base_directory_;
		std::wstring config_path_;
		// outputデバイス監視アダプタ
    DeviceWatcherAdapter^ output_watcher_adapter_;

    // 入力デバイス監視アダプタ
		DeviceWatcherAdapter^ input_watcher_adapter_;

    // outputデバイスの列挙が完了したか
    bool output_enum_completed_;
		// 入力デバイスの列挙が完了したか
    bool input_enum_completed_;

    // 現在選択しているoutputデバイスID
    std::wstring output_id_;
    // 現在選択している入力デバイスID
		std::wstring input_id_;
    // 既定のoutputデバイスID
    std::wstring default_output_id_;
    // 既定の入力デバイスID
    std::wstring default_input_id_;

    // 外部公開用シグナル変数
    device_added_t device_added_;
    device_enumeration_completed_t device_enumeration_completed_; 
    device_updated_t device_updated_;
    device_removed_t device_removed_;

  };

	// タイマーモードのポリシークラス
	struct wasapi_timer_policy
	{
		wasapi_timer_policy(REFERENCE_TIME & latency) : latency_(latency) {};
		void wait(){ Sleep(/*Concurrency::wait(*/latency_ / 20000L);}
		static const int STREAM_FLAG = 0;
	private:
		const REFERENCE_TIME& latency_;
	};

	// イベントモードのポリシークラス
	struct wasapi_event_policy
	{
		wasapi_event_policy() :  
			wait_events_(::CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE)) {}

		void wait()
		{

			WaitForSingleObjectEx(wait_events_.get(),INFINITE,FALSE);
			;
			//	Thread
			//	Windows::UI::Core::CoreWindow::GetForCurrentThread()
		}

		HANDLE event_handle(){return wait_events_.get();}
		static const int STREAM_FLAG = AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
	private:
		handle_holder wait_events_;
	};

	// 排他モードのポリシークラス
	struct wasapi_exclusive_policy 
	{
		static const AUDCLNT_SHAREMODE SHARE_MODE = AUDCLNT_SHAREMODE_EXCLUSIVE;  
	};

	// 共有モードのポリシークラス
	struct wasapi_shared_policy 
	{
		static const AUDCLNT_SHAREMODE SHARE_MODE = AUDCLNT_SHAREMODE_SHARED;
	};

	// レンダークライアントのポリシークラス
	struct render_client_policy 
	{
		//typedef IAudioRenderClientPtr client_ptr_t;
	};

	// キャプチャのポリシークラス
	struct capture_client_policy 
	{
		//typedef IAudioCaptureClientPtr client_ptr_t;
	};

	// キャプチャのポリシークラス
	struct loopback_capture_client_policy 
	{
		//typedef IAudioCaptureClientPtr client_ptr_t;
	};

	/// WASAPI処理クラス(デフォルト：共有・タイマーモード)
	template <typename ShareModePolicy = wasapi_shared_policy,typename DrivenPolicy = wasapi_timer_policy,typename IOClientPolicy = render_client_policy> 
	struct wasapi_base : public audio_base
	{
		friend IOClientPolicy;
		//typedef typename IOClientPolicy::client_ptr_t client_ptr_t;
		typedef wasapi_base<ShareModePolicy,DrivenPolicy> this_type;
		//wasapi_base();
		wasapi_base(int device_index,wasapi_device_manager::device_info::params_t& params);
		//explicit wasapi_base(::WAVEFORMATEXTENSIBLE& wfx);
		virtual ~wasapi_base();

		bool is_enabled () const {return is_enabled_;}

		/// テスト用 サイン波データの生成
		void create_wave_data();

		/// サウンド再生処理
		// bool play_buffer(BYTE* buffer);
		bool is_start(){ return is_start_; }
		// output用
		uint32_t get_buffer(BYTE** buffer,uint32_t size);
		// キャプチャ用
		uint32_t get_buffer(BYTE** buffer);

		void release_buffer(uint32_t num_byte_written);

		void start();
		void stop();
		void reset();
		void wait(){driven_policy_.wait();};
		uint32_t get_current_padding();
		uint64_t get_position() const 
		{
			uint64_t pos = 0;
			audio_clock2_->GetPosition(&pos,0);
			return pos;
		};
		uint32_t get_buffer_byte_size () const { return buffer_size_ * num_of_frames_;}
		uint32_t get_frame_size() const {return num_of_frames_;}
		uint32_t get_buffer_size () const { return buffer_size_; }
		uint64_t get_buffer_duration() { return latency_;}
		uint32_t get_channel() { return mix_format_.Format.nChannels; }
		win32_error_exception* const result() {return exception_holder_.get(); }

	private:

		void init_io_client( boost::type<render_client_policy>)
		{
			// 再生クライアントの取得
			THROW_IF_ERR(audio_client_->GetService(IID_PPV_ARGS(&render_client_)));
		};

		void init_io_client(boost::type<capture_client_policy>)
		{
			// 再生クライアントの取得
			THROW_IF_ERR(audio_client_->GetService(IID_PPV_ARGS(&capture_client_)));
		};

		void init_audio_client(WAVEFORMATEXTENSIBLE& wfx);
		DrivenPolicy driven_policy_;
		void get_default_audio_client();
		void init(wasapi_device_manager::device_info& device,wasapi_device_manager::device_info::params_t& params);
		// IMMDeviceEnumeratorPtr device_collection_;
		wasapi_device_manager::device_info* current_device_;
		//Windows::Devices::Enumeration::DeviceInformation^ current_device_;
		IAudioClient2Ptr audio_client_;
		IAudioRenderClientPtr render_client_;
		IAudioCaptureClientPtr capture_client_;
		IAudioClockPtr audio_clock2_;
		//IAudioClockAdjustmentPtr audio_clock_adjustment_;
		//handle_holder buffer_control_event_;
		WAVEFORMATEXTENSIBLE mix_format_;
		bool is_enabled_;
		bool is_start_;
		boost::shared_ptr<win32_error_exception> exception_holder_;
		boost::uint32_t num_of_frames_;
		boost::uint32_t buffer_size_;
		uint64_t position_;
		std::vector<short> tone_buffer_;
		size_t buffer_in_periods_;//  = buffer_size_ / periods_per_buffer_;

		// バッファ中の区切り数（レイテンシ時間が何個あるか）
		static const uint32_t periods_per_buffer_ = 1;
		//= mpl::if_c<boost::is_same<this_type,wasapi_base<wasapi_exclusive_policy,wasapi_timer_policy> >::value,
		//    mpl::int_<1>,mpl::int_<1> >::type::value;

		// 再生レイテンシ
		REFERENCE_TIME latency_;/* ms */
		REFERENCE_TIME actual_latency_;

	};

	typedef wasapi_base<wasapi_shared_policy,wasapi_timer_policy> wasapi_shared_timer;
	typedef wasapi_base<wasapi_exclusive_policy,wasapi_timer_policy> wasapi_exclusive_timer;

	typedef wasapi_base<wasapi_shared_policy,wasapi_event_policy> wasapi_shared_event;
	typedef wasapi_base<wasapi_exclusive_policy,wasapi_event_policy> wasapi_exclusive_event;

	typedef wasapi_base<wasapi_shared_policy,wasapi_timer_policy,capture_client_policy> wasapi_capture_shared_timer;
	typedef wasapi_base<wasapi_exclusive_policy,wasapi_timer_policy,capture_client_policy> wasapi_capture_exclusive_timer;

	typedef wasapi_base<wasapi_shared_policy,wasapi_event_policy,capture_client_policy> wasapi_capture_shared_event;
	typedef wasapi_base<wasapi_exclusive_policy,wasapi_event_policy,capture_client_policy> wasapi_capture_exclusive_event;


	struct ActivateAudioInterfaceCompletionHandler 
		: public  Microsoft::WRL::RuntimeClass<  Microsoft::WRL::RuntimeClassFlags<  Microsoft::WRL::ClassicCom >,  Microsoft::WRL::FtmBase, IActivateAudioInterfaceCompletionHandler >
	{
		ActivateAudioInterfaceCompletionHandler() :
			hr_(S_OK),
			ptr_(nullptr),
			eventHolder_(::CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE)) {};

		~ActivateAudioInterfaceCompletionHandler(){};

		// IActivateAudioInterfaceCompletionHandler
		STDMETHOD(ActivateCompleted)( IActivateAudioInterfaceAsyncOperation *operation )
		{
			HRESULT hr = S_OK;
			HRESULT hrActivateResult = S_OK;
			Microsoft::WRL::ComPtr<IUnknown> punkAudioInterface;
			// Check for a successful activation result
			hr = operation->GetActivateResult( &hrActivateResult, punkAudioInterface.GetAddressOf() );
			if(SUCCEEDED(hr)){
				if (SUCCEEDED( hrActivateResult ))
				{
					hr_ = S_OK;
					// Get the pointer for the Audio Client
					punkAudioInterface.As(&ptr_ );
  				if( nullptr == ptr_ )
					{
						hr_ = E_FAIL;
						::SetEvent(eventHolder_.get());
						return hr_;
					}
					::SetEvent(eventHolder_.get());
				} else {
					hr_ = hr = hrActivateResult;
					::SetEvent(eventHolder_.get());
				}
			} else {
				hr_ = hr;
				::SetEvent(eventHolder_.get());
			}
			return hr;
			//operation
			//    concurrency::wait(0);
		}

		void wait()
		{
			::WaitForSingleObjectEx(eventHolder_.get(),INFINITE,FALSE);
		}

		HRESULT ResultCode() {return hr_;}
		IAudioClient2Ptr AudioClient(){return ptr_;}
	private:
		HRESULT hr_;
		sf::handle_holder eventHolder_;
		IAudioClient2Ptr ptr_;
	};

}

