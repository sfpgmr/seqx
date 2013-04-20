#include "stdafx.h"
#include "exception.h"
#include <objbase.h>
#include <wtypes.h>
#include <winerror.h>
#include <avrt.h>
#include <strsafe.h>
#include <audioclient.h>
#include <audiopolicy.h>



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

#include "sf_memory.h"

using namespace boost;

namespace sf
{
//typedef CComPtr<IAudioClient> IAudioClientPtr;
std::map<HRESULT,std::wstring> com_error_  = boost::assign::list_of<std::pair<HRESULT,std::wstring> >
    (E_POINTER,L"E_POINTER")
    (E_INVALIDARG,L"E_INVALIDARG")
    (DWRITE_E_FILEFORMAT,L"DWRITE_E_FILEFORMAT")
    (DWRITE_E_UNEXPECTED,L"DWRITE_E_UNEXPECTED")
    (DWRITE_E_NOFONT,L"DWRITE_E_NOFONT")
    (DWRITE_E_FILENOTFOUND,L"DWRITE_E_FILENOTFOUND")
    (DWRITE_E_FILEACCESS,L"DWRITE_E_FILEACCESS")
    (DWRITE_E_FONTCOLLECTIONOBSOLETE,L"DWRITE_E_FONTCOLLECTIONOBSOLETE")
    (DWRITE_E_ALREADYREGISTERED,L"DWRITE_E_ALREADYREGISTERED")
	(AUDCLNT_E_NOT_INITIALIZED,L"AUDCLNT_E_NOT_INITIALIZED")
	(AUDCLNT_E_ALREADY_INITIALIZED,L"AUDCLNT_E_ALREADY_INITIALIZED")
    (AUDCLNT_E_WRONG_ENDPOINT_TYPE,L"AUDCLNT_E_WRONG_ENDPOINT_TYPE")
	(AUDCLNT_E_DEVICE_INVALIDATED,L"AUDCLNT_E_DEVICE_INVALIDATED")
	(AUDCLNT_E_NOT_STOPPED,L"AUDCLNT_E_NOT_STOPPED")
	(AUDCLNT_E_BUFFER_TOO_LARGE,L"AUDCLNT_E_BUFFER_TOO_LARGE")
	(AUDCLNT_E_OUT_OF_ORDER,L"AUDCLNT_E_OUT_OF_ORDER")
	(AUDCLNT_E_UNSUPPORTED_FORMAT,L"AUDCLNT_E_UNSUPPORTED_FORMAT")
	(AUDCLNT_E_INVALID_SIZE,L"AUDCLNT_E_INVALID_SIZE")
	(AUDCLNT_E_DEVICE_IN_USE,L"AUDCLNT_E_DEVICE_IN_USE")
	(AUDCLNT_E_BUFFER_OPERATION_PENDING,L"AUDCLNT_E_BUFFER_OPERATION_PENDING")
	(AUDCLNT_E_THREAD_NOT_REGISTERED,L"AUDCLNT_E_THREAD_NOT_REGISTERED")
	(AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED,L"AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED")
	(AUDCLNT_E_ENDPOINT_CREATE_FAILED,L"AUDCLNT_E_ENDPOINT_CREATE_FAILED")
	(AUDCLNT_E_SERVICE_NOT_RUNNING,L"AUDCLNT_E_SERVICE_NOT_RUNNING")
	(AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED,L"AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED")
	(AUDCLNT_E_EXCLUSIVE_MODE_ONLY,L"AUDCLNT_E_EXCLUSIVE_MODE_ONLY")
	(AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL,L"AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL")
	(AUDCLNT_E_EVENTHANDLE_NOT_SET,L"AUDCLNT_E_EVENTHANDLE_NOT_SET")
	(AUDCLNT_E_INCORRECT_BUFFER_SIZE,L"AUDCLNT_E_INCORRECT_BUFFER_SIZE")
  (AUDCLNT_E_CPUUSAGE_EXCEEDED,L"AUDCLNT_E_CPUUSAGE_EXCEEDED")
  (AUDCLNT_E_BUFFER_ERROR,L"AUDCLNT_E_BUFFER_ERROR")
  (AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED,L"AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED")
  (AUDCLNT_E_BUFFER_SIZE_ERROR,L"AUDCLNT_E_BUFFER_SIZE_ERROR")
  (AUDCLNT_E_INVALID_DEVICE_PERIOD,L"AUDCLNT_E_INVALID_DEVICE_PERIOD")
	(AUDCLNT_S_BUFFER_EMPTY,L"AUDCLNT_S_BUFFER_EMPTY")
	(AUDCLNT_S_THREAD_ALREADY_REGISTERED,L"AUDCLNT_S_THREAD_ALREADY_REGISTERED")
  (AUDCLNT_S_POSITION_STALLED,L"AUDCLNT_S_POSITION_STALLED")
  (DXGI_ERROR_UNSUPPORTED,L"DXGI_ERROR_UNSUPPORTED ")
  (DXGI_STATUS_OCCLUDED,L"DXGI_STATUS_OCCLUDED")
  (DXGI_STATUS_CLIPPED,L"DXGI_STATUS_CLIPPED")
  ;
	

win32_error_exception::win32_error_exception(uint32_t hr)
: std::exception("HRESULT ERROR"),hresult_(hr)
{
	local_memory<wchar_t> mem;
	DWORD result = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,0,hr,0,(LPWSTR)&mem,0,0);
	if(result != 0){
		error_ = mem.get();
	} else {
		std::map<HRESULT,std::wstring>::iterator it = com_error_.find(hr);
		if(it != com_error_.end())
		{
			error_ = it->second;
		} else {
			error_ = (boost::wformat(L"0x%x 不明なCOMエラー") % hr).str();
		}
#ifdef _DEBUG
  debug_out(boost::wformat(L"#### Exception Occured #### %s \n") %  error_ ); 
#endif
	}

};

win32_error_exception::win32_error_exception()
{
	hresult_ = ::GetLastError();
	local_memory<wchar_t> mem;
	DWORD rv =  FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,0,hresult_,0,(LPWSTR)&mem,0,0);
	error_ = mem.get();
#ifdef _DEBUG
  debug_out( boost::wformat( L"#### Exception Occured #### %s \n") % error_);
#endif

  //Logger::outputDebugPrintf(L"Win32 Error %x %s",hresult_,mem.Get() );
};

std::wstring win32_error_exception::get_last_error_str(uint32_t err)
{
//	DWORD err = ::GetLastError();
  std::wstring err_str;
	local_memory<wchar_t> mem;
	DWORD rv =  FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,0,err,0,(LPWSTR)&mem,0,0);
	err_str = mem.get();
#ifdef _DEBUG
  debug_out(boost::wformat(L"#### Exception Occured #### %s \n" )% err_str); 
#endif
  return err_str;
}

}