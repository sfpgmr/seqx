#pragma once

/* http://vision.kuee.kyoto-u.ac.jp/~nob/doc/win32/win32.html#doc1_42 
	より拝借
*/


#ifndef STDX_DSTREAM_H

#define STDX_DSTREAM_H

namespace sf
{

#ifdef _DEBUG
  
// VC++ で STLport だと using std::char_traits; みたいなのが必要かも
template <typename Ch_T, typename Tr_T = std::char_traits<Ch_T> >
class basic_dbg_streambuf: public std::basic_stringbuf<Ch_T, Tr_T>
{
public:
  basic_dbg_streambuf()
    {
#ifndef STDX_DSTREAM_BUFFERING
      setbuf(0,0);
#endif
    }

  virtual ~basic_dbg_streambuf()
    {
      sync();
    }

protected:
  int sync(void)
    {
      dbg_out(str().c_str());
      pbump(static_cast<int>(pbase() - pptr()));
      return 0;
    }

  void dbg_out(const Ch_T*);
};

template <>
inline void basic_dbg_streambuf<char>::dbg_out(const char *str)
{
  ::OutputDebugStringA(str);
}

template <>
inline void basic_dbg_streambuf<wchar_t>::dbg_out(const wchar_t *str)
{
  ::OutputDebugStringW(str);
}

template <typename Ch_T, typename Tr_T = std::char_traits<Ch_T> >
class basic_dbg_ostream: public std::basic_ostream<Ch_T, Tr_T>
{
public:
  basic_dbg_ostream() : std::basic_ostream<Ch_T, Tr_T>(new basic_dbg_streambuf<Ch_T, Tr_T>())
    {
    }

  virtual ~basic_dbg_ostream()
    {
      // flush(); // 不要らしい．http://www.tietew.jp/cppll/archive/607
      delete rdbuf();
    }
};

typedef basic_dbg_streambuf<wchar_t>  wdbg_streambuf;
typedef basic_dbg_ostream<wchar_t> wdstream;

// これを定義しておくと， dout の宣言がいらなくなる．

static wdstream wdout;
#define DOUT(x) wdout << x 
}
#else
#define DOUT(x) 
#define wdout //
//  struct dummy_wdout {
//    //template <typename R>
//    //inline dummy_wdout& operator<<(const R v) {return *this;};
//
//    //template <typename R>
//    //inline dummy_wdout& operator<<(const R* v) {return *this;};
//
//    //template <typename R>
//    //inline dummy_wdout& operator<<(const R& v) {return *this;};
//
//    //template <typename R>
//    //inline dummy_wdout& operator<<(const R&& v) {return *this;};
//
//    ////template <typename R,int N>
//    ////inline dummy_wdout& operator<<(const R(&a)[N]) {return *this;};
//
//  };
//
//  static dummy_wdout wdout;
//}
//
//template <typename L>
//inline sf::dummy_wdout& operator<<(sf::dummy_wdout& p,const L v) {return p;};
//
//template <typename L>
//inline sf::dummy_wdout& operator<<(sf::dummy_wdout& p,const L& v) {return p;};
//
//template <typename L>
//inline sf::dummy_wdout& operator<<(sf::dummy_wdout& p,const L&& v) {return p;};
//
////template <typename L>
////inline sf::dummy_wdout& operator<<(sf::dummy_wdout& p,const L* v) {return p;};
//
//template <typename L,int N>
//inline sf::dummy_wdout& operator<<(sf::dummy_wdout& p,const L(&a)[N]) {return p;};
//
//template <typename L>
//inline L& operator<<(const L v,sf::dummy_wdout& p) {return v;};
//
//template <typename L>
//inline L& operator<<(sf::dummy_wdout& p,L v) {return v;};
//
//
//template <typename L>
//inline sf::dummy_wdout& operator<<(const L v,sf::dummy_wdout& p) {return p;};
//
//template <typename L>
//inline sf::dummy_wdout& operator<<(const L& v,sf::dummy_wdout& p) {return p;};
//
////template <typename L>
////inline sf::dummy_wdout& operator<<(const L* v,sf::dummy_wdout& p) {return p;};
//
//template <typename L>
//inline sf::dummy_wdout& operator<<(const L&& v,sf::dummy_wdout& p) {return p;};
//
//
//template <typename L,int N>
//inline sf::dummy_wdout& operator<<(const L(&a)[N],sf::dummy_wdout& p) {return p;};
////
//template <typename L>
//inline sf::dummy_wdout& operator<<(sf::dummy_wdout& p , const L *v) {return p;};

//
}
#endif
#endif // STDX_DSTREAM_