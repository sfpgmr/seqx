

#ifndef PLAYER_H
#define PLAYER_H

#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

namespace sf {
  namespace player {
    namespace msmf = boost::msm::front;

    // const UINT WM_APP_PLAYER_EVENT = WM_APP + 1;   
    // WPARAM = IMFMediaEvent*, WPARAM = MediaEventType

    enum struct PlayerState
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

    namespace ev 
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

    struct Player_ : public IMFAsyncCallback,public boost::msm::front::state_machine_def<sf::player::Player_>
    {
      typedef boost::msm::back::state_machine< Player_ > this_type;
      typedef Microsoft::WRL::ComPtr<this_type> ptr_type;
      friend ptr_type CreatePlayer(HWND hVideo, HWND hEvent);
      friend struct transition_table;
      typedef boost::signals2::signal<void( this_type &)> signal_t;


      // IUnknown methods
      STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
      STDMETHODIMP_(ULONG) AddRef();
      STDMETHODIMP_(ULONG) Release();

      // IMFAsyncCallback methods
      STDMETHODIMP  GetParameters(DWORD*, DWORD*)
      {
        // Implementation of this method is optional.
        return E_NOTIMPL;
      }
      STDMETHODIMP  Invoke(IMFAsyncResult* pAsyncResult);

      void       HandleEvent(UINT_PTR pUnkPtr);
 //     PlayerState   GetState() const { return m_state; }

      // Video functionality
      void       Repaint();
      void       ResizeVideo(WORD width, WORD height);

      BOOL          HasVideo() const { return (m_pVideoDisplay != NULL);  }

      signal_t& OnReady(){return OnReady_;}
      signal_t& OnOpenURL() {return OnOpenURL_;}
      signal_t& OnStart(){return OnStart_;}
      signal_t& OnEnd(){return OnEnd_;}
      signal_t& OnPause(){return OnPause_;}
      signal_t& OnStop(){return OnStop_;}
      signal_t& OnOpenComplete(){return OnOpenComplete_;}

      void OpenComplete()
      {
        OnOpenComplete_(static_cast<this_type&>(*this));
      }


    protected:


      // Constructor is private. Use static CreateInstance method to instantiate.
      Player_(HWND hVideo, HWND hEvent);

      // Destructor is private. Caller should call Release.
      virtual ~Player_(); 

      void CreateSession();
      void CloseSession();
      void StartPlayback();

      // Playback
      void initialize( ev::Init const& ev);
      void open_url( ev::OpenURL const& openurl);
      void play( ev::Play const& ev);
      void resume( ev::Pause const& ev){play(ev::Play());};
      void pause( ev::Pause const& ev);
      void stop( ev::Stop const& ev);
      void shutdown( ev::Close const& ev);
      void open_complete(ev::OpenComplete const&)
      {
        OnOpenComplete()(static_cast<this_type&>(*this));
      }

      // Media event handlers
      virtual void OnTopologyStatus(IMFMediaEventPtr pEvent);
      virtual void OnPresentationEnded(IMFMediaEventPtr pEvent);
      virtual void OnNewPresentation(IMFMediaEventPtr pEvent);

      // Override to handle additional session events.
      virtual void OnSessionEvent(IMFMediaEventPtr, MediaEventType) 
      { 
        //return S_OK; 
      }


      long                    m_nRefCount;        // Reference count.

      IMFMediaSessionPtr         m_pSession;
      IMFMediaSourcePtr          m_pSource;
      IMFVideoDisplayControlPtr  m_pVideoDisplay;

      HWND                    m_hwndVideo;        // Video window.
      HWND                    m_hwndEvent;        // App window to receive events.
     // PlayerState             m_state;            // Current state of the media session.
      HANDLE                  m_hCloseEvent;      // Event to wait on while closing.

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
  < a_row       <Closed        ,ev::Init          ,Ready          ,&sf::player::Player_::initialize      >,
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
      typedef Closed initial_state;


      template <class FSM,class Event>
      void no_transition(Event const& e, FSM&,int state)
      {
        throw exception(L"No Transition");
      }

      template <class FSM>
      void no_transition(ev::Close const& e, FSM&,int state)
      {
        shutdown(ev::Close());
      }

    };

    typedef boost::msm::back::state_machine< Player_ > Player;

    typedef Microsoft::WRL::ComPtr<Player> PlayerPtr;
    PlayerPtr CreatePlayer(HWND hVideo, HWND hEvent);

  }
}
#endif PLAYER_H
