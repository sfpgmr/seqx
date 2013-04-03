#pragma once
/** @file
*  @brief MIDI Outputのヘッダーファイル
*/
#include "boost/detail/lightweight_test.hpp"
#include "midi_input.h"
#include "midi_output.h"
#include "midi_message.h"
namespace sf {
class midi_test
{
public:
  midi_test()
  {
    // MIDI IN デバイスを検索してコンテナに格納する
    sf::midi入力::デバイスの列挙();

    // MIDI OUT デバイスを検索してコンテナに格納する
    sf::midi出力::デバイスの列挙();

    入力_.reset(new midi入力(sf::midi入力::デバイス情報コンテナ().begin()->id()));
    出力_.reset(new midi出力(sf::midi入力::デバイス情報コンテナ().begin()->id()));
    
    入力_->MIDIメッセージ到着時.connect(boost::bind(&midi_test::MIDIメッセージ到着時,*this,_1,_2,_3));

  }

  ~midi_test()
  {
    ::midiDisconnect((HMIDI)入力_->ハンドル(),出力_->ハンドル(),NULL);
  };

  void run_test()
  {


    // コンテナに格納したデバイス名を表示
    std::for_each(sf::midi入力::デバイス情報コンテナ().begin(),sf::midi入力::デバイス情報コンテナ().end()
      ,[](const sf::midi入力::caps& c)
    {
      sf::wdout <<  L"MIDI Input Device: " << c.name() << std::endl;
    }
    );

    // MIDI Inputの簡単なテスト
    if(!sf::midi入力::デバイス情報コンテナ().empty())
    {
      try{
        入力_->受信開始();
        入力_->受信停止();
        入力_->リセット();
        入力_->クローズ();
      } catch (sf::midi入力エラー& e)
      {
        sf::wdout << boost::wformat(L"Error: %s") % e.what() << std::endl;
      }
    }

  
    // コンテナに格納したデバイス名を表示
    std::for_each(sf::midi出力::デバイス情報コンテナ().begin(),sf::midi出力::デバイス情報コンテナ().end()
      ,[](const sf::midi出力::デバイス情報& c)
    {
      sf::wdout << L"MIDI Output Device: " << c.名前() << std::endl;
    }
    );

    // MIDI Outputの簡単なテスト
    if(!sf::midi出力::デバイス情報コンテナ().empty())
    {
      try{
        出力_->リセット();
        出力_->クローズ();
      } catch (sf::midi出力エラー& e)
      {
        sf::wdout << boost::wformat(L"Error: %s") % e.what() << std::endl;
      }
    }

    出力_->オープン();
    入力_->オープン();

    // 
    ::midiConnect((HMIDI)入力_->ハンドル(),出力_->ハンドル(),NULL);
    入力_->受信開始();
  }

  void MIDIメッセージ到着時( boost::uint32_t メッセージ, boost::uint32_t MIDIメッセージ, boost::uint32_t タイムスタンプ)
  {

    switch( メッセージ )
    {
      // normal data message
    case MIM_DATA:
      {
        boost::uint32_t data2_ = ((MIDIメッセージ & 0xFF0000) >> 16);
        boost::uint32_t data1_ = ((MIDIメッセージ & 0xFF00) >> 8);
        boost::uint32_t status_ = ((MIDIメッセージ & 0xFF));

        SFTRACE((boost::wformat(_T("%2x %2x %2x \n")) % status_ % data1_ % data2_));
        switch (status_ & 0xf0)
        {
        case 0x90: // note on
          {
            SFTRACE((boost::wformat(_T("note on: %2x %2x \n")) % data1_ % data2_));
          }
          break;
        case 0x80: // note off
          {
          }
          break;
        case 0xa0: // poly after touch
          {
          }
          break;
        case 0xb0: // control change
          {
            SFTRACE((boost::wformat(_T("control change: %2x %2x \n")) % data1_ % data2_));
            //if(data1_ < 32 )
            //{
            //  m_control_change_buffer[status_ & 0xf][0] = data1_;
            //  m_control_change_buffer[status_ & 0xf][1] = data2_ << 7;

            //  input_messages::instance()->push_back
            //    (
            //    m_control_creators[data1_](current_step,cur_mac,(float)(data2_ << 7) / 16384.0f ,status_ & 0xf)
            //    );

            //} else {
            //  if(data1_ > 31 && data1_ < 64)
            //  {

            //    if(m_control_change_buffer[status_ & 0xf][0] == data1_)
            //    {
            //      data2_ += m_control_change_buffer[status_ & 0xf][1];
            //      input_messages::instance()->push_back(
            //        m_control_creators[data1_](current_step,(main_controller::instance()->current_machine()),(float)data2_ / 16384.0f,status_ & 0xf)
            //        );
            //    };
            //    m_control_change_buffer[status_ & 0xf][0] = 0;
            //    m_control_change_buffer[status_ & 0xf][1] = 0;
            //  } else {
            //    // 64 ... 119
            //    input_messages::instance()->push_back(
            //      m_control_creators[data1_](current_step,sf::model::main_controller::instance()->current_machine(),(float)(data2_) / 127.0f,status_ & 0xf)
            //      );
            //    m_control_change_buffer[status_ & 0xf][0] = 0;
            //    m_control_change_buffer[status_ & 0xf][1] = 0;
            //  }
            //}
          }
          break;
        case 0xc0: // program change
          {
          }
          break;
        case 0xd0: // channel after touch
          {
          }
          break;
        case 0xe0: // pitch bend
          {
            const boost::uint32_t value_ = data1_ << 7 | data2_;
            const float fvalue_ = value_ / 16384.0f; 
          }
          break;
        default:
          break;
        }
      }
      break;
    }
  }
private:
  boost::shared_ptr<midi入力> 入力_;
  boost::shared_ptr<midi出力> 出力_;

};
}

