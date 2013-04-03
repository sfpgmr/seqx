
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#include "stdafx.h"
#include <new>
#include <windows.h>
#include <shobjidl.h> 
#include <shlwapi.h>
#include <assert.h>
#include <strsafe.h>
#include "sfmf.h"
#include "resource.h"
#include "Player.h"
#include <assert.h>
#include <iostream>

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "sf_memory.h"

//Mf.lib Mfidl.h
namespace sf {
  namespace player {
    void print_mft();


    IMFMediaSourcePtr CreateMediaSource(const std::wstring& pszURL);

    IMFTopologyPtr CreatePlaybackTopology(IMFMediaSourcePtr pSource, 
      IMFPresentationDescriptorPtr pPD, HWND hVideoWnd);

    //  Player_オブジェクトを生成するフリー関数
    PlayerPtr CreatePlayer(
      HWND hVideo,                  // Video window.
      HWND hEvent                  // Window to receive notifications.
      )           
    {

      PlayerPtr p(new Player(hVideo, hEvent));

      if (!p)
      {
        throw win32_error_exception(E_OUTOFMEMORY);
      }
      p->process_event(ev::Init());
      print_mft();
//      p->processInitialize();
      return p;
    }

    void Player_::initialize(const ev::Init& ev)
    {
      // Start up Media Foundation platform.
      THROW_IF_ERR(MFStartup(MF_VERSION));
      m_hCloseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
      THROW_IF_ERR(HRESULT_FROM_WIN32(GetLastError()));
    }

    Player_::Player_(HWND hVideo, HWND hEvent) : 
      m_hwndVideo(hVideo),
      m_hwndEvent(hEvent),
//      m_state(Closed),
      m_hCloseEvent(NULL),
      m_nRefCount(0)
    {
    }

    Player_::~Player_()
    {
      assert(m_pSession.Get() == NULL);  
      // If FALSE, the app did not call Shutdown().

      // When Player_ calls IMediaEventGenerator::BeginGetEvent on the
      // media session, it causes the media session to hold a reference 
      // count on the Player_. 

      // This creates a circular reference count between Player_ and the 
      // media session. Calling Shutdown breaks the circular reference 
      // count.

      // If CreateInstance fails, the application will not call 
      // Shutdown. To handle that case, call Shutdown in the destructor. 

      shutdown(ev::Close());
    }

    // IUnknown methods

    HRESULT Player_::QueryInterface(REFIID riid, void** ppv)
    {
      static const QITAB qit[] = 
      {
        QITABENT(Player_, IMFAsyncCallback),
        { 0 }
      };
      return QISearch(this, qit, riid, ppv);
    }

    ULONG Player_::AddRef()
    {
      return InterlockedIncrement(&m_nRefCount);
    }

    ULONG Player_::Release()
    {
      ULONG uCount = InterlockedDecrement(&m_nRefCount);
      if (uCount == 0)
      {
        delete this;
      }
      return uCount;
    }

    //  Open a URL for playback.
    void Player_::open_url( ev::OpenURL const& openurl)
    {
      // 1. Create a new media session.
      // 2. Create the media source.
      // 3. Create the topology.
      // 4. Queue the topology [asynchronous]
      // 5. Start playback [asynchronous - does not happen in this method.]

      IMFTopologyPtr pTopology;
      IMFPresentationDescriptorPtr pSourcePD;

      // Create the media session.
      CreateSession();

      // Create the media source.
      m_pSource = CreateMediaSource(openurl.url());

      // Create the presentation descriptor for the media source.
      THROW_IF_ERR(m_pSource->CreatePresentationDescriptor(&pSourcePD));

      // Create a partial topology.
      pTopology = CreatePlaybackTopology(m_pSource, pSourcePD, m_hwndVideo);

      // Set the topology on the media session.
      THROW_IF_ERR(m_pSession->SetTopology(0, pTopology.Get()));

//      OnOpenURL_();

      // m_state = OpenPending;

      // If SetTopology succeeds, the media session will queue an 
      // MESessionTopologySet event.
    }

    //  Pause playback.
    void Player_::pause( ev::Pause const& ev)    
    {
//      if (m_state != Started)
//      {
//        throw win32_error_exception(MF_E_INVALIDREQUEST);
//      }
//
//      if (!m_pSession  || !m_pSource)
//      {
//       throw win32_error_exception(E_UNEXPECTED);
//      }
      THROW_IF_ERR(m_pSession->Pause());
      OnPause()(static_cast<this_type&>(*this));
      
      //m_state = Paused;
    }

    // Stop playback.
    void Player_::stop( ev::Stop const& ev)
    {
      //if (m_state != Started && m_state != Paused)
      //{
      //  throw win32_error_exception( MF_E_INVALIDREQUEST );
      //}
      //if (!m_pSession)
      //{
      //  throw win32_error_exception( E_UNEXPECTED);
      //}

      THROW_IF_ERR(m_pSession->Stop());
      OnStop()(static_cast<this_type&>(*this));
    }

    //  Repaint the video window. Call this method on WM_PAINT.
    void Player_::Repaint()
    {
      if (m_pVideoDisplay)
      {
        m_pVideoDisplay->RepaintVideo();
      }
    }

    //  Resize the video rectangle.
    //
    //  Call this method if the size of the video window changes.

    void Player_::ResizeVideo(WORD width, WORD height)
    {
      if (m_pVideoDisplay)
      {
        // Set the destination rectangle.
        // Leave the default source rectangle (0,0,1,1).

        RECT rcDest = { 0, 0, width, height };

        THROW_IF_ERR(m_pVideoDisplay->SetVideoPosition(NULL, &rcDest));
      }
    }

    //  Callback for the asynchronous BeginGetEvent method.

    HRESULT Player_::Invoke(IMFAsyncResult *pResult)
    {
      MediaEventType meType = MEUnknown;  // Event type

      IMFMediaEventPtr pEvent;

      try {
        // Get the event from the event queue.
        THROW_IF_ERR(m_pSession->EndGetEvent(pResult, pEvent.GetAddressOf()));

        // Get the event type. 
        THROW_IF_ERR(pEvent->GetType(&meType));

        if (meType == MESessionClosed)
        {
          // The session was closed. 
          // The application is waiting on the m_hCloseEvent event handle. 
          SetEvent(m_hCloseEvent);
        }
        else
        {
          // For all other events, get the next event in the queue.
          THROW_IF_ERR(m_pSession->BeginGetEvent(this, NULL));
        }

        // Check the application state. 

        // If a call to IMFMediaSession::Close is pending, it means the 
        // application is waiting on the m_hCloseEvent event and
        // the application's message loop is blocked. 

        // Otherwise, post a private window message to the application. 

        //if (m_state != Closing)
        //{
        //  // Leave a reference count on the event.

        //  //PostMessage(m_hwndEvent, WM_APP_PLAYER_EVENT, 
        //  //  (WPARAM) pEvent.Detach(), (LPARAM)meType);
        //}

        HandleEvent((UINT_PTR)pEvent.Detach());

        return S_OK;
      } catch (win32_error_exception& e) 
      {
        return e.hresult();
      }
    }

    void Player_::HandleEvent(UINT_PTR pEventPtr)
    {
      MediaEventType meType = MEUnknown;  

      IMFMediaEventPtr pEvent;
      //    pEvent.Attach((IMFMediaEvent*)pEventPtr);
      pEvent.Attach(reinterpret_cast<IMFMediaEvent*>(pEventPtr));

      if (!pEvent)
      {
        throw win32_error_exception(E_POINTER);
      }

      // Get the event type.
      THROW_IF_ERR(pEvent->GetType(&meType));

      // Get the event status. If the operation that triggered the event 
      // did not succeed, the status is a failure code.
      HRESULT hrStatus = S_OK;
      THROW_IF_ERR(pEvent->GetStatus(&hrStatus));
      // Check if the async operation succeeded.
      THROW_IF_ERR(hrStatus);

      switch(meType)
      {
      case MESessionTopologyStatus:
        OnTopologyStatus(pEvent);
        break;

      case MEEndOfPresentation:
        OnPresentationEnded(pEvent);
        break;

      case MENewPresentation:
        OnNewPresentation(pEvent);
        break;

      default:
        OnSessionEvent(pEvent, meType);
        break;
      }
    }

    //  Release all resources held by this object.
    void Player_::shutdown( ev::Close const& ev)
    {
      // Close the session
      CloseSession();

      // Shutdown the Media Foundation platform
      MFShutdown();

      if (m_hCloseEvent)
      {
        CloseHandle(m_hCloseEvent);
        m_hCloseEvent = NULL;
      }
    }

    /// Protected methods

    void Player_::OnTopologyStatus(IMFMediaEventPtr pEvent)
    {
      UINT32 status; 

      THROW_IF_ERR(pEvent->GetUINT32(MF_EVENT_TOPOLOGY_STATUS, &status));
      if (status == MF_TOPOSTATUS_READY)
      {
        safe_release(m_pVideoDisplay);

        // Get the IMFVideoDisplayControl interface from EVR. This call is
        // expected to fail if the media file does not have a video stream.

        (void)MFGetService(m_pSession.Get(), MR_VIDEO_RENDER_SERVICE, 
          IID_PPV_ARGS(m_pVideoDisplay.GetAddressOf()));
        static_cast<this_type&>(*this).process_event(ev::OpenComplete());
        //StartPlayback();
      }
    }


    //  Handler for MEEndOfPresentation event.
    void Player_::OnPresentationEnded(IMFMediaEventPtr pEvent)
    {
      // The session puts itself into the stopped state automatically.
      // m_state = Stopped;
      static_cast<this_type&>(*this).process_event(ev::Stop());
    }

    //  Handler for MENewPresentation event.
    //
    //  This event is sent if the media source has a new presentation, which 
    //  requires a new topology. 

    void Player_::OnNewPresentation(IMFMediaEventPtr pEvent)
    {
      IMFPresentationDescriptorPtr pPD;
      IMFTopologyPtr pTopology;

      // Get the presentation descriptor from the event.
      THROW_IF_ERR(GetEventObject(pEvent.Get(), pPD.GetAddressOf()));

      // Create a partial topology.
      pTopology = CreatePlaybackTopology(m_pSource, pPD,  m_hwndVideo);

      // Set the topology on the media session.
      THROW_IF_ERR(m_pSession->SetTopology(0, pTopology.Get()));

      // m_state = OpenPending;

    }

    //  Create a new instance of the media session.
    void Player_::CreateSession()
    {
      // Close the old session, if any.
      CloseSession();

      // assert(m_state == Closed);

      // Create the media session.
      THROW_IF_ERR(MFCreateMediaSession(NULL, &m_pSession));

      // Start pulling events from the media session
      THROW_IF_ERR(m_pSession->BeginGetEvent((IMFAsyncCallback*)this, NULL));
      // m_state = Ready;
    }

    //  Close the media session. 
    void Player_::CloseSession()
    {
      //  The IMFMediaSession::Close method is asynchronous, but the 
      //  Player_::CloseSession method waits on the MESessionClosed event.
      //  
      //  MESessionClosed is guaranteed to be the last event that the 
      //  media session fires.

      m_pVideoDisplay.Reset();

      // First close the media session.
      if (m_pSession)
      {
        DWORD dwWaitResult = 0;

        //m_state = Closing;

        THROW_IF_ERR(m_pSession->Close());
        // Wait for the close operation to complete
        dwWaitResult = WaitForSingleObject(m_hCloseEvent, 5000);
        if (dwWaitResult == WAIT_TIMEOUT)
        {
          assert(FALSE);
        }
        // Now there will be no more events from this session.
      }

      if (m_pSource)
      {
        m_pSource->Shutdown();
      }
      // Shut down the media session. (Synchronous operation, no events.)
      if (m_pSession)
      {
        m_pSession->Shutdown();
      }

      m_pSource.Reset();
      m_pSession.Reset();
      //m_state = Closed;
    }

    //  Start playback from the current position. 
    void Player_::StartPlayback()
    {
      assert(m_pSession != NULL);

      prop_variant varStart;
      //    PROPVARIANT varStart;
      //    PropVariantInit(&varStart);

      THROW_IF_ERR(m_pSession->Start(&GUID_NULL, &varStart));
      // Note: Start is an asynchronous operation. However, we
      // can treat our state as being already started. If Start
      // fails later, we'll get an MESessionStarted event with
      // an error code, and we will update our state then.
      //m_state = Started;
    }

    //  Start playback from paused or stopped.
    void Player_::play( ev::Play const& ev)
    {
 /*     if (m_state != Paused && m_state != Stopped)
      {
        THROW_IF_ERR(MF_E_INVALIDREQUEST);
      }*/
      if (!m_pSession || !m_pSource)
      {
        THROW_IF_ERR(E_UNEXPECTED);
      }
      StartPlayback();
      OnStart()(static_cast<this_type&>(*this));
    }


    //  Create a media source from a URL.
    IMFMediaSourcePtr CreateMediaSource(const std::wstring& sURL)
    {
      MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;

      IMFSourceResolverPtr pSourceResolver;
      Microsoft::WRL::ComPtr<IUnknown> pSource;
      IMFMediaSourcePtr pSourceReturn;
      // Create the source resolver.
      THROW_IF_ERR(MFCreateSourceResolver(pSourceResolver.GetAddressOf()));

      // Use the source resolver to create the media source.

      // Note: For simplicity this sample uses the synchronous method to create 
      // the media source. However, creating a media source can take a noticeable
      // amount of time, especially for a network source. For a more responsive 
      // UI, use the asynchronous BeginCreateObjectFromURL method.

      THROW_IF_ERR(pSourceResolver->CreateObjectFromURL(
        sURL.c_str(),                       // URL of the source.
        MF_RESOLUTION_MEDIASOURCE,  // Create a source object.
        NULL,                       // Optional property store.
        &ObjectType,        // Receives the created object type. 
        &pSource            // Receives a pointer to the media source.
        ));

      // Get the IMFMediaSource interface from the media source.
      THROW_IF_ERR(pSource.As<IMFMediaSource>(&pSourceReturn));
      return pSourceReturn;
    }

    //  Create an activation object for a renderer, based on the stream media type.

    IMFActivatePtr CreateMediaSinkActivate(
      IMFStreamDescriptor* pSourceSD,     // Pointer to the stream descriptor.
      HWND hVideoWindow                 // Handle to the video clipping window.
      )
    {
      IMFMediaTypeHandlerPtr pHandler;
      IMFActivatePtr pActivate;

      // Get the media type handler for the stream.
      THROW_IF_ERR(pSourceSD->GetMediaTypeHandler(pHandler.GetAddressOf()));

      // Get the major media type.
      GUID guidMajorType;
      THROW_IF_ERR(pHandler->GetMajorType(&guidMajorType));

      // Create an IMFActivate object for the renderer, based on the media type.
      if (MFMediaType_Audio == guidMajorType)
      {
        // Create the audio renderer.
        THROW_IF_ERR(MFCreateAudioRendererActivate(pActivate.GetAddressOf()));
      }
      else if (MFMediaType_Video == guidMajorType)
      {
        // Create the video renderer.
        THROW_IF_ERR(MFCreateVideoRendererActivate(hVideoWindow, pActivate.GetAddressOf()));
      }
      else
      {
        // Unknown stream type. 
        THROW_IF_ERR(E_FAIL);
        // Optionally, you could deselect this stream instead of failing.
      }

      // Return IMFActivate pointer to caller.
      return pActivate;
    }

    // Add a source node to a topology.
    IMFTopologyNodePtr AddSourceNode(
      IMFTopologyPtr pTopology,           // Topology.
      IMFMediaSourcePtr pSource,          // Media source.
      IMFPresentationDescriptorPtr pPD,   // Presentation descriptor.
      IMFStreamDescriptorPtr pSD)         // Stream descriptor.
    {
      IMFTopologyNodePtr pNode;

      // Create the node.
      THROW_IF_ERR(MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, pNode.GetAddressOf()));

      // Set the attributes.
      THROW_IF_ERR(pNode->SetUnknown(MF_TOPONODE_SOURCE, pSource.Get()));

      THROW_IF_ERR(pNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pPD.Get()));

      THROW_IF_ERR(pNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pSD.Get()));

      // Add the node to the topology.
      THROW_IF_ERR(pTopology->AddNode(pNode.Get()));

      // Return the pointer to the caller.
      return pNode;
    }

    // Add an output node to a topology.
    IMFTopologyNodePtr AddOutputNode(
      IMFTopologyPtr pTopology,     // Topology.
      IMFActivatePtr pActivate,     // Media sink activation object.
      DWORD dwId                 // Identifier of the stream sink.
      ) 
    {
      IMFTopologyNodePtr pNode;

      // Create the node.
      THROW_IF_ERR(MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pNode));

      // Set the object pointer.
      THROW_IF_ERR(pNode->SetObject(pActivate.Get()));

      // Set the stream sink ID attribute.
      THROW_IF_ERR(pNode->SetUINT32(MF_TOPONODE_STREAMID, dwId));

      THROW_IF_ERR(pNode->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE));

      // Add the node to the topology.
      THROW_IF_ERR(pTopology->AddNode(pNode.Get()));

      return pNode;

    }
    //</SnippetPlayer.cpp>

    //  Add a topology branch for one stream.
    //
    //  For each stream, this function does the following:
    //
    //    1. Creates a source node associated with the stream. 
    //    2. Creates an output node for the renderer. 
    //    3. Connects the two nodes.
    //
    //  The media session will add any decoders that are needed.

    void AddBranchToPartialTopology(
      IMFTopologyPtr pTopology,         // Topology.
      IMFMediaSourcePtr pSource,        // Media source.
      IMFPresentationDescriptorPtr pPD, // Presentation descriptor.
      DWORD iStream,                  // Stream index.
      HWND hVideoWnd)                 // Window for video playback.
    {
      IMFStreamDescriptorPtr pSD;
      IMFActivatePtr         pSinkActivate;
      IMFTopologyNodePtr     pSourceNode;
      IMFTopologyNodePtr     pOutputNode;

      BOOL fSelected = FALSE;

      THROW_IF_ERR(pPD->GetStreamDescriptorByIndex(iStream, &fSelected, &pSD));

      if (fSelected)
      {
        // Create the media sink activation object.
        pSinkActivate = CreateMediaSinkActivate(pSD.Get(), hVideoWnd);

        // Add a source node for this stream.
        pSourceNode = AddSourceNode(pTopology, pSource, pPD, pSD);

        // Create the output node for the renderer.
        pOutputNode = AddOutputNode(pTopology, pSinkActivate, 0);

        // Connect the source node to the output node.
        THROW_IF_ERR(pSourceNode->ConnectOutput(0, pOutputNode.Get(), 0));
      }
      // else: If not selected, don't add the branch. 
    }

    //  Create a playback topology from a media source.
    // Receives a pointer to the topology.
    IMFTopologyPtr CreatePlaybackTopology(
      IMFMediaSourcePtr pSource,          // Media source.
      IMFPresentationDescriptorPtr pPD,   // Presentation descriptor.
      HWND hVideoWnd                   // Video window.
      )        
    {
      IMFTopologyPtr pTopology;
      DWORD cSourceStreams = 0;

      // Create a new topology.
      THROW_IF_ERR(MFCreateTopology(&pTopology));

      // Get the number of streams in the media source.
      THROW_IF_ERR(pPD->GetStreamDescriptorCount(&cSourceStreams));

      // For each stream, create the topology nodes and add them to the topology.
      for (DWORD i = 0; i < cSourceStreams; i++)
      {
        AddBranchToPartialTopology(pTopology, pSource, pPD, i, hVideoWnd);
      }

      // Return the IMFTopology pointer to the caller.
      return pTopology;
    }

  void print_mft_(const GUID& guid,std::wfstream& out)
  {
    co_task_memory<IMFActivate*> activate;

    UINT32 count = 0;
    
    HRESULT hr = MFTEnumEx(guid,MFT_ENUM_FLAG_ALL,NULL,NULL,&activate,&count);
    
    if(SUCCEEDED(hr) && count > 0)
    {
      for(int i = 0; i < count;++i)
      {
        UINT32 l = 0;
        UINT32 l1 = 0;
        activate.get()[i]->GetStringLength(MFT_FRIENDLY_NAME_Attribute,&l);
        std::unique_ptr<wchar_t[]> name(new wchar_t[l+1]);
        memset(name.get(),0,l+1);
        hr = activate.get()[i]->GetString(MFT_FRIENDLY_NAME_Attribute,name.get(),l+1,&l1);
        out << name.get() << std::endl;
        activate.get()[i]->Release();
      }
      //CoTaskMemFree(activate);
    }
  }

  void print_mft()
  {
    std::wfstream out(L"MFT.txt",std::ios_base::out | std::ios_base::trunc);

    out << std::endl << "**" << L"MFT_CATEGORY_AUDIO_DECODER" << L"**" << std::endl << std::endl;

    print_mft_(MFT_CATEGORY_AUDIO_DECODER,out);

    out << std::endl << L"**" << L"MFT_CATEGORY_AUDIO_EFFECT" << L"**" << std::endl << std::endl;
    print_mft_(MFT_CATEGORY_AUDIO_EFFECT,out);

    out << std::endl << L"**" << L"MFT_CATEGORY_AUDIO_ENCODER" << L"**" << std::endl << std::endl;
    print_mft_(MFT_CATEGORY_AUDIO_ENCODER,out);

    out << std::endl << L"**" << L"MFT_CATEGORY_DEMULTIPLEXER" << L"**" << std::endl << std::endl;
    print_mft_(MFT_CATEGORY_DEMULTIPLEXER,out);

    out << std::endl << L"**" << L"MFT_CATEGORY_MULTIPLEXER" << L"**" << std::endl << std::endl;
    print_mft_(MFT_CATEGORY_MULTIPLEXER,out);

    out << std::endl << L"**" << L"MFT_CATEGORY_OTHER" << L"**" << std::endl << std::endl;
    print_mft_(MFT_CATEGORY_OTHER,out);

    out << std::endl << L"**" << L"MFT_CATEGORY_VIDEO_DECODER" << L"**" << std::endl << std::endl;
    print_mft_(MFT_CATEGORY_VIDEO_DECODER,out);

    out << std::endl << L"**" << L"MFT_CATEGORY_VIDEO_EFFECT" << L"**" << std::endl << std::endl;
    print_mft_(MFT_CATEGORY_VIDEO_EFFECT,out);

    out << std::endl << L"**" << L"MFT_CATEGORY_VIDEO_ENCODER" << L"**" << std::endl << std::endl;
    print_mft_(MFT_CATEGORY_VIDEO_ENCODER,out);

    out << std::endl << L"**" << L"MFT_CATEGORY_VIDEO_PROCESSOR" << L"**" << std::endl << std::endl;
    print_mft_(MFT_CATEGORY_VIDEO_PROCESSOR,out);

    out.close();
  }
}

}