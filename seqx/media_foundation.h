#pragma once
#include <new>
#include <windows.h>
#include <shobjidl.h> 
#include <shlwapi.h>
#include <assert.h>
#include <strsafe.h>

#include "sfmf.h"
#include "sf_memory.h"
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

namespace sf{
  namespace media {
    namespace msmf = boost::msm::front;

    enum struct State
    {
      closed = 0,     // No session.
      ready,          // Session was created, ready to open a file. 
      open_pending,    // Session is opening a file.
      started,        // Session is playing a file.
      paused,         // Session is paused.
      stopped,        // Session is stopped (ready to play). 
      closing         // Application has closed the session, but is waiting for MESessionClosed.
    };

    // 状態クラス定義
    struct closed : msmf::state<> {}; // セッションなし
    struct ready : msmf::state<> {}; // セッションが作成され、ファイルを開く準備ができている。
    struct open_pending :msmf::state<> {};// セッションはファイルをオープンしている
    struct started : msmf::state<> {};// セッションは演奏している。
    struct paused : msmf::state<> {};// セッションは一時停止している。
    struct stopped : msmf::state<> {};// セッションは停止している（演奏可能状態である）。
    struct closing : msmf::state<> {};// アプリケーションはセッションを閉じたが、MESessionClosed状態を待っている。

    namespace ev 
    {
      struct init {}; // 初期化イベント
      struct open_url  // ファイルを開くイベント
      {
        open_url() {};
        open_url(const open_url& s) : url_(s.url()) {}
        explicit open_url(const std::wstring& u) : url_(u) {}
        const std::wstring& url() const {return url_;}
      private:
        std::wstring url_;
      };
      struct open_complete {};// ファイルオープン完了
      struct start {};// 演奏開始
      struct end {};// 終了
      struct pause {};// 一時停止
      struct stop {};// 停止
      struct close {};// 閉じる
    }

    // Media Foundationのスタートアップとシャットダウン
    struct start_up
    {
      start_up() 
      {
        // スタートアップ
        res_ = MFStartup(MF_VERSION);
        THROW_IF_ERR(res_);
      }

      ~start_up()
      {
        // シャットダウン
        if(SUCCEEDED(res_)){
          THROW_IF_ERR(MFShutdown());
        }
        
      }
    private:
      HRESULT res_;
    };

    IMFTopologyPtr create_topology();
    IMFTopologyPtr create_topology(std::wstring& url);
    

    // Media Session ラッパー
    struct session_ 
      : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,IMFAsyncCallback>,
      public boost::msm::front::state_machine_def<session_>
    {
      typedef boost::msm::back::state_machine< session_ > this_type;
      typedef Microsoft::WRL::ComPtr<this_type> ptr_type;
      friend struct transition_table;
      typedef boost::signals2::signal<void( this_type &)> signal_t;

      session_();
      virtual ~session_();

      // IMFAsyncCallback methods
      STDMETHODIMP  GetParameters(DWORD*, DWORD*)
      {
        // Implementation of this method is optional.
        return E_NOTIMPL;
      }
      STDMETHODIMP  Invoke(IMFAsyncResult* pAsyncResult);
//      void HandleEvent(UINT_PTR pUnkPtr);
      
      signal_t& on_ready(){return on_ready_;}
      signal_t& on_open_url() {return on_open_url_;}
      signal_t& on_start(){return on_start_;}
      signal_t& on_end(){return on_end_;}
      signal_t& on_pause(){return on_pause_;}
      signal_t& on_stop(){return on_stop_;}
      signal_t& on_open_complete(){return on_open_complete_;}
      void open_complete()
      {
        on_open_complete_(static_cast<this_type&>(*this));
      }

    protected:

      void CreateSession();
      void CloseSession();
      void StartPlayback();

      // Playback
      void initialize( ev::init const& ev);
      void open_url( ev::open_url const& openurl);
      void start( ev::start const& ev);
      void resume( ev::pause const& ev){start(ev::start());};
      void pause( ev::pause const& ev);
      void stop( ev::stop const& ev);
      void shutdown( ev::close const& ev);
      void open_complete(ev::open_complete const&)
      {
        on_open_complete()(static_cast<this_type&>(*this));
      }

      // Media event handlers
      virtual void OnTopologyStatus(IMFMediaEventPtr pEvent){};
      virtual void OnPresentation_ended(IMFMediaEventPtr pEvent){};
      virtual void OnNewPresentation(IMFMediaEventPtr pEvent){};

      // Override to handle additional session events.
      virtual void OnSessionEvent(IMFMediaEventPtr, MediaEventType) 
      { 
        //return S_OK; 
      }
      void HandleEvent(UINT_PTR pEventPtr);
    private:

      // 外部にイベントを飛ばすためのシグナル
      signal_t on_ready_;
      signal_t on_open_url_;
      signal_t on_open_complete_;
      signal_t on_start_;
      signal_t on_end_;
      signal_t on_pause_;
      signal_t on_stop_;

    public:
      // 状態遷移テーブル
      struct transition_table : boost::mpl::vector
        //            現在状態     ,イベント           , 次の状態      , アクション               , ガード 
        < a_row     <closed        ,ev::init          ,ready          ,&session_::initialize      >,
        a_row       <ready         ,ev::open_url      ,open_pending    ,&session_::open_url         >,
        a_row       <open_pending  ,ev::open_complete ,stopped        ,&session_::open_complete>,
        a_row       <started       ,ev::pause         ,paused         ,&session_::pause           >,
        a_row       <started       ,ev::stop          ,stopped        ,&session_::stop            >,
        _row        <started       ,ev::end           ,stopped        >,
        a_row       <paused        ,ev::pause         ,started        ,&session_::resume            >,
        a_row       <paused        ,ev::stop          ,stopped        ,&session_::stop            >,
        a_row       <stopped       ,ev::start         ,started        ,&session_::start            >,
        a_row       <stopped       ,ev::open_url     ,open_pending    ,&session_::open_url        >//,
        // a_row       <msmf::interrupt_state    ,ev::Close         ,Closed         ,&Player_::shutdown>
        >
      {};
      typedef closed initial_state;
      IMFMediaSessionPtr media_session()
      {
        assert(media_session_);
        return media_session_;
      };
    private:
//      void create_media_source();
      void create_session();
      void close_session();
//      void configure_output(IMFStreamDescriptorPtr stream_dec,IMFTopologyPtr& topology);
      IMFMediaSessionPtr media_session_;
//      IMFMediaSourcePtr media_source_;
      handle_holder close_event_;
    };

    typedef boost::msm::back::state_machine< session_ > session;
    typedef Microsoft::WRL::ComPtr<session> session_ptr;

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

