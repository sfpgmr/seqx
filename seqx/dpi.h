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

namespace sf{
  template <typename T>
  class dpi_t
  {
  public:
    dpi_t() : init_(false), dpi_x_(96), dpi_y_(96) { }
    // 画面の DPI を取得します。
    T dpix() { init(); return dpi_x_; }
    T dpiy() { init(); return dpi_y_; }

    // 絶対ピクセルと相対ピクセル間の変換を行います。
    T scale_x(T x) { init(); return MulDiv(x, dpi_x_, 96); }
    T scale_y(T y) { init(); return MulDiv(y, dpi_y_, 96); }
    T unscale_x(T x) { init(); return MulDiv(x, 96, dpi_x_); }
    T unscale_y(T y) { init(); return MulDiv(y, 96, dpi_y_); }

    // 画面サイズ (相対ピクセル単位) を求めます。
    T scaled_screen_width() { return scaled_system_metrix_x(SM_CXSCREEN); }
    T scaled_screen_height() { return scaled_system_metrix_y(SM_CYSCREEN); }

    // 四角形のサイズを絶対ピクセルから相対ピクセルに変更します。
    void scale_rect(RECT *rect_ptr)
    {
      rect_ptr->left = scale_x(rect_ptr->left);
      rect_ptr->right = scale_x(rect_ptr->right);
      rect_ptr->top = scale_y(rect_ptr->top);
      rect_ptr->bottom = scale_y(rect_ptr->bottom);
    }

    // 画面解像度が最低value (相対ピクセル単位) を満たしているかどうかを
    // 確認します。
    bool is_resolution_at_least(T xmin, T ymin) 
    { 
      return (scaled_screen_width() >= xmin) && (scaled_screen_height() >= ymin); 
    }
    // ポイント サイズ (1/72 インチ) を絶対ピクセルに変換します。
    T point_to_pixels(int pt) { return MulDiv(pt, dpi_y_, 72); }
    // キャッシュされたメトリックをすべて無効にします。
    void invalidate() { init_ = false; }
  private:
    void init();

    T scaled_system_metrix_x(int nIndex) 
    { 
      init(); 
      return MulDiv(GetSystemMetrics(nIndex), 96, dpi_x_); 
    }

    T scaled_system_metrix_y(int nIndex) 
    { 
      init(); 
      return MulDiv(GetSystemMetrics(nIndex), 96, dpi_y_); 
    }

  private:
    bool init_;
    T dpi_x_;
    T dpi_y_;
  };

  typedef dpi_t<int> dpi;
}
