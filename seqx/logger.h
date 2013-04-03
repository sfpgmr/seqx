#pragma once

#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "singleton.h"

namespace sf {

  struct logger : public singleton<logger>
  {
  public:
    logger();
    ~logger();
    void write(const boost::wformat & fmt);
    void write(const std::wstring & mes);
    void write(const TCHAR * mes);
    void write(const std::wistream & st);
  private:
    struct impl;
    std::shared_ptr<impl> m_impl;
  };

  void debug_out(const char * file_name,const int line,boost::wformat& fmt);
  void debug_out(const char * file_name,const int line,const std::wstring& str);
  void debug_out(const char * file_name,const int line,const char* str);
  void debug_out(const char * file_name,const int line,const wchar_t* str);
}

#define WRITE_LOG(s) \
  sf::logger::instance()->write(boost::wformat(_T("%s %s %6d %s")) % boost::posix_time::second_clock::local_time() % (TCHAR*)sf::ca2t(__FILE__) % __LINE__ % (s))

#ifdef _DEBUG
#define SFTRACE(x) sf::debug_out(__FILE__,__LINE__,(x))
#else 
#define SFTRACE(x) 
#endif

