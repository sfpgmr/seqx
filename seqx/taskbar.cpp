#include "StdAfx.h"

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "sf_windows.h"
#include "taskbar.h"


namespace sf {

const int taskbar::none = TBPF_NOPROGRESS;
const int taskbar::indeterminate = TBPF_INDETERMINATE;
const int taskbar::normal = TBPF_NORMAL;
const int taskbar::error = TBPF_ERROR;
const int taskbar::paused = TBPF_PAUSED;

long  taskbar::register_message()
{
  return ::RegisterWindowMessage(L"TaskbarButtonCreated");
}

struct taskbar::impl
{
  typedef throw_if_err<sf::taskbar::exception> throw_if_err_;

  impl(){}
  ~impl()
  {
    discard();
  }

  void create() {
	 throw_if_err_()(CoCreateInstance(CLSID_TaskbarList,nullptr,CLSCTX::CLSCTX_INPROC_SERVER,__uuidof(ITaskbarList4),(LPVOID*)taskbar_.GetAddressOf()));
  }
  bool is_create() const 
  {
    return (taskbar_ != 0);
  }
  void discard()
  {
    safe_release(taskbar_);
  }
    
  void overlay_icon(const sf::base_window& w,const sf::icon& ic,const std::wstring& description)
  {
    throw_if_err_()(taskbar_->SetOverlayIcon(reinterpret_cast<HWND>(w.raw_handle()),ic.get(),description.c_str()));
  }

  void progress_state(const sf::base_window& w,TBPFLAG state)
  {
    throw_if_err_()(taskbar_->SetProgressState(reinterpret_cast<HWND>(w.raw_handle()),state));
  }

  void progress_value(const sf::base_window& w,boost::uint64_t completed, boost::uint64_t total)
  {
    throw_if_err_()(taskbar_->SetProgressValue(reinterpret_cast<HWND>(w.raw_handle()),completed,total));
  }

  void add_thumb_buttons(const sf::base_window& w,const std::vector<THUMBBUTTON>& tbs){
      taskbar_->ThumbBarAddButtons(reinterpret_cast<HWND>(w.raw_handle()),tbs.size(),const_cast<LPTHUMBBUTTON>(&(tbs[0])));
  };

  void update_thumb_buttons(const sf::base_window& w,const std::vector<THUMBBUTTON>& tbs){
      taskbar_->ThumbBarUpdateButtons(reinterpret_cast<HWND>(w.raw_handle()),tbs.size(),const_cast<LPTHUMBBUTTON>(&(tbs[0])));
  };

private:
  _WRL_PTR_TYPEDEF(ITaskbarList4);
  ITaskbarList4Ptr taskbar_;
};

   
taskbar::taskbar() : impl_(new sf::taskbar::impl()) {}
taskbar::~taskbar() { discard();};

void taskbar::create(){impl_->create();};
bool taskbar::is_create() const {return impl_->is_create();};
void taskbar::discard(){impl_->discard();};
void taskbar::overlay_icon(const sf::base_window& w,const icon& ic,const std::wstring& description){impl_->overlay_icon(w,ic,description);};
void taskbar::progress_state(const sf::base_window& w,int state){impl_->progress_state(w,(TBPFLAG)state);};
void taskbar::progress_value(const sf::base_window& w,boost::uint64_t completed, boost::uint64_t total){impl_->progress_value(w,completed,total);};
void taskbar::add_thumb_buttons(const sf::base_window& w,const thumb_button_manager& tm){
  BOOST_ASSERT(!tm.is_added);
  impl_->add_thumb_buttons(w,tm.thumbbuttons_);
  tm.is_added = true;
};
void taskbar::update_thumb_buttons(const sf::base_window& w,const thumb_button_manager& tm){
  BOOST_ASSERT(tm.is_added);
  if(tm.is_added){
    impl_->update_thumb_buttons(w,tm.thumbbuttons_);
  }
};

}