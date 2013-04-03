/*
==============================================================================

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
/* ToDo

TODO: リサイズに対応する

*/

#include "stdafx.h"
#include "resource.h"
#define BOOST_ASSIGN_MAX_PARAMS 7
#include <boost/assign.hpp>
#include <boost/assign/ptr_list_of.hpp>
#include <boost/assign/ptr_list_inserter.hpp>
#include <boost/foreach.hpp>

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "sf_windows.h"
#include "tab_dialog.h"
#include "CommDlg.h"
#include "icon.h"
#include "timer.h"
#include "exception.h"
#include "application.h"

#define THROW_IFERR(hres) \
  if (FAILED(hres)) { throw sf::win32_error_exception(hres); }

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

namespace sf 
{
    tab_dialog_base::tab_dialog_base(sf::base_window& parent_window,HWND tab_hwnd,int tab_id,const std::wstring& menu_name,const std::wstring& name,HINSTANCE inst,LPCTSTR temp)
      : base_win32_dialog_t(menu_name,name,false,0,0),parent_window_(parent_window)
      ,tab_hwnd_(tab_hwnd),tab_id_(tab_id),inst_(inst),temp_(temp)
    {

    }

    void tab_dialog_base::create()
    {
      hwnd_ =  CreateDialogW(inst_,temp_,(HWND)parent_window_.raw_handle(),thunk_proc_);
      BOOST_ASSERT(hwnd_ != NULL);
      if(hwnd_ == NULL)
      {
        throw win32_error_exception();
      }

      resize();
    }

    void tab_dialog_base::resize()
    {
      
      DWORD dwDlgBase = GetDialogBaseUnits();
      int cxMargin = LOWORD(dwDlgBase) / 4; 
      int cyMargin = HIWORD(dwDlgBase) / 8;
      RECT rci;
      TabCtrl_GetItemRect(tab_hwnd_,0,&rci);
      RECT r;
      GetClientRect(tab_hwnd_,&r);
      RECT rw;
      GetWindowRect(tab_hwnd_,&rw);
      POINT pt = {rw.left,rw.top + rci.bottom};
      ScreenToClient((HWND)parent_window_.raw_handle(),&pt);

      set_pos(HWND_TOP,
        pt.x + cxMargin,
        pt.y + cyMargin,
        r.right - cxMargin * 2 - 1,
        r.bottom - rci.bottom - cyMargin * 2 - 1
        ,SWP_NOZORDER 
        );
    }
}

