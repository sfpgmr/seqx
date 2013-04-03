#pragma once
/*
  ==============================================================================

   This file is part of the mfsample
   Copyright 2005-11 by Satoshi Fujiwara.

   mfsample can be redistributed and/or modified under the terms of the
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
/** @file
 *  @brief util
 *  @author S.F. (Satoshi Fujiwara)
 */
#include "objbase.h"



namespace sf 
{
  template <class COM_SMART_PTR > inline void safe_release(COM_SMART_PTR& ptr)
  {
    if(ptr)
    {
      ptr.Reset();
    }
  };
    enum com_init 
    {
        multi_threaded  = 0x0,
        apartment_threaded = 0x2,
        disable_ole1dde   = 0x4,
        speed_over_memory = 0x8
    };

    struct com_initialize
    {
        struct impl;
        com_initialize(void * reserved ,unsigned int init);
        com_initialize(){com_initialize(0,multi_threaded);};
        ~com_initialize() {};
    private:
        std::shared_ptr<impl> m_impl;
    };

	template <typename ComClass,typename ComInterface> 
		  boost::intrusive_ptr<ComInterface> create_instance()
		  {
			ComInterface * com_ptr;
			CoCreateInstance( __uuidof(ComClass), NULL,
					 CLSCTX_ALL, __uuidof(ComInterface),
					 (void**)&com_ptr);
			return boost::intrusive_ptr<ComInterface>(com_ptr,false);

		  };
			template <typename COMInterface> 
		struct IUnknownImpl : public COMInterface 
		{
			IUnknownImpl() : ref_(1) {}; 
			virtual ~IUnknownImpl() {};
			ULONG __stdcall AddRef()
			{
				return InterlockedIncrement(&ref_);
			}

			ULONG __stdcall Release()
			{
				ULONG ref = InterlockedDecrement(&ref_);
				if (0 == ref)
				{
					delete this;
				}
				return ref;
			}

			HRESULT __stdcall QueryInterface(REFIID riid, VOID **ppObj)
			{
				if (IID_IUnknown == riid)
				{
					AddRef();
					*ppObj = (IUnknown*)this;
				}
				else if (__uuidof(COMInterface) == riid)
				{
					AddRef();
					*ppObj = (COMInterface*)this;
				}
				else
				{
					*ppObj = NULL;
					return E_NOINTERFACE;
				}
				return S_OK;
			}
		private:
			LONG ref_;
		};
}