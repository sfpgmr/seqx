#pragma once
//class encoder
//{
//public:
//  encoder(void);
//  virtual ~encoder(void);
//};
#include "sfmf.h"
#include "sf_memory.h"
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

namespace sf{
  namespace encoder {
    namespace msmf = boost::msm::front;

    enum struct State
    {
        Closed = 0,     // No session.
        Ready,          // Session was created, ready to open a file. 
        OpenPending,    // Session is opening a file.
        Started,        // Session is playing a file.
        Paused,         // Session is paused.
        Stopped,        // Session is stopped (ready to play). 
        Closing         // Application has closed the session, but is waiting for MESessionClosed.
    };

    // 状態クラス定義
    struct Closed : msmf::state<> {}; // セッションなし
    struct Ready : msmf::state<> {}; // セッションが作成され、ファイルを開く準備ができている。
    struct OpenPending :msmf::state<> {};// セッションはファイルをオープンしている
    struct Started : msmf::state<> {};// セッションは演奏している。
    struct Paused : msmf::state<> {};// セッションは一時停止している。
    struct Stopped : msmf::state<> {};// セッションは停止している（演奏可能状態である）。
    struct Closing : msmf::state<> {};// アプリケーションはセッションを閉じたが、MESessionClosed状態を待っている。

    namespace event 
    {
      struct Init {}; // 初期化イベント
      struct OpenURL  // ファイルを開くイベント
      {
        OpenURL() {};
        OpenURL(const OpenURL& s) : url_(s.url()) {}
        explicit OpenURL(const std::wstring& u) : url_(u) {}
        const std::wstring& url() const {return url_;}
      private:
        std::wstring url_;
      };
      struct OpenComplete {};// ファイルオープン完了
      struct Play {};// 演奏開始
      struct End {};// 終了
      struct Pause {};// 一時停止
      struct Stop {};// 停止
      struct Close {};// 閉じる
    }

    struct encoder_ 
    : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,IMFAsyncCallback>,
      public boost::msm::front::state_machine_def<sf::encoder::encoder_>
  {
    encoder_();
    typedef boost::msm::back::state_machine< encoder_ > this_type;
    typedef Microsoft::WRL::ComPtr<this_type> ptr_type;
    friend struct transition_table;
    typedef boost::signals2::signal<void( this_type &)> signal_t;

    // IMFAsyncCallback methods
    STDMETHODIMP  GetParameters(DWORD*, DWORD*)
    {
      // Implementation of this method is optional.
      return E_NOTIMPL;
    }
    STDMETHODIMP  Invoke(IMFAsyncResult* pAsyncResult);
    void       HandleEvent(UINT_PTR pUnkPtr);
    void start();
    void stop();
    void pause();
    private:

      // 外部にイベントを飛ばすためのシグナル
      signal_t OnReady_;
      signal_t OnOpenURL_;
      signal_t OnOpenComplete_;
      signal_t OnStart_;
      signal_t OnEnd_;
      signal_t OnPause_;
      signal_t OnStop_;

    public:
// 状態遷移テーブル
struct transition_table : boost::mpl::vector
  //            現在状態      ,イベント           , 次の状態      , アクション               , ガード 
  < a_row       <Closed        ,ev::Init          ,Ready          ,&:encoder_::Player_::initialize      >,
    a_row       <Ready         ,ev::OpenURL       ,OpenPending    ,&sf::player::Player_::open_url         >,
    a_row        <OpenPending   ,ev::OpenComplete  ,Stopped        ,&sf::player::Player_::open_complete>,
    a_row       <Started       ,ev::Pause         ,Paused         ,&sf::player::Player_::pause           >,
    a_row       <Started       ,ev::Stop          ,Stopped        ,&sf::player::Player_::stop            >,
    _row        <Started       ,ev::End           ,Stopped        >,
    a_row       <Paused        ,ev::Pause         ,Started        ,&sf::player::Player_::resume            >,
    a_row       <Paused        ,ev::Stop          ,Stopped        ,&sf::player::Player_::stop            >,
    a_row       <Stopped       ,ev::Play          ,Started        ,&sf::player::Player_::play            >,
    a_row       <Stopped       ,ev::OpenURL       ,OpenPending    ,&sf::player::Player_::open_url        >//,
   // a_row       <msmf::interrupt_state    ,ev::Close         ,Closed         ,&Player_::shutdown>
  >
{};
  private:
    void create_media_source();
    void configure_output(IMFStreamDescriptorPtr stream_dec,IMFTopologyPtr& topology);
    IMFMediaSessionPtr session_;
    IMFMediaSourcePtr source_;
    handle_holder close_event_;
  };

  typedef boost::msm::back::state_machine< encoder_ > encoder;
  typedef Microsoft::WRL::ComPtr<encoder> PlayerPtr;

  //
  HRESULT InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex);
  HRESULT WriteFrame(
    IMFSinkWriter *pWriter, 
    DWORD streamIndex, 
    const LONGLONG& rtStart,        // Time stamp.
    const LONGLONG& rtDuration      // Frame duration.
    );
  }
}

