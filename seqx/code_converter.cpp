#include "stdafx.h"

namespace sf 
{

  code_converter<char,wchar_t>::code_converter(const char*p)
  {
    int len = ::MultiByteToWideChar(CP_ACP,0,p,-1,NULL,0);
    m_dest.reset(new wchar_t[len]);
    ::MultiByteToWideChar(CP_ACP,0,p,-1,&m_dest[0],len);
  };

  code_converter<char,wchar_t>::code_converter(const std::string& p)
  {
    int len = ::MultiByteToWideChar(CP_ACP,0,p.c_str(),-1,NULL,0);
    m_dest.reset(new wchar_t[len]);
    ::MultiByteToWideChar(CP_ACP,0,p.c_str(),-1,&m_dest[0],len);
  };

  code_converter<wchar_t,char>::code_converter(const wchar_t*p)
  {
    int len = ::WideCharToMultiByte(CP_ACP,0,p,-1,NULL,0,NULL,NULL);
    m_dest.reset(new char[len]);
    ::WideCharToMultiByte(CP_ACP,0,p,-1,&m_dest[0],len,NULL,NULL);
  };

  code_converter<wchar_t,char>::code_converter(const std::wstring & p)
  {
    int len = ::WideCharToMultiByte(CP_ACP,0,p.c_str(),-1,NULL,0,NULL,NULL);
    m_dest.reset(new char[len]);
    ::WideCharToMultiByte(CP_ACP,0,p.c_str(),-1,&m_dest[0],len,NULL,NULL);
  };

};