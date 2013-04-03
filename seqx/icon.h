#pragma once
/*
==============================================================================

This file is part of the async
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

namespace sf {

  struct icon_deleter {
    typedef HICON pointer;
    void operator ()(HICON handle) {
      if (handle) {
        ::DestroyIcon(handle);
      }
    }
  };

  /** iconハンドルの解放し忘れを予防する */
  typedef std::unique_ptr<HICON,icon_deleter> icon_holder;

  /** 
  　icon管理クラス
  */
  struct icon
  {
  public:
    icon(){};
    explicit icon(uint32_t id);/// リソースIDからアイコンを作成する
    explicit icon(icon_holder& ic);/// アイコンフォルダからアイコンを作成する
    explicit icon(icon_holder&& ic);/// 右辺value参照からアイコンを作成する
    icon(bitmap_holder& bmp_color,int width = ::GetSystemMetrics(SM_CXICON),int height = ::GetSystemMetrics(SM_CYICON));
    icon(ID2D1BitmapPtr& ptr);
    icon(boost::filesystem::wpath& path);

    virtual ~icon();

    HICON get() const {return icon_.get();};

    icon(icon& i) : width_(i.width_),height_(i.height_),bits_per_pixel_(i.bits_per_pixel_)
    {
      icon_.reset(::CopyIcon(i.icon_.get()));
    }

    icon(icon&& i) : width_(i.width_),height_(i.height_),bits_per_pixel_(i.bits_per_pixel_)
    {
      std::swap(icon_,i.icon_);
    }

    icon& operator= (icon&& i) 
    {
      BOOST_ASSERT(icon_ != i.icon_);
      if(icon_ == i.icon_) return *this;
      std::swap(icon_,i.icon_);
      width_ = i.width_;
      height_ = i.height_;
      bits_per_pixel_ = i.bits_per_pixel_;
      return *this;
    }

    icon& operator= (icon& i);

    void swap(icon& i)
    {
      std::swap(icon_,i.icon_);
      std::swap(width_,i.width_);
      std::swap(height_,i.height_);
      std::swap(bits_per_pixel_,i.bits_per_pixel_);
    }

  private:

    void analyze();
    //ID2D1DCRenderTargetPtr dcr_;
    //ID2D1BitmapPtr bitmap_;
    int width_;
    int height_;
    int bits_per_pixel_;
    icon_holder icon_;
    static bitmap_holder default_mono_bitmap; 
  };
}

