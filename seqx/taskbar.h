#pragma once

/*
  ==============================================================================

   This file is part of the async
   Copyright 2005-10 by Satoshi Fujiwara.

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

#include "exception.h"
#include "icon.h"

namespace sf
{
  
  ///** インターフェース */
  //struct thumb_button : boost::noncopyable
  //{
  //  typedef std::shared_ptr<thumb_button> ptr;
  //  static ptr create_thumb_button();
  //  virtual void show()  = 0;
  //  virtual void hide()  = 0;
  //  virtual void enable() = 0;
  //  virtual void disable() = 0;
  //private:
  //  thumb_button();
  //};

  struct taskbar;

  /** サムネイルボタン管理 */
  struct thumb_button_manager
  {
    friend struct taskbar;
    // Thumb Buttonは最大7個まで
    static const int THUMB_BUTTON_MAX = 7;

    // THUMBBUTTON構造体操作クラス
    struct thumb_button {

      // コンストラクタ
      thumb_button(uint32_t id ,const icon& i,const std::wstring& tool_tip,THUMBBUTTON& t) : thumb_(t)
      {
        BOOST_ASSERT(tool_tip.size() < 259);
        memset(&thumb_,0,sizeof(THUMBBUTTON));
        thumb_.dwMask = THB_ICON | THB_TOOLTIP | THB_FLAGS;
        thumb_.hIcon = i.get();
        thumb_.iId = id;
        std::copy(tool_tip.begin(),tool_tip.end(),thumb_.szTip);
        //thumb_.szTip = const_cast<wchar_t*>(tool_tip_.c_str());
      }
      
      // 今回はプロパティ・チェイニングなアクセス方法にしてみた。
      /** アイコン */
      thumb_button& set_icon(icon& ic) {thumb_.hIcon = ic.get(); return *this;};
      /** IDのセット */
      thumb_button& id(uint32_t v) {thumb_.iId = v; return *this;}
      /** ツールチップ */
      thumb_button& tool_tip(const std::wstring& v) {
        BOOST_ASSERT(v.size() < 259);
        std::copy(v.begin(),v.end(),thumb_.szTip);
        thumb_.szTip[v.size()] = L'\0';
        return *this;
      }
      /** 有効化・無効化 */
      thumb_button& enable(bool v){v?(thumb_.dwFlags &= ~THBF_ENABLED):(thumb_.dwFlags |= THBF_DISABLED); return *this;}
      /** クリックされたらサムネイルを閉じるか */
      thumb_button& dismission_click(bool v){ v?(thumb_.dwFlags |= THBF_DISMISSONCLICK):(thumb_.dwFlags &= ~THBF_DISMISSONCLICK); return *this;}
      /** ボタンの外枠を描画するかどうか */
      thumb_button& no_background(bool v){v?(thumb_.dwFlags |= THBF_NOBACKGROUND):(thumb_.dwFlags &= ~THBF_NOBACKGROUND);return *this;}
      /** 隠すか・表示するか */
      thumb_button& hidden(bool v){v?(thumb_.dwFlags |= THBF_HIDDEN):(thumb_.dwFlags &= ~ THBF_HIDDEN);return *this;}
      /** ボタンアクションを起こすかどうか。ボタンを通知目的で使用する際に用いる。*/
      thumb_button& no_interacive(bool v){v?(thumb_.dwFlags |= THBF_NONINTERACTIVE):(thumb_.dwFlags &= ~THBF_NONINTERACTIVE);return *this;}

    private:
      THUMBBUTTON& thumb_;
    };
    
    thumb_button_manager() : is_added(false) {}

    // Thumb Buttonは7個までに制限されている
    thumb_button&  add_thumb_button(uint32_t id,const icon& i,const std::wstring& str)
    {
      // 事前条件
      BOOST_ASSERT(thumb_buttons_.size() < 7 && is_added == false);
      if(!is_added){
        thumbbuttons_.push_back(THUMBBUTTON());
        thumb_buttons_.push_back(thumb_button(id,i,str, (thumbbuttons_.at(thumbbuttons_.size() - 1))));
      }
      return thumb_buttons_.at(thumb_buttons_.size() - 1);
    }

    thumb_button& at(uint32_t i){
      BOOST_ASSERT(i < thumb_buttons_.size());
      return thumb_buttons_.at(i);
    }

    size_t size() const {return thumb_buttons_.size();}
  private:
    mutable bool is_added;
    // メモリ配列を合わせるためにこうした。
    std::vector<THUMBBUTTON> thumbbuttons_;
    std::vector<thumb_button> thumb_buttons_;
    //typedef boost::ptr_vector<thumb_button> thumb_buttons;
  };

  /** タスクバーAPIのラップクラス。本来であればインターフェースにする方が良いかもしれないけれど */
  struct taskbar : boost::noncopyable
  {
    struct exception
     : public sf::win32_error_exception 
    {
      exception(uint32_t hr) : win32_error_exception(hr) {};
      exception() : win32_error_exception() {} ;
    };

     taskbar();
    ~taskbar();
    void create();
    void discard();
    void overlay_icon(const sf::base_window& w,const icon& ic,const std::wstring& description);
    void progress_state(const sf::base_window& w,int state);
    void progress_value(const sf::base_window& w,boost::uint64_t completed, boost::uint64_t total);

    void add_thumb_buttons(const sf::base_window& w,const thumb_button_manager& tm);
    void update_thumb_buttons(const sf::base_window& w,const thumb_button_manager& tm);
    bool is_create() const;

    static long register_message();

    static const int none;
    static const int indeterminate;
    static const int normal;
    static const int error;
    static const int paused;

  private:
    struct impl;
    std::shared_ptr<impl> impl_;
    friend struct impl;
  };

}

