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

#include "StdAfx.h"

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "sf_windows.h"
#include "icon.h"
#include "application.h"

namespace sf {
  bitmap_holder icon::default_mono_bitmap(CreateBitmap(32,32,1,1,NULL));

  icon::icon(uint32_t id) 
    : icon_((HICON)::LoadImageW(application::instance()->instance_handle(),MAKEINTRESOURCE(id),IMAGE_ICON,0,0,LR_DEFAULTCOLOR))
  {

  };

  icon::icon(icon_holder& ic)
  {
    icon_.reset(::CopyIcon(ic.get()));
    analyze();
  }

  icon::icon(icon_holder&& ic)
  {
    std::swap(ic,icon_);
    analyze();
  }

  icon::icon(bitmap_holder& bmp_color,int width,int height) 
    : width_(width),height_(height)
  {
    BITMAP bmp;
    ICONINFO ii;
    ::GetObjectW(bmp_color.get(),sizeof(BITMAP),&bmp);
    bits_per_pixel_ = bmp.bmBitsPixel;

    ii.fIcon = TRUE;
    ii.xHotspot = 0;
    ii.yHotspot = 0;
    ii.hbmColor = bmp_color.get();

    if(width == 32 && height == 32){
      ii.hbmMask = default_mono_bitmap.get();
      icon_.reset(::CreateIconIndirect(&ii));
    } else {
      bitmap_holder hmb(::CreateBitmap(width,height,1,1,NULL));
      ii.hbmMask = hmb.get();
      icon_.reset(::CreateIconIndirect(&ii));
    }

  }

  icon::icon(ID2D1BitmapPtr& ptr)
  {
    ID2D1FactoryPtr factory;
    ptr->GetFactory(&factory);

    D2D1_SIZE_U size(ptr->GetPixelSize());
    // ビットマップヘッダのセットアップ
    BITMAPV5HEADER bi = {0};
    bi.bV5Size           = sizeof(BITMAPV5HEADER);
    bi.bV5Width           = size.width;
    bi.bV5Height          = size.height;
    bi.bV5Planes = 1;
    bi.bV5BitCount = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask   =  0x00FF0000;
    bi.bV5GreenMask =  0x0000FF00;
    bi.bV5BlueMask  =  0x000000FF;
    bi.bV5AlphaMask =  0xFF000000; 

    // デスクトップHDCの取得
    get_dc dc(NULL);
    
    // DIBセクションの作成
    void *bits;// 得られるビットマップ
    bitmap_holder bmp(
      ::CreateDIBSection(
        dc.get(),reinterpret_cast<BITMAPINFO *>(&bi),DIB_RGB_COLORS,&bits,NULL,0));
    { 
      // 互換DCの作成
      compatible_dc cdc(dc.get());
      {
        // 描画先への切り替え
        select_object s(cdc.get(),bmp.get());

        // DC互換レンダーターゲットのセットアップ
        D2D1_RENDER_TARGET_PROPERTIES 
          props = D2D1::RenderTargetProperties(
          D2D1_RENDER_TARGET_TYPE_DEFAULT,
          D2D1::PixelFormat(
              DXGI_FORMAT_B8G8R8A8_UNORM,
              D2D1_ALPHA_MODE_PREMULTIPLIED),
          0,
          0,
          D2D1_RENDER_TARGET_USAGE_NONE,
          D2D1_FEATURE_LEVEL_DEFAULT
        );

        ID2D1DCRenderTargetPtr dcr;
        throw_if_err<>()(factory->CreateDCRenderTarget(&props,dcr.GetAddressOf()));
        RECT rect = {0,0,size.width,size.height};
        // 互換DCへのバインド
        throw_if_err<>()(dcr->BindDC(cdc.get(),&rect));
        dcr->DrawBitmap(ptr.Get());
      }
    }
    icon(bmp,size.width,size.height);
  };


  icon::icon(boost::filesystem::wpath& path)
  {
    icon_.reset(
      reinterpret_cast<HICON>(
        LoadImageW(NULL,path.native().c_str(),IMAGE_ICON,0,0,LR_DEFAULTSIZE | LR_LOADFROMFILE)));
  }

  icon::~icon()
  {

  }

  void icon::analyze()
  {
    ::ICONINFOEXW info;
    ::GetIconInfoExW(icon_.get(),&info);
    BITMAP bmp;
    ::GetObjectW(info.hbmColor,sizeof(BITMAP),&bmp);
    width_ = bmp.bmWidth;
    height_ = bmp.bmHeight;
    bits_per_pixel_ = bmp.bmBitsPixel;
  }

  icon& icon::operator= (icon& i)
  {
    BOOST_ASSERT(icon_ != i.icon_);
    if(icon_ == i.icon_) return *this;
    icon_.reset(::CopyIcon(i.icon_.get()));
    width_ = i.width_;
    height_ = i.height_;
    bits_per_pixel_ = i.bits_per_pixel_;
    return *this;
  }

  //icon_ptr icon::create_icon()
  //{
 
  //  // ビットマップヘッダのセットアップ
  //  BITMAPV5HEADER bi = {0};
  //  bi.bV5Size           = sizeof(BITMAPV5HEADER);
  //  bi.bV5Width           = width_;
  //  bi.bV5Height          = height_;
  //  bi.bV5Planes = 1;
  //  bi.bV5BitCount = 32;
  //  bi.bV5Compression = BI_BITFIELDS;
  //  bi.bV5RedMask   =  0x00FF0000;
  //  bi.bV5GreenMask =  0x0000FF00;
  //  bi.bV5BlueMask  =  0x000000FF;
  //  bi.bV5AlphaMask =  0xFF000000; 

  //  // デスクトップHDCの取得
  //  get_dc dc(NULL);

  //  // DIBセクションの作成
  //  void *bits;// 得られるビットマップ
  //  gdi_object<HBITMAP> bmp(
  //    ::CreateDIBSection(
  //      dc.get(),reinterpret_cast<BITMAPINFO *>(&bi),DIB_RGB_COLORS,&bits,NULL,0));
  //  { 
  //    // 互換DCの作成
  //    compatible_dc cdc(dc.get());
  //    {
  //      // 描画先への切り替え
  //      select_object s(cdc.get(),bmp);

  //      // DC互換レンダーターゲットのセットアップ
  //      D2D1_RENDER_TARGET_PROPERTIES 
  //        props = D2D1::RenderTargetProperties(
  //        D2D1_RENDER_TARGET_TYPE_DEFAULT,
  //        D2D1::PixelFormat(
  //            DXGI_FORMAT_B8G8R8A8_UNORM,
  //            D2D1_ALPHA_MODE_PREMULTIPLIED),
  //        0,
  //        0,
  //        D2D1_RENDER_TARGET_USAGE_NONE,
  //        D2D1_FEATURE_LEVEL_DEFAULT
  //      );

  //      ID2D1DCRenderTargetPtr dcr;
  //      throw_if_err<>()(d2d_factory_->CreateDCRenderTarget(&props,&dcr));
  //      RECT rect = {0,0,w,h};
  //      // 互換DCへのバインド
  //      throw_if_err<>()(dcr->BindDC(cdc.get(),&rect));

  //      // ブラシのセットアップ(背景の赤丸）
  //      ID2D1SolidColorBrushPtr brush_e;
  //      throw_if_err<>()(
  //        dcr->CreateSolidColorBrush(
  //          D2D1::ColorF(D2D1::ColorF::Red,0.8f), &brush_e));

  //      // アイコンに描画する文字の生成
  //      std::wstring t(L"S.F.\nTimer");
  //      D2D1_RECT_F l = D2D1::RectF(0.0f,0.0f,width_,height_);
  //      // Text Formatの作成
  //      IDWriteTextFormatPtr f;
  //      write_factory_->CreateTextFormat(
  //      L"メイリオ",                // Font family name.
  //      NULL,                       // Font collection (NULL sets it to use the system font collection).
  //      DWRITE_FONT_WEIGHT_REGULAR,
  //      DWRITE_FONT_STYLE_NORMAL,
  //      DWRITE_FONT_STRETCH_NORMAL,
  //      10.0f,
  //      L"ja-jp",
  //      &f
  //      );
  //      f->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
  //      f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

  //      // 文字描画用ブラシのセットアップ
  //      ID2D1SolidColorBrushPtr brush;
  //      dcr->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &brush);
  //      // 描画開始
  //      dcr->BeginDraw();
  //      // ビットマップクリア
  //      dcr->Clear(D2D1::ColorF(D2D1::ColorF::Black,0.0f));
  //      // 赤丸を描く
  //      dcr->FillEllipse(D2D1::Ellipse(D2D1::Point2F(16.0f,16.0f),14,14),brush_e);
  //      // テキストを表示する
  //      dcr->DrawTextW(t.c_str(),t.size(),f,&l,brush);
  //      // 描画終了
  //      dcr->EndDraw();
  //    }
  //  }
  //}
}
