#pragma once
/*
*/
// Windows Header Files:
#define XBYAK64
#include "exception.h"
#include "base_window.h"
#include "dpi.h"
#include "xbyak.h"
#include "windows.h"
#include "windowsx.h"
#include "CommCtrl.h"
#include <type_traits>
#include "timer.h"
//#include <boost/type_traits/is_same.hpp>

//#include "input.h"


// Direct Write

_WRL_PTR_TYPEDEF(IDWriteFactory);
_WRL_PTR_TYPEDEF(IDWriteFactory1);
_WRL_PTR_TYPEDEF(IDWriteGdiInterop);
_WRL_PTR_TYPEDEF(IDWriteFontFace1);
_WRL_PTR_TYPEDEF(IDWriteFontFace);
_WRL_PTR_TYPEDEF(IDWriteFont1);
_WRL_PTR_TYPEDEF(IDWriteFont);
_WRL_PTR_TYPEDEF(IDWriteFontFamily);
_WRL_PTR_TYPEDEF(IDWriteFontCollection);
_WRL_PTR_TYPEDEF(IDWriteLocalizedStrings);
_WRL_PTR_TYPEDEF(IDWriteTextFormat);
_WRL_PTR_TYPEDEF(IDWriteTextAnalyzer1);
_WRL_PTR_TYPEDEF(IDWriteTextLayout);
_WRL_PTR_TYPEDEF(IDWriteTextLayout1);
_WRL_PTR_TYPEDEF(IDWriteFontFile);
//_WRL_PTR_TYPEDEF(IDWriteFontFile);

// Direct2D

_WRL_PTR_TYPEDEF(ID2D1AnalysisTransform);
_WRL_PTR_TYPEDEF(ID2D1Bitmap);
_WRL_PTR_TYPEDEF(ID2D1Bitmap1);
_WRL_PTR_TYPEDEF(ID2D1BitmapBrush);
_WRL_PTR_TYPEDEF(ID2D1BitmapBrush1);
_WRL_PTR_TYPEDEF(ID2D1BitmapRenderTarget);
_WRL_PTR_TYPEDEF(ID2D1BlendTransform);
_WRL_PTR_TYPEDEF(ID2D1BorderTransform);
_WRL_PTR_TYPEDEF(ID2D1BoundsAdjustmentTransform);
_WRL_PTR_TYPEDEF(ID2D1Brush);
_WRL_PTR_TYPEDEF(ID2D1ColorContext);
_WRL_PTR_TYPEDEF(ID2D1CommandList);
_WRL_PTR_TYPEDEF(ID2D1CommandSink);
_WRL_PTR_TYPEDEF(ID2D1ComputeInfo);
_WRL_PTR_TYPEDEF(ID2D1ComputeTransform);
_WRL_PTR_TYPEDEF(ID2D1ConcreteTransform);
_WRL_PTR_TYPEDEF(ID2D1DCRenderTarget);
_WRL_PTR_TYPEDEF(ID2D1Device);
_WRL_PTR_TYPEDEF(ID2D1DeviceContext);
_WRL_PTR_TYPEDEF(ID2D1DrawInfo);
_WRL_PTR_TYPEDEF(ID2D1DrawingStateBlock);
_WRL_PTR_TYPEDEF(ID2D1DrawingStateBlock1);
_WRL_PTR_TYPEDEF(ID2D1DrawTransform);
_WRL_PTR_TYPEDEF(ID2D1Effect);
_WRL_PTR_TYPEDEF(ID2D1EffectContext);
_WRL_PTR_TYPEDEF(ID2D1EffectImpl);
_WRL_PTR_TYPEDEF(ID2D1EllipseGeometry);
_WRL_PTR_TYPEDEF(ID2D1Factory);
_WRL_PTR_TYPEDEF(ID2D1Factory1);
_WRL_PTR_TYPEDEF(ID2D1GdiInteropRenderTarget);
_WRL_PTR_TYPEDEF(ID2D1GdiMetafile);
_WRL_PTR_TYPEDEF(ID2D1GdiMetafileSink);
_WRL_PTR_TYPEDEF(ID2D1Geometry);
_WRL_PTR_TYPEDEF(ID2D1GeometryGroup);
_WRL_PTR_TYPEDEF(ID2D1GeometrySink);
_WRL_PTR_TYPEDEF(ID2D1GradientStopCollection);
_WRL_PTR_TYPEDEF(ID2D1GradientStopCollection1);
_WRL_PTR_TYPEDEF(ID2D1HwndRenderTarget);
_WRL_PTR_TYPEDEF(ID2D1Image);
_WRL_PTR_TYPEDEF(ID2D1ImageBrush);
_WRL_PTR_TYPEDEF(ID2D1Layer);
_WRL_PTR_TYPEDEF(ID2D1LinearGradientBrush);
_WRL_PTR_TYPEDEF(ID2D1Mesh);
//_WRL_PTR_TYPEDEF(ID2D1MultiThread);
_WRL_PTR_TYPEDEF(ID2D1OffsetTransform);
_WRL_PTR_TYPEDEF(ID2D1PathGeometry);
_WRL_PTR_TYPEDEF(ID2D1PathGeometry1);
_WRL_PTR_TYPEDEF(ID2D1PrintControl);
_WRL_PTR_TYPEDEF(ID2D1Properties);
_WRL_PTR_TYPEDEF(ID2D1RadialGradientBrush);
_WRL_PTR_TYPEDEF(ID2D1RectangleGeometry);
_WRL_PTR_TYPEDEF(ID2D1RenderInfo);
_WRL_PTR_TYPEDEF(ID2D1RenderTarget);
_WRL_PTR_TYPEDEF(ID2D1Resource);
_WRL_PTR_TYPEDEF(ID2D1ResourceTexture);
_WRL_PTR_TYPEDEF(ID2D1RoundedRectangleGeometry);
_WRL_PTR_TYPEDEF(ID2D1SimplifiedGeometrySink);
_WRL_PTR_TYPEDEF(ID2D1SolidColorBrush);
_WRL_PTR_TYPEDEF(ID2D1SourceTransform);
_WRL_PTR_TYPEDEF(ID2D1StrokeStyle);
_WRL_PTR_TYPEDEF(ID2D1StrokeStyle1);
_WRL_PTR_TYPEDEF(ID2D1TessellationSink);
_WRL_PTR_TYPEDEF(ID2D1Transform);
_WRL_PTR_TYPEDEF(ID2D1TransformedGeometry);
_WRL_PTR_TYPEDEF(ID2D1TransformGraph);
_WRL_PTR_TYPEDEF(ID2D1TransformNode);
_WRL_PTR_TYPEDEF(ID2D1VertexBuffer);

// WIC

_WRL_PTR_TYPEDEF(IWICImagingFactory);
_WRL_PTR_TYPEDEF(IWICBitmapDecoder);
_WRL_PTR_TYPEDEF(IWICBitmapFrameDecode);
_WRL_PTR_TYPEDEF(IWICStream);
_WRL_PTR_TYPEDEF(IWICFormatConverter);
_WRL_PTR_TYPEDEF(IWICBitmapScaler);
_WRL_PTR_TYPEDEF(ITaskbarList3);

// DXGI 

_WRL_PTR_TYPEDEF(IDXGIAdapter);
_WRL_PTR_TYPEDEF(IDXGIAdapter1);
_WRL_PTR_TYPEDEF(IDXGIAdapter2);
_WRL_PTR_TYPEDEF(IDXGIDebug);
_WRL_PTR_TYPEDEF(IDXGIDevice);
_WRL_PTR_TYPEDEF(IDXGIDevice1);
_WRL_PTR_TYPEDEF(IDXGIDevice2);
_WRL_PTR_TYPEDEF(IDXGIDeviceSubObject);
_WRL_PTR_TYPEDEF(IDXGIDisplayControl);
_WRL_PTR_TYPEDEF(IDXGIFactory);
_WRL_PTR_TYPEDEF(IDXGIFactory1);
_WRL_PTR_TYPEDEF(IDXGIFactory2);
_WRL_PTR_TYPEDEF(IDXGIInfoQueue);
_WRL_PTR_TYPEDEF(IDXGIKeyedMutex);
_WRL_PTR_TYPEDEF(IDXGIObject);
_WRL_PTR_TYPEDEF(IDXGIOutput);
_WRL_PTR_TYPEDEF(IDXGIOutput1);
_WRL_PTR_TYPEDEF(IDXGIOutputDuplication);
_WRL_PTR_TYPEDEF(IDXGIResource);
_WRL_PTR_TYPEDEF(IDXGIResource1);
_WRL_PTR_TYPEDEF(IDXGISurface);
_WRL_PTR_TYPEDEF(IDXGISurface1);
_WRL_PTR_TYPEDEF(IDXGISurface2);
_WRL_PTR_TYPEDEF(IDXGISwapChain);
_WRL_PTR_TYPEDEF(IDXGISwapChain1);

// Direct3D

_WRL_PTR_TYPEDEF(ID3D11Asynchronous);
_WRL_PTR_TYPEDEF(ID3D11BlendState);
_WRL_PTR_TYPEDEF(ID3D11BlendState1);
_WRL_PTR_TYPEDEF(ID3D11CommandList);
_WRL_PTR_TYPEDEF(ID3D11Counter);
_WRL_PTR_TYPEDEF(ID3D11DepthStencilState);
_WRL_PTR_TYPEDEF(ID3D11Device);
_WRL_PTR_TYPEDEF(ID3D11Device1);
_WRL_PTR_TYPEDEF(ID3D11DeviceChild);
_WRL_PTR_TYPEDEF(ID3D11DeviceContext);
_WRL_PTR_TYPEDEF(ID3D11DeviceContext1);
_WRL_PTR_TYPEDEF(ID3DDeviceContextState);
_WRL_PTR_TYPEDEF(ID3D11InputLayout);
_WRL_PTR_TYPEDEF(ID3D11Predicate);
_WRL_PTR_TYPEDEF(ID3D11Query);
_WRL_PTR_TYPEDEF(ID3D11RasterizerState);
_WRL_PTR_TYPEDEF(ID3D11RasterizerState1);
_WRL_PTR_TYPEDEF(ID3D11SamplerState);
_WRL_PTR_TYPEDEF(ID3D11Debug);
_WRL_PTR_TYPEDEF(ID3D11InfoQueue);
_WRL_PTR_TYPEDEF(ID3D11RefDefaultTrackingOptions);
_WRL_PTR_TYPEDEF(ID3D11RefTrackingOptions);
_WRL_PTR_TYPEDEF(ID3D11SwitchToRef);
_WRL_PTR_TYPEDEF(ID3D11TracingDevice);
_WRL_PTR_TYPEDEF(ID3D11Buffer);
_WRL_PTR_TYPEDEF(ID3D11DepthStencilView);
_WRL_PTR_TYPEDEF(ID3D11RenderTargetView);
_WRL_PTR_TYPEDEF(ID3D11Resource);
_WRL_PTR_TYPEDEF(ID3D11ShaderResourceView);
_WRL_PTR_TYPEDEF(ID3D11Texture1D);
_WRL_PTR_TYPEDEF(ID3D11Texture2D);
_WRL_PTR_TYPEDEF(ID3D11Texture3D);
_WRL_PTR_TYPEDEF(ID3D11UnorderedAccessView);
_WRL_PTR_TYPEDEF(ID3D11View);
_WRL_PTR_TYPEDEF(ID3D11ClassInstance);
_WRL_PTR_TYPEDEF(ID3D11ClassLinkage);
_WRL_PTR_TYPEDEF(ID3D11ComputeShader);
_WRL_PTR_TYPEDEF(ID3D11DomainShader);
_WRL_PTR_TYPEDEF(ID3D11GeometryShader);
_WRL_PTR_TYPEDEF(ID3D11HullShader);
_WRL_PTR_TYPEDEF(ID3D11PixelShader);
_WRL_PTR_TYPEDEF(ID3D11ShaderReflection);
_WRL_PTR_TYPEDEF(ID3D11ShaderReflectionConstantBuffer);
_WRL_PTR_TYPEDEF(ID3D11ShaderReflectionType);
_WRL_PTR_TYPEDEF(ID3D11ShaderReflectionVariable);
_WRL_PTR_TYPEDEF(ID3D11ShaderTrace);
_WRL_PTR_TYPEDEF(ID3D11ShaderTraceFactory);
_WRL_PTR_TYPEDEF(ID3D11VertexShader);
_WRL_PTR_TYPEDEF(ID3DBlob);
_WRL_PTR_TYPEDEF(ID3DInclude);
_WRL_PTR_TYPEDEF(ID3DUserDefinedAnnotation);

// DirectComposition

_WRL_PTR_TYPEDEF(IDCompositionAnimation);
_WRL_PTR_TYPEDEF(IDCompositionClip);
_WRL_PTR_TYPEDEF(IDCompositionDevice);
_WRL_PTR_TYPEDEF(IDCompositionEffect);
_WRL_PTR_TYPEDEF(IDCompositionEffectGroup);
_WRL_PTR_TYPEDEF(IDCompositionMatrixTransform);
_WRL_PTR_TYPEDEF(IDCompositionMatrixTransform3D);
_WRL_PTR_TYPEDEF(IDCompositionRectangleClip);
_WRL_PTR_TYPEDEF(IDCompositionRotateTransform);
_WRL_PTR_TYPEDEF(IDCompositionRotateTransform3D);
_WRL_PTR_TYPEDEF(IDCompositionScaleTransform);
_WRL_PTR_TYPEDEF(IDCompositionScaleTransform3D);
_WRL_PTR_TYPEDEF(IDCompositionSkewTransform);
_WRL_PTR_TYPEDEF(IDCompositionSurface);
_WRL_PTR_TYPEDEF(IDCompositionTarget);
_WRL_PTR_TYPEDEF(IDCompositionTransform);
_WRL_PTR_TYPEDEF(IDCompositionTransform3D);
_WRL_PTR_TYPEDEF(IDCompositionTranslateTransform);
_WRL_PTR_TYPEDEF(IDCompositionTranslateTransform3D);
_WRL_PTR_TYPEDEF(IDCompositionVirtualSurface);
_WRL_PTR_TYPEDEF(IDCompositionVisual);


namespace sf{

  /* inline template <class Exc = win32_error_exception> void throw_if_err<>()(HRESULT hr)
  {
  if(hr != S_OK){throw Exc(hr);}
  };*/


  ID2D1BitmapPtr load_bitmap_from_file(
    ID2D1HwndRenderTargetPtr render_target,
    IWICImagingFactoryPtr wic_factory,
    std::wstring uri,
    uint32_t destination_width = 0,
    uint32_t destination_height = 0
    );

  /** WNDCLASSEXラッパクラス */
  struct window_class_ex
  {
    window_class_ex(
      const wchar_t*  menu_name ,
      const std::wstring&  class_name ,
      HINSTANCE   hInstance = NULL,
      WNDPROC     lpfnWndProc = ::DefWindowProcW,
      uint32_t        style = CS_HREDRAW | CS_VREDRAW,
      int32_t     cbClsExtra  = 0,
      int32_t     cbWndExtra = sizeof(LONG_PTR), 
      HICON       hIcon = ::LoadIcon(NULL,IDI_APPLICATION),
      HCURSOR     hCursor = ::LoadCursor(NULL, IDC_ARROW),
      HBRUSH      hbrBackground = ::CreateSolidBrush(0xff000000),
      HICON       hIconSm = NULL
      ) : is_register_(false)
    {

      if(::GetClassInfoExW(hInstance,class_name.c_str(),&wndclass_) == 0)
      {
        if(::GetLastError() == ERROR_CLASS_DOES_NOT_EXIST)
        { 
          ::ZeroMemory(&wndclass_,sizeof(wndclass_));
          wndclass_.lpszMenuName = (LPCWSTR)menu_name;
          wndclass_.lpszClassName = class_name.c_str();
          wndclass_.cbSize = sizeof(::WNDCLASSEXW);
          wndclass_.cbWndExtra = cbWndExtra;
          wndclass_.hInstance = hInstance;
          wndclass_.lpfnWndProc = lpfnWndProc;
          wndclass_.style = style;
          wndclass_.cbClsExtra = cbClsExtra;
          wndclass_.hIcon = hIcon;
          wndclass_.hCursor = hCursor;
          wndclass_.hbrBackground = hbrBackground;
          wndclass_.hIconSm = hIconSm;
          atom_ = ::RegisterClassExW(&wndclass_) ;
          BOOST_ASSERT(atom_ != 0);
          is_register_ = true;
        } else {
          throw win32_error_exception();
        }
      } else {
        is_register_ = false;
      }
    };

    ~window_class_ex()
    {
      if(is_register_){
        ::UnregisterClassW(wndclass_.lpszClassName,wndclass_.hInstance);
      }
    }

  private:
    bool is_register_;
    ATOM atom_;
    ::WNDCLASSEXW wndclass_;
  };

  struct get_dc {
    get_dc(HWND hwnd) : hwnd_(hwnd),hdc_(GetDC(hwnd)) {}
    HDC get(){return hdc_;}
    ~get_dc(){::ReleaseDC(hwnd_,hdc_);}
  private:
    HDC hdc_;
    HWND hwnd_;
  };

  struct get_window_dc {
    get_window_dc(HWND hwnd) : hwnd_(hwnd),hdc_(GetWindowDC(hwnd)) {}
    HDC get(){return hdc_;}
    ~get_window_dc(){::ReleaseDC(hwnd_,hdc_);}
  private:
    HDC hdc_;
    HWND hwnd_;
  };

  struct compatible_dc {
    compatible_dc(HDC hdc) : hdc_(::CreateCompatibleDC(hdc)){}; 
    ~compatible_dc(){::DeleteDC(hdc_);};
    HDC get() { return hdc_;};
  private:
    HDC hdc_;
  };

  struct ref_dc {
    ref_dc(HDC& hdc) : hdc_(hdc) {};
    ~ref_dc(){};
    HDC get() { return hdc_;};
  private:
    HDC& hdc_;
  };

  struct d2_dc {
    d2_dc(ID2D1GdiInteropRenderTargetPtr& ptr,D2D1_DC_INITIALIZE_MODE mode) :hdc_(0),ptr_(ptr)
    {
      hr_ = ptr->GetDC(mode,&hdc_);
    };
    ~d2_dc(){ptr_->ReleaseDC(NULL);};
    HDC get() { return hdc_;};
  private:
    HRESULT hr_;
    HDC hdc_;
    ID2D1GdiInteropRenderTargetPtr& ptr_;
  };

  template <typename Holder>
  struct device_context
  {
    explicit device_context(Holder* holder) : holder_(holder){};
    ~device_context() {}
    operator HDC(){return holder_->get();}
  private:
    std::unique_ptr<Holder> holder_;
  };

  //struct handle_holder : boost::noncopyable
  //{
  //  explicit handle_holder(HANDLE handle) : handle_(handle) {};
  //  ~handle_holder(){if(handle_) ::CloseHandle(handle_);}
  //  operator HANDLE(){return handle_;}
  //private:
  //  HANDLE handle_;
  //};


  struct HBITMAP_deleter {
    typedef HBITMAP pointer;
    void operator ()(HBITMAP handle) {
      if (handle) {
        ::DeleteObject(handle);
      }
    }
  };

  //template <typename Handle,typename Handle_Deleter>
  //struct handle_holder {
  //  typedef boost::unique_ptr<Handle,Handle_Deleter> holder_type;
  //  handle_holder(Handle handle) : holder_(handle) {}
  //  operator Handle(){return holder_->get();}
  //private:
  //  holder_type holder_;
  //};

  typedef std::unique_ptr<HBITMAP,HBITMAP_deleter> bitmap_holder;

  typedef device_context<d2_dc> d2_dc_type;

  struct paint_struct 
  {
    paint_struct(HWND hwnd) : hwnd_(hwnd)
    {
      ::BeginPaint(hwnd,&paintstruct_);
    }
    ~paint_struct() {::EndPaint(hwnd_,&paintstruct_);}
    PAINTSTRUCT* operator->(){return &paintstruct_;}
  private:
    HWND hwnd_;
    PAINTSTRUCT paintstruct_;
  };

  // GDI オブジェクト管理テンプレート
  template <class GdiObject> 
  struct gdi_object: boost::noncopyable
  {
    explicit gdi_object(GdiObject obj) : gdiobj_(obj) {}
    ~gdi_object(){::DeleteObject(gdiobj_);}
    operator GdiObject(){return gdiobj_;}
  private:
    GdiObject gdiobj_;
  };

  //
  struct select_object 
  {
    select_object(HDC dc,HGDIOBJ o) : dc_(dc),o_(::SelectObject(dc,o)) {}
    ~select_object(){::SelectObject(dc_,o_);}
  private:
    HDC dc_;
    HGDIOBJ o_;
  };

  // Direct2D BeginDrawヘルパ関数
  template <typename T >
  struct begin_draw
  {
    typedef std::function<void(HRESULT hr)> err_handler_type;

    begin_draw(T& render_target,err_handler_type& handler = err_handler_type([](HRESULT hr)->void{throw sf::win32_error_exception(hr);}))
      : render_target_(render_target) ,
      handler_(handler)
    {render_target->BeginDraw();}

    ~begin_draw(){ 
      HRESULT hr = S_OK;
      hr = render_target_->EndDraw();
      if( hr != S_OK)
      {
        handler_(hr);
      }
    }
  private:
    T& render_target_;
    err_handler_type handler_;
  };

  struct mouse
  {
    mouse() : x_(0.0f),y_(0.0f),left_button_(false),middle_button_(false),right_button_(false){}
  private:
    float x_,y_;
    bool left_button_,middle_button_,right_button_;
  };


  // ウィンドウプロシージャの識別用クラス
  struct wndproc 
  {
    typedef WNDPROC proc_type;
    typedef LRESULT return_type;
    static inline return_type def_wnd_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam) 
    {
      return ::DefWindowProcW(hwnd,message,wParam,lParam);
    }
  };

  // ダイアログプロシージャの識別用クラス
  struct dlgproc
  {
    typedef DLGPROC proc_type;
    typedef INT_PTR return_type;
    static inline return_type def_wnd_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam)
    {
      return FALSE;
    }
  };

  /** window ベースクラス */
  template <typename ProcType = wndproc>
  struct base_win32_window : public base_window 
  {
    typedef ProcType proc_t;
    typedef typename proc_t::return_type result_t;
//    typedef proc_t::return_type result_t;

    operator HWND() const {return hwnd_;};

    virtual void * raw_handle() const {return hwnd_;};
    //    virtual void show(uint32_t show_flag);

    virtual void show() {
      ::ShowWindow(hwnd_,SW_SHOW);
      ::GetWindowPlacement(hwnd_,&wp_);
    }

    // Window を画面から隠す
    virtual bool is_show() {
      return ( wp_.showCmd == SW_SHOWMAXIMIZED 
        || wp_.showCmd == SW_SHOWMINIMIZED
        || wp_.showCmd == SW_SHOWNORMAL );
    };

    //
    virtual void hide()
    {
      ::ShowWindow(hwnd_,SW_HIDE);
      ::GetWindowPlacement(hwnd_,&wp_);
    };

    virtual void text(std::wstring& text)
    {
      ::SetWindowTextW(*this,text.c_str());
    };

    virtual void send_message(uint32_t message,uint32_t wparam,uint32_t lparam )
    {
      ::SendNotifyMessage(hwnd_,message,wparam,lparam);
    }

    virtual void post_message(uint32_t message,uint32_t wparam,uint32_t lparam )
    {
      ::PostMessage(hwnd_,message,wparam,lparam);
    }

    virtual void message_box(const std::wstring& text,const std::wstring& caption,uint32_t type = MB_OK)
    {
      ::MessageBox(hwnd_,text.c_str(),caption.c_str(),type);
    }

    virtual void update();

    virtual void create_device_independent_resources();
    virtual void create_device();

    virtual void create_swapchain_dependent_resources();
    virtual void create_d2d_render_target();

    virtual void discard_swapchain_dependent_resources();
    virtual void restore_swapchain_and_dependent_resources();
    virtual void discard_device();
    virtual void discard_device_independant_resources();
    virtual void discard_d2d_render_target();

  protected:
    base_win32_window(
      const std::wstring& title,
      const std::wstring& name,bool fit_to_display,
      float width,float height);


    ~base_win32_window();

    void register_class (
      const wchar_t* menu_name,
      uint32_t style, 
      int32_t     cbClsExtra  = 0,
      int32_t     cbWndExtra  = sizeof(LONG_PTR),
      HICON       hIcon = ::LoadIcon(NULL,IDI_APPLICATION),
      HCURSOR     hCursor = ::LoadCursor(NULL, IDC_ARROW),
      HBRUSH      hbrBackground = ::CreateSolidBrush(0xff000000),
      HICON       hIconSm = NULL
      );		

    /** デフォルト設定 */
    void register_class();
    void create_window(HWND parent = NULL);

    void calc_client_size()
    {
      //クライアント領域の現在の幅、高さを求める
      RECT rc;
      GetClientRect( hwnd_, &rc );
      client_width_ = rc.right - rc.left;
      client_height_ = rc.bottom - rc.top;
    }
  public:
    // SetWindowLong API
    void set_long(int index,long data)
    {
      SetLastError(0);
      if(::SetWindowLongW(hwnd_,index,data) == 0)
      {
        long err = 0;
        if( (err = GetLastError()) != 0){
          SetLastError(err);
          throw sf::win32_error_exception();
        }
      };
    }

    void set_pos(
      HWND hwnd_insert_after,  // 配置順序のhandle
      int x,                 // 横方向の位置
      int y,                 // 縦方向の位置
      int cx,                // 幅
      int cy,                // 高さ
      UINT flags            // ウィンドウ位置のオプション
      )
    {
      BOOL res = SetWindowPos(hwnd_,hwnd_insert_after,x,y,cx,cy,flags);
      if(!res)
      {
        throw win32_error_exception();
      }
    }

    bool invalidate_rect(bool erase = false,const RECT * rect_ptr = 0)
    {
      return ::InvalidateRect(*this,rect_ptr,erase) == TRUE;
    }

    void enable_control(uint32_t id,bool enable)
    {
      ::EnableWindow(GetDlgItem(hwnd_,id),enable?TRUE:FALSE);
    }

    void enable_control(HWND hwnd,uint32_t id,bool enable)
    {
      ::EnableWindow(GetDlgItem(hwnd,id),enable?TRUE:FALSE);
    };

    virtual result_t window_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam);
    virtual result_t other_window_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam)
    {
      return proc_t::def_wnd_proc(hwnd,message,wParam,lParam);
    };

    virtual result_t on_activate(int active,bool minimized);
    // デフォルトウィンドウメッセージハンドラ
    virtual result_t on_nccreate(CREATESTRUCT *p) ;//{ return std::is_same<proc_t,wndproc>::value?1:FALSE;}
    virtual result_t on_create(CREATESTRUCT *p); //{ return std::is_same<proc_t,wndproc>::value?0:FALSE;}
    virtual result_t on_init_dialog(HWND default_focus_ctrl,LPARAM data) {return TRUE;}
    virtual result_t on_size(uint32_t flag,uint32_t width,uint32_t height);//{return std::is_same<proc_t,wndproc>::value?0:FALSE;    }
    //virtual LRESULT 
    virtual result_t on_paint();
    virtual result_t on_display_change(uint32_t bpp,uint32_t h_resolution,uint32_t v_resolution);// {         invalidate_rect();return std::is_same<proc_t,wndproc>::value?0:FALSE;}
    virtual result_t on_erase_backgroud(HDC dc) {return std::is_same<proc_t,wndproc>::value?1:TRUE;}
    virtual result_t on_hscroll(uint32_t state,uint32_t position,HWND ctrl_hwnd) {return std::is_same<proc_t,wndproc>::value?0:FALSE;}
    virtual result_t on_vscroll(uint32_t state,uint32_t position,HWND ctrl_hwnd) {return std::is_same<proc_t,wndproc>::value?0:FALSE;}
    virtual result_t on_left_mouse_button_down(uint32_t mouse_key,int x,int y ) { return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    virtual result_t on_left_mouse_button_up(uint32_t mouse_key,int x,int y) { return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    virtual result_t on_left_mouse_button_double_click(uint32_t mouse_key,int x,int y) { return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    virtual result_t on_mouse_move(uint32_t mouse_key,int x,int y) {return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    virtual result_t on_mouse_wheel(uint32_t mouse_key,int delta,int x,int y) {  return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    //virtual bool on_mouse_enter(uint32_t mouse_key,int x,int y) {  return false; }
    virtual result_t on_mouse_leave() {  return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    virtual result_t on_destroy(){   
      //::PostQuitMessage(0);
      return std::is_same<proc_t,wndproc>::value?0:FALSE;
    }

    virtual result_t on_close()
    {
      discard_device();
      // Windowの破棄
      BOOL ret(::DestroyWindow(hwnd_));
      BOOST_ASSERT(ret != 0);
//      return TRUE;
      return std::is_same<proc_t,wndproc>::value?1:TRUE;
    }

    virtual result_t on_set_cursor() { return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    virtual result_t on_key_down(uint32_t vkey,uint32_t ext_key,uint32_t repeat);
    virtual result_t on_key_up(uint32_t vkey,uint32_t ext_key,uint32_t repeat) { return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    virtual result_t on_app_command(uint32_t command,uint32_t device,uint32_t keystate) {return std::is_same<proc_t,wndproc>::value?0:FALSE;}
    virtual result_t on_command(uint32_t wparam, uint32_t lparam)  { return std::is_same<proc_t,wndproc>::value?0:FALSE; } 
    virtual result_t on_timer(uint32_t timer_id)  {
      //::InvalidateRect(hwnd_,NULL,FALSE);
      render();
      return std::is_same<proc_t,wndproc>::value?0:FALSE; 
    } 
    virtual result_t on_notify(NMHDR* nmhdr)  { return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    virtual result_t on_dwm_composition_changed();
    virtual void render();
  protected:
    void get_dxgi_information();

    // Window生成後呼ばれる関数
    // WM_NCCREATEメッセージの時にthunkに切り替える
    static result_t CALLBACK start_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
      if(message == WM_NCCREATE)
      {
        LPCREATESTRUCT param = reinterpret_cast<LPCREATESTRUCT>(lParam);
        base_win32_window* ptr = reinterpret_cast<base_win32_window*>(param->lpCreateParams);
        ptr->hwnd_ = hwnd;
        // ウィンドウプロシージャをインスタンスと結び付けるthunkプロシージャに入れ替える
        LONG_PTR r = SetWindowLongPtr(hwnd,GWLP_WNDPROC,reinterpret_cast<LONG_PTR>(ptr->thunk_proc_));
        assert(r == reinterpret_cast<LONG_PTR>(&start_wnd_proc));
        return ptr->window_proc(hwnd,message,wParam,lParam);
      }
      return ::DefWindowProcW(hwnd,message,wParam,lParam);
    };

    static result_t CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
      base_win32_window* ptr = reinterpret_cast<base_win32_window*>(hwnd);
      return ptr->window_proc(ptr->hwnd_,message,wParam,lParam);
    };

    // thisとhwndをつなぐthunkクラス
    struct hwnd_this_thunk : public Xbyak::CodeGenerator {
      hwnd_this_thunk(base_win32_window* impl,typename proc_t::proc_type proc)
      {
        // rcxにhwndが格納されているので、それをimpl->hwndに保存
//        mov(ptr[&(impl->hwnd_)],rcx); // <-- エラー発生部分
		mov(rax,(size_t)&impl->hwnd_);	// <-- 訂正
        mov(ptr[rax],rcx);				// <-- 訂正
        // 代わりにthisのアドレスをrcxに格納
        mov(rcx,(LONG_PTR)impl);
        // r10にproc(Window プロシージャ)へのアドレスを格納
        mov(r10,(LONG_PTR)proc);
        // Window プロシージャへへジャンプ
        jmp(r10);
      }
    };

    void update_window_size()
    {
      RECT r;
      GetWindowRect(hwnd_,&r);
      width_ = r.right - r.left;
      height_ = r.bottom - r.top;
    }

    void init_view_matrix();

    HWND hwnd_;
    hwnd_this_thunk thunk_;
    std::wstring title_;
    std::wstring name_;
    float width_,height_;
    bool fit_to_display_;
    std::shared_ptr<sf::window_class_ex> wnd_class_;
    typename proc_t::proc_type thunk_proc_;
    dpi dpi_;
    WINDOWPLACEMENT wp_;
    bool init_;

    ID2D1Factory1Ptr d2d_factory_;
    ID2D1Bitmap1Ptr d2d1_target_bitmap_;
    //ID2D1HwndRenderTargetPtr render_target_;
    ID2D1DevicePtr d2d_device_;
    ID2D1DeviceContextPtr d2d_context_;

    IDWriteFactory1Ptr write_factory_;
    IWICImagingFactoryPtr wic_imaging_factory_;
    IDWriteTextFormatPtr write_text_format_;

    IDXGIFactory2Ptr dxgi_factory_;
    IDXGIAdapter2Ptr dxgi_adapter_;
    IDXGIOutput1Ptr dxgi_output_;
    IDXGIDevice2Ptr dxgi_device_;
    IDXGISurface2Ptr dxgi_back_buffer_;
    DXGI_MODE_DESC1 mode_desc_;
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc_;
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swap_chain_fullscreen_desc_;
    IDXGISwapChain1Ptr dxgi_swap_chain_;
    std::wstring dxgi_info_;

    ID3D11DevicePtr d3d_device_;
    ID3D11DeviceContextPtr d3d_context_;
    ID3D11Texture2DPtr back_buffer_;

    ID3D11RenderTargetViewPtr d3d_render_target_view_;
    ID3D11Texture2DPtr d3d_depth_texture_;
    ID3D11DepthStencilViewPtr depth_view_;
    ID3D11VertexShaderPtr v_shader_;
    ID3D11InputLayoutPtr input_layout_;
    ID3D11PixelShaderPtr p_shader_;
    ID3D11BufferPtr v_buffer_;
    ID3D11BufferPtr i_buffer_;
    ID3D11BufferPtr cb_never_changes_;
    ID3D11BufferPtr cb_change_on_resize_;
    ID3D11BufferPtr cb_changes_every_frame_;
    ID3D11ShaderResourceViewPtr shader_res_view_;
    ID3D11SamplerStatePtr sampler_state_;

    
 /*   ID3D11SamplerStatePtr cube_sampler_state_;
    ID3D11Texture2DPtr cube_texture_;
    ID3D11Texture2DPtr cube_depth_texture_;
    ID3D11ShaderResourceViewPtr cube_shader_res_view_;
    ID3D11RenderTargetViewPtr cube_view_;
    ID3D11DepthStencilViewPtr cube_depth_view_;*/


	
	
    DirectX::XMMATRIX                            mat_world_;
    DirectX::XMMATRIX                            mat_view_;
    DirectX::XMMATRIX                            mat_projection_;
    DirectX::XMMATRIX                            cube_mat_projection_;
	
    DirectX::XMFLOAT4                            mesh_color_;
    float client_width_,client_height_;

    bool activate_;
    bool fullscreen_;
    timer timer_;

    // __declspec ( thread ) static std::queue<proc_t::proc_type> ptrs_ ;// thread local storage

  };

  typedef base_win32_window<> base_win32_window_t;
  typedef base_win32_window<dlgproc> base_win32_dialog_t;

  /// サブクラスウィンドウ
  struct subclass_window : public base_win32_window_t
  {
    subclass_window(HWND hwnd);
    subclass_window();
    void attach(HWND hwnd);
    void detatch();
    ~subclass_window();
    virtual result_t window_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam) 
    {
      return CallWindowProc(proc_backup_,hwnd,message,wParam,lParam);
    };
  protected:
    bool is_subclassed_;
    WNDPROC proc_backup_;
  };

  struct av_mm_thread_characteristics
  {
    av_mm_thread_characteristics(std::wstring& str) : task_name_(str)
    {
      handle_ = ::AvSetMmThreadCharacteristicsW(str.c_str(),(LPDWORD)&task_index_);
    }

    bool set_priority(AVRT_PRIORITY p){return (::AvSetMmThreadPriority(handle_,p) == TRUE);}

    ~av_mm_thread_characteristics()
    {
      ::AvRevertMmThreadCharacteristics(handle_);
    }

  private:
    std::wstring task_name_;
    uint32_t task_index_;
    HANDLE handle_;
  };

  struct widget
  {
    void draw();
    float x_,y_;
  };

  typedef sf::begin_draw<ID2D1BitmapRenderTargetPtr> begin_draw_bitmap;
  typedef sf::begin_draw<ID2D1HwndRenderTargetPtr> begin_draw_hwnd;

}