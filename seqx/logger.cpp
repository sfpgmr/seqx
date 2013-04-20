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
*  @author S.F. (Satoshi Fujiwara)
*/

#include "stdafx.h"
#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
namespace sf {

  struct logger::impl 
  {
    impl() : m_log_file("sftracker.log",std::ios_base::out | std::ios_base::trunc ){};
    ~impl() {m_log_file.close();}

    void write(const boost::wformat & fmt)
    {
      write(fmt.str());
    };

    void write(const std::wistream & st)
    {
      {   
        //mutex_type::scoped_lock lock_(m_mutex);
        m_log_file << st << std::endl;
      }
      boost::gregorian::date dt = boost::gregorian::day_clock::local_day();

    };

    void write(const std::wstring & mes) 
    {   
      write(mes.c_str());
    }

    void write(const TCHAR * mes) 
    {   
      //mutex_type::scoped_lock lock_(m_mutex);
      m_log_file << mes << std::endl;
      m_log_file.flush();
    }   
  private:
    std::wofstream m_log_file;
    //mutex_type m_mutex;
  };

  logger::logger() : m_impl(new logger::impl())
  {
  }
  void logger::write(const boost::wformat & fmt)
  {
    m_impl->write(fmt);
  };

  void logger::write(const std::wstring & mes) 
  {   
    m_impl->write(mes);
  }

  void logger::write(const TCHAR * mes) 
  {   
    m_impl->write(mes);
  }   

  void logger::write(const std::wistream & st)
  {
    m_impl->write(st);
  }

  logger::~logger()
  {

  }

  void debug_log(const char * file_name,const int line,boost::wformat& fmt)
  {
    OutputDebugString((boost::wformat(_T("%s(%d) %s \n")) % std::wstring(sf::code_converter<char,wchar_t>(file_name)) % line % fmt).str().c_str());
  };

  void debug_log(const char * file_name,const int line,const std::wstring& str)
  {
    OutputDebugString((boost::wformat(_T("%s(%d) %s \n")) % std::wstring(sf::code_converter<char,wchar_t>(file_name)) % line % str).str().c_str());
  };

  void debug_log(const char * file_name,const int line,const char* str)
  {
    OutputDebugString((boost::wformat(_T("%s(%d) %s \n")) % std::wstring(sf::code_converter<char,wchar_t>(file_name)) %  line % sf::code_converter<char,wchar_t>(str)).str().c_str());
  }

  void debug_logW(const char * file_name,const int line,const wchar_t* str)
  {
    OutputDebugString((boost::wformat(_T("%s(%d) %s \n")) % std::wstring(sf::code_converter<char,wchar_t>(file_name)) % line % str).str().c_str());
  };

}
