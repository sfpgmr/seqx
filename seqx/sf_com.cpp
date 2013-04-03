/*
  ==============================================================================

   This file is part of the S.F.Tracker
   Copyright 2005-7 by Satoshi Fujiwara.

   S.F.Tracker can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   S.F.Tracker is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with S.F.Tracker; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
   Boston, MA 02111-1307 USA

  ==============================================================================
*/
/** @file
 *  @brief util
 *  @author S.F. (Satoshi Fujiwara)
 */

#include "stdafx.h"
#include <objbase.h>
#include "sf_com.h"
namespace sf {

    struct com_initialize::impl
    {
		impl(void * reserved,unsigned int init) : hr(::CoInitializeEx(reserved,init))
        {
        }

        ~impl()
        {
			if(hr == S_OK){
	            ::CoUninitialize();
			}
        }
	private:
		HRESULT hr;
    };

    com_initialize::com_initialize(void * reserved,unsigned int  init)
        : m_impl(new com_initialize::impl(reserved,init))
    {
    };

	//template <typename ComClass,typename ComInterface> boost::intrusive_ptr<ComInterface> com_creator<ComClass,ComInterface>::create_instance()
	//{
	//	ComClass * com_ptr;
	//	CoCreateInstance( __uuidof(ComClass), NULL,
	//			 CLSCTX_ALL, __uuidof(ComInterface),
	//			 (void**)&com_ptr);
	//	return instrusive_ptr<ComClass>(com_ptr,false);
	//};


}