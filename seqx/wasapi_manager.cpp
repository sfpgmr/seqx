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
#include "sf_memory.h"
#include "audio_base.h"
#include "wasapi.h"
//#include "application.h"
//using namespace boost;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Media::Devices;
using namespace Concurrency;
using namespace Platform;
using namespace Microsoft::WRL;

namespace sf {
  const wstring wasapi_device_manager::base_directory_(L"."/* Windows::ApplicationModel::Package::Current->InstalledLocation->Path->Data() */);
  const int wasapi_device_manager::sample_rates[NUM_SAMPLE_RATE] = {8000,11025,16000,22050,24000,32000,44100,48000,88200,96000,176400,192000};
  const bits_pair wasapi_device_manager::sample_bits[NUM_SAMPLE_BITS] = {{8,8},{16,16},{24,24},{32,24},{32,32},{32,WAVE_FORMAT_IEEE_FLOAT}};

  void make_wave_format(WAVEFORMATEXTENSIBLE& format,int sample_rate,int channels,bits_pair b,uint32_t type,const GUID&  sub_type)
  {
    ZeroMemory(&format,sizeof(WAVEFORMATEXTENSIBLE));
    format.Format.wFormatTag = type;
    format.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
    format.SubFormat = sub_type;
    format.Format.nSamplesPerSec = sample_rate;
    format.Format.nChannels = channels;
    format.Format.wBitsPerSample = b.bits_per_sample;
    format.Format.nBlockAlign = (format.Format.wBitsPerSample / 8) * format.Format.nChannels;
    format.Format.nAvgBytesPerSec = format.Format.nSamplesPerSec  * format.Format.nBlockAlign;
    format.Samples.wValidBitsPerSample = b.valid_bits_per_sample;

    format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
  };

  void make_wave_format(WAVEFORMATEX& format,int sample_rate,int channels,int bits,uint32_t type)
  {
    ZeroMemory(&format,sizeof(WAVEFORMATEX));
    format.wFormatTag = type;
    format.nSamplesPerSec = sample_rate;
    format.nChannels = channels;
    format.wBitsPerSample = bits;
    format.nBlockAlign = (format.wBitsPerSample / 8) * format.nChannels;
    format.nAvgBytesPerSec = format.nSamplesPerSec  * format.nBlockAlign;
  };

 /* struct prop_variant 
  {
    prop_variant()
    {
      PropVariantInit(&value_);
    }

    ~prop_variant()
    {
      PropVariantClear(&value_);
    }

    PROPVARIANT* get(){ return &value_;};

    PROPVARIANT* operator &(){return get();}

    operator PROPVARIANT*() {return get();}

  private:
    PROPVARIANT value_;
  };*/


  wasapi_device_manager::wasapi_device_manager() 
    : output_device_index_(-1),input_device_index_(-1),output_enum_completed_(false),input_enum_completed_(false)
  {

    config_path_.append(base_directory_).append(L"\\wasapi_device_manager.config.xml");
    boost::filesystem::wpath config_path(config_path_);


    if(boost::filesystem::exists(config_path))
    {
      try {
        boost::filesystem::wifstream f(config_path);
        boost::archive::xml_wiarchive ar(f);
        ar & boost::serialization::make_nvp("output_device_id",output_id_);
        ar & boost::serialization::make_nvp("input_device_id" ,input_id_);
      } catch(...)
      {
        output_id_ = input_id_ = L"";
        boost::filesystem::remove(config_path);
      }
    }

    default_output_id_ = MediaDevice::GetDefaultAudioRenderId(AudioDeviceRole::Default)->Data();
    default_input_id_ = MediaDevice::GetDefaultAudioCaptureId(AudioDeviceRole::Default)->Data();

    //watcher_ = DeviceInformation::CreateWatcher();
    //dxgi_adapter_ = ref new typed_event_handler_adapter<DeviceWatcher^,DeviceInformation^>(boost::bind(&wasapi_device_manager::added,this,_1,_2));
    //  watcher_->Added += dxgi_adapter_->get();
    //watcher_->Added += (ref new typed_event_handler_adapter<DeviceWatcher^,DeviceInformation^>(boost::bind(&wasapi_device_manager::added,this,_1,_2)))->get();
    //watcher_->Start();

    output_watcher_adapter_ = ref new DeviceWatcherAdapter(en::DeviceClass::AudioRender);
    output_watcher_adapter_->added().connect(boost::bind(&wasapi_device_manager::output_added,this,_1,_2));
    output_watcher_adapter_->enumration_completed().connect(boost::bind(&wasapi_device_manager::output_enumeration_completed,this,_1,_2));
    output_watcher_adapter_->removed().connect(boost::bind(&wasapi_device_manager::output_removed,this,_1,_2));
    output_watcher_adapter_->updated().connect(boost::bind(&wasapi_device_manager::output_updated,this,_1,_2));
    output_watcher_adapter_->stopped().connect(boost::bind(&wasapi_device_manager::output_stopped,this,_1,_2));
    //output_watcher_adapter_->start();

    input_watcher_adapter_ = ref new DeviceWatcherAdapter(en::DeviceClass::AudioCapture);
    input_watcher_adapter_->added().connect(boost::bind(&wasapi_device_manager::input_added,this,_1,_2));
    input_watcher_adapter_->enumration_completed().connect(boost::bind(&wasapi_device_manager::input_enumeration_completed,this,_1,_2));
    input_watcher_adapter_->removed().connect(boost::bind(&wasapi_device_manager::input_removed,this,_1,_2));
    input_watcher_adapter_->updated().connect(boost::bind(&wasapi_device_manager::input_updated,this,_1,_2));
    input_watcher_adapter_->stopped().connect(boost::bind(&wasapi_device_manager::input_stopped,this,_1,_2));
    //input_watcher_adapter_->start();

  }//

  int wasapi_device_manager::get_device_infos(DeviceClass data_flow, std::vector<device_info>& infos,const std::wstring& idd)
  {

    // 対象デバイスを列挙する
    task<DeviceInformationCollection^> t((DeviceInformation::FindAllAsync(data_flow)));
    t.wait();
    DeviceInformationCollection^ collection = t.get();
    int current_index = 0;
    bool found = false;
    Platform::String^  default_id = ref new Platform::String();
    switch(data_flow)
    {
    case Windows::Devices::Enumeration::DeviceClass::AudioRender:
      default_id = MediaDevice::GetDefaultAudioRenderId(AudioDeviceRole::Default);
      break;
    case Windows::Devices::Enumeration::DeviceClass::AudioCapture:
      default_id = MediaDevice::GetDefaultAudioCaptureId(AudioDeviceRole::Default);
      break;
    }


    for(int i = 0 ,end = collection->Size;i < end;++i)
    {
      DeviceInformation^ info = collection->GetAt(i);
      infos.push_back(device_info(info));
      if(info->Id == default_id)
      {
        infos[i].is_default_ = true;
      }

      if(idd == wstring(info->Id->Data()) )
      {
        infos[i].is_selected_ = true;
        current_index = i;
        found = true;
      }
    }

    if(!found)
    {
      for(int i = 0,end = infos.size();i < end;++i)
      {
        if(infos[i].is_default_)
        {
          current_index = i;
          infos[i].is_selected_ = true;
        }
      }

    }
    return current_index;
  }

  wasapi_device_manager::~wasapi_device_manager()
  {
    //    stop_watching();
  }

  wasapi_device_manager::device_info::device_info
    (DeviceInformation^ info)
    :	id_(info->Id->Data()),
    name_(info->Name->Data()),	
    display_name_(dynamic_cast<String^>(info->Properties->Lookup(L"System.ItemNameDisplay"))->Data()),
    is_enabled_(info->IsEnabled),is_selected_(false),is_default_(info->IsDefault)
  {
    boost::filesystem::wpath 
      config_path(wasapi_device_manager::base_directory() + L"\\" + Windows::Foundation::Uri::EscapeComponent(info->Id)->Data() + L".xml");
    if(boost::filesystem::exists(config_path))
    {
      try{
        boost::filesystem::wifstream ifile(config_path);
        boost::archive::xml_wiarchive ar(ifile);
        ar >> BOOST_SERIALIZATION_NVP(params);
      } catch (...) 
      {
        // ファイル読み込みに問題がある場合はファイルを消す。
        boost::filesystem::remove(config_path);
      }
    }
  }

  void wasapi_device_manager::stop_watching()
  {
    input_watcher_adapter_->stop();
    output_watcher_adapter_->stop();
  }

  void wasapi_device_manager::save_params()
  {
    // watcherの停止
    // stop_watching();

    // 設定の保存
    try{
      boost::filesystem::wpath config_path(config_path_);
      boost::filesystem::wofstream f(config_path,std::ios_base::out | std::ios_base::trunc);
      boost::archive::xml_woarchive ar(f);
      ar & boost::serialization::make_nvp("output_device_id",current_output_device().id_);
      ar & boost::serialization::make_nvp("input_device_id" ,current_input_device().id_);
    }catch(...) {

    }

    // outputデバイスパラメータの保存
    for(device_info& inf : output_device_infos_)
    {

      try {
        boost::filesystem::wpath 
          config_path(wasapi_device_manager::base_directory() + L"\\" +  Windows::Foundation::Uri::EscapeComponent(ref new Platform::String(inf.id_.c_str()))->Data() + L".xml");
        boost::filesystem::wofstream ofile(config_path,std::ios_base::out | ios_base::trunc);
        boost::archive::xml_woarchive ar(ofile);
        ar << BOOST_SERIALIZATION_NVP(inf.params);
      } catch(...)
      {

      }
    }

    // 入力デバイスパラメータの保存
    for(device_info& inf : input_device_infos_)
    {

      try {
        boost::filesystem::wpath 
          config_path(wasapi_device_manager::base_directory() + L"\\" + Windows::Foundation::Uri::EscapeComponent(ref new Platform::String(inf.id_.c_str()))->Data() + L".xml");
        boost::filesystem::wofstream ofile(config_path,std::ios_base::out | ios_base::trunc);
        boost::archive::xml_woarchive ar(ofile);
        ar << BOOST_SERIALIZATION_NVP(inf.params);
      } catch(...)
      {

      }
    }

  }

  wasapi_device_manager::device_info::~device_info()
  {
  }

  void wasapi_device_manager::output_added(Windows::Devices::Enumeration::DeviceWatcher^ sender, Windows::Devices::Enumeration::DeviceInformation^ deviceInfo)
  {
    device_info info(deviceInfo);

    IAudioClient2Ptr c;
    // オーディオクライアントを取得
    {
      ComPtr<IActivateAudioInterfaceAsyncOperation> asyncOpPtr;
      ComPtr<ActivateAudioInterfaceCompletionHandler> handler(Make<ActivateAudioInterfaceCompletionHandler>());
      HRESULT hr = ActivateAudioInterfaceAsync(info.id_.c_str(),__uuidof(IAudioClient2),nullptr,handler.Get(),asyncOpPtr.GetAddressOf());
      if(SUCCEEDED(hr)){
        handler->wait();
        if(handler->ResultCode() == S_OK)
        {
          c = handler->AudioClient();
        } else {
          throw win32_error_exception(handler->ResultCode());
        }
      } else {
        throw win32_error_exception(handler->ResultCode());
      }
    }
    c->GetDevicePeriod(&(info.latency_default_),&(info.latency_minimum_));
    if(info.params.latency == 0 
      || info.params.latency < info.latency_minimum_ )
    {
      info.params.latency = info.latency_default_;
    }

#ifdef _DEBUG
    debug_out(info.name_ + L"\n");
    debug_out(boost::wformat(L"latency default:%d min:%d \n") % info.latency_default_ % info.latency_minimum_);
#endif
    for(int bits = 0;bits < NUM_SAMPLE_BITS;++bits)
    {
      for(int channel = 1;channel < 3;++channel)
      {
        for(int rate = 0;rate < NUM_SAMPLE_RATE;++rate)
        {
          sf::co_task_memory<WAVEFORMATEXTENSIBLE>  a,a1;
          WAVEFORMATEXTENSIBLE f;

          //            make_wave_fomat(f,sample_rates[rate],channel,sample_bits[bits]);
          if(sample_bits[bits].valid_bits_per_sample == WAVE_FORMAT_IEEE_FLOAT)
          {
            bits_pair b ={sample_bits[bits].bits_per_sample,sample_bits[bits].bits_per_sample};
            make_wave_format(f,sample_rates[rate],channel,b,WAVE_FORMAT_EXTENSIBLE,KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);
          } else {
            make_wave_format(f,sample_rates[rate],channel,sample_bits[bits],WAVE_FORMAT_EXTENSIBLE,KSDATAFORMAT_SUBTYPE_PCM);
          }

          // 排他モード
          HRESULT hr = c->IsFormatSupported(
            AUDCLNT_SHAREMODE_EXCLUSIVE,reinterpret_cast<WAVEFORMATEX*>(&f),reinterpret_cast<WAVEFORMATEX**>(&a));
          if(hr == S_OK){
            info.support_formats_[AUDCLNT_SHAREMODE_EXCLUSIVE][ sample_bits[bits].bits_per_sample ][sample_bits[bits].valid_bits_per_sample][channel][sample_rates[rate]] = 0;
#ifdef _DEBUG
            debug_out(boost::wformat(L"|exc  |bits:%02d|vbits:%02d|channel:%02d|rate:%06d|%s|\n") % sample_bits[bits].bits_per_sample % sample_bits[bits].valid_bits_per_sample % channel % sample_rates[rate] % (hr == S_OK?L"OK":L"NG"));
#endif            }

            // make_wave_format(f,sample_rates[rate],channel,sample_bits[bits],WAVE_FORMAT_EXTENSIBLE);
            // 共有モード
            hr = c->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,reinterpret_cast<WAVEFORMATEX*>(&f),reinterpret_cast<WAVEFORMATEX**>(&a1));
            if(hr == S_OK){
              info
                .support_formats_
                [AUDCLNT_SHAREMODE_SHARED]
              [ sample_bits[bits].bits_per_sample ]
              [sample_bits[bits].valid_bits_per_sample]
              [channel][sample_rates[rate]] 
              = 0;
#ifdef _DEBUG
              debug_out(boost::wformat(L"|share|bits:%02d|vbits:%02d|channel:%02d|rate:%06d|%s|\n")
                % sample_bits[bits].bits_per_sample 
                % sample_bits[bits].valid_bits_per_sample 
                % channel % sample_rates[rate] 
              % (hr == S_OK?L"OK":L"NG"));

#endif
            }
        }
      }
    }
#ifdef _DEBUG
    debug_out(L"-------------------------------\n");
#endif
  }

  if(output_id_ == deviceInfo->Id->Data())
  {
    info.is_selected_ = true;
  }
  if(default_output_id_ == deviceInfo->Id->Data())
  {
    info.is_default_ = true;
  }
  output_device_infos_.push_back(info);

#ifdef _DEBUG
  wdout << deviceInfo->Name->Data() << std::endl;
#endif
}

void wasapi_device_manager::output_enumeration_completed(en::DeviceWatcher^ sender, Platform::Object^ obj)
{

  for(int i = 0;i < output_device_infos_.size();++i)
  {
    device_info& inf(output_device_infos_[i]);
    if(inf.is_selected_ && inf.is_enabled_)
    {
      output_device_index_ = i;
    }
  }

  if(output_device_index_ == -1)
  {
    for(int i = 0;i < output_device_infos_.size();++i)
    {
      device_info& inf(output_device_infos_[i]);
      if(inf.is_default_)
      {
        output_device_index_ = i;
        inf.is_selected_ = true;
      }
    }
  }

  output_enum_completed_ = true;
}

void wasapi_device_manager::output_updated(en::DeviceWatcher^ sender, en::DeviceInformationUpdate^ deviceInfo)
{
}

void wasapi_device_manager::output_removed(en::DeviceWatcher^ sender, en::DeviceInformationUpdate^ deviceInfo)
{
}

void wasapi_device_manager::output_stopped(en::DeviceWatcher^ sender, Platform::Object^ obj)
{
}

void wasapi_device_manager::input_added(Windows::Devices::Enumeration::DeviceWatcher^ sender, Windows::Devices::Enumeration::DeviceInformation^ deviceInfo)
{
  device_info info(deviceInfo);
  #ifdef _DEBUG
    debug_out(L"================================================\n");
    debug_out(boost::wformat(L"%d \n %s \n" ) % info.id_ % info.display_name_);
    debug_out(boost::wformat(L"%d \n") % info.params.latency);
    debug_out(L"================================================\n");
 #endif

  IAudioClient2Ptr c;
  // オーディオクライアントを取得
  //    THROW_IF_ERR(ActivateAudioInterface(info.id_.c_str(),__uuidof(IAudioClient2),&c)      );          
  {
    ComPtr<IActivateAudioInterfaceAsyncOperation> asyncOpPtr;
//    IActivateAudioInterfaceAsyncOperation* asyncOp;
    ComPtr<ActivateAudioInterfaceCompletionHandler> handler(Make<ActivateAudioInterfaceCompletionHandler>());
    HRESULT hr = ActivateAudioInterfaceAsync(info.id_.c_str(),__uuidof(IAudioClient2),nullptr,handler.Get(),asyncOpPtr.GetAddressOf());
//    asyncOpPtr.Attach(asyncOp);
    if(SUCCEEDED(hr)){
      handler->wait();
      if(handler->ResultCode() == S_OK)
      {
        c = handler->AudioClient();
      } else {
        throw win32_error_exception(handler->ResultCode());
      }
    } else {
      throw win32_error_exception(handler->ResultCode());
    }
  }
  c->GetDevicePeriod(&(info.latency_default_),&(info.latency_minimum_));
  if(info.params.latency == 0 || info.params.latency < info.latency_minimum_ ){
    info.params.latency = info.latency_default_;
  }
#ifdef _DEBUG
  debug_out(info.display_name_);
  debug_out(boost::wformat(L"\nlatency default:%d min:%d \n") % info.latency_default_ % info.latency_minimum_);
#endif
  for(int bits = 0;bits < NUM_SAMPLE_BITS;++bits)
  {
    for(int channel = 1;channel < 3;++channel)
    {
      for(int rate = 0;rate < NUM_SAMPLE_RATE;++rate)
      {
        sf::co_task_memory<WAVEFORMATEXTENSIBLE>  a,a1;
        WAVEFORMATEXTENSIBLE f;

        // make_wave_fomat(f,sample_rates[rate],channel,sample_bits[bits]);
        if(sample_bits[bits].valid_bits_per_sample == WAVE_FORMAT_IEEE_FLOAT)
        {
          bits_pair b ={sample_bits[bits].bits_per_sample,sample_bits[bits].bits_per_sample};
          make_wave_format(f,sample_rates[rate],channel,b,WAVE_FORMAT_EXTENSIBLE,KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);
        } else {
          make_wave_format(f,sample_rates[rate],channel,sample_bits[bits],WAVE_FORMAT_EXTENSIBLE,KSDATAFORMAT_SUBTYPE_PCM);
        }

        // 排他モード
        HRESULT hr = c->IsFormatSupported(
          AUDCLNT_SHAREMODE_EXCLUSIVE,reinterpret_cast<WAVEFORMATEX*>(&f),reinterpret_cast<WAVEFORMATEX**>(&a));
        if(hr == S_OK){
          info
            .support_formats_
            [AUDCLNT_SHAREMODE_EXCLUSIVE]
          [ sample_bits[bits].bits_per_sample ]
          [sample_bits[bits].valid_bits_per_sample]
          [channel]
          [sample_rates[rate]] = 0;
#ifdef _DEBUG
          debug_out(boost::wformat(L"|exc  |bits:%02d|vbits:%02d|channel:%02d|rate:%06d|%s|\n") 
            % sample_bits[bits].bits_per_sample 
            % sample_bits[bits].valid_bits_per_sample 
            % channel % sample_rates[rate] 
          % (hr == S_OK?L"OK":L"NG"));
#endif            }

          // make_wave_format(f,sample_rates[rate],channel,sample_bits[bits],WAVE_FORMAT_EXTENSIBLE);
          // 共有モード
          hr = c->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,reinterpret_cast<WAVEFORMATEX*>(&f),reinterpret_cast<WAVEFORMATEX**>(&a1));
          if(hr == S_OK){
            info.
              support_formats_
              [AUDCLNT_SHAREMODE_SHARED]
            [sample_bits[bits].bits_per_sample ]
            [sample_bits[bits].valid_bits_per_sample]
            [channel]
            [sample_rates[rate]] = 0;
#ifdef _DEBUG
            debug_out(boost::wformat(L"|share|bits:%02d|vbits:%02d|channel:%02d|rate:%06d|%s|\n") 
              % sample_bits[bits].bits_per_sample 
              % sample_bits[bits].valid_bits_per_sample 
              % channel 
              % sample_rates[rate] 
            % (hr == S_OK?L"OK":L"NG"));

#endif
          }
      }
    }
  }
#ifdef _DEBUG
  debug_out(L"-------------------------------\n");
#endif
}

if(input_id_ == deviceInfo->Id->Data())
{
  info.is_selected_ = true;
}
if(default_input_id_ == deviceInfo->Id->Data())
{
  info.is_default_ = true;
}

input_device_infos_.push_back(info);

#ifdef _DEBUG
//    if(deviceInfo->IsEnabled){
wdout << deviceInfo->Name->Data() << std::endl;

//		  auto i = deviceInfo->Properties->First();
//		  while(i->HasCurrent)
//		  {
////			  wdout << iCurrent->Key->Data() << L"," << i->Current->Value->ToString()->Data() << std::endl;
//        wdout << i->
//			  i->MoveNext();
//		  }
//    }
#endif
  }

  void wasapi_device_manager::input_enumeration_completed(en::DeviceWatcher^ sender, Platform::Object^ obj)
  {

    for(int i = 0;i < input_device_infos_.size();++i)
    {
      device_info& inf(input_device_infos_[i]);
      if(inf.is_selected_ && inf.is_enabled_)
      {
        input_device_index_ = i;
      }
    }

    if(input_device_index_ == -1)
    {
      for(int i = 0;i < input_device_infos_.size();++i)
      {
        device_info& inf(input_device_infos_[i]);
        if(inf.is_default_)
        {
          input_device_index_ = i;
          inf.is_selected_ = true;
        }
      }
    }
    input_enum_completed_ = true;
  }

  void wasapi_device_manager::input_updated(en::DeviceWatcher^ sender, en::DeviceInformationUpdate^ deviceInfo)
  {
  }

  void wasapi_device_manager::input_removed(en::DeviceWatcher^ sender, en::DeviceInformationUpdate^ deviceInfo)
  {
    if(input_device_infos_[input_device_index_].id_ == deviceInfo->Id->Data())
    {
      
    }
    input_device_infos_.erase(std::remove_if(input_device_infos_.begin(),input_device_infos_.end(),
      [&](device_infos_t::reference p) -> bool 
    {
      return p.id_ == deviceInfo->Id->Data();
    }),input_device_infos_.end());
  }

  void wasapi_device_manager::input_stopped(en::DeviceWatcher^ sender, Platform::Object^ obj)
  {
  }
}