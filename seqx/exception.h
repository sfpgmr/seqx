#pragma once

namespace sf
{
///Exception
	class exception : public std::exception
	{
	public:
		explicit exception(const std::wstring& reason)
		{
			m_reason = reason;
		};
		const wchar_t * what() {return m_reason.c_str();};
		const std::wstring& what_str() { return m_reason;};
	protected:
		std::wstring m_reason;
	};

	class win32_error_exception : std::exception 
	{
	public:
		win32_error_exception(uint32_t hr);
		win32_error_exception();

		virtual ~win32_error_exception() {};
		uint32_t hresult() {return hresult_;}
		std::wstring& error() {return error_;}
    static std::wstring get_last_error_str(uint32_t err = ::GetLastError());
  private:
		uint32_t hresult_;
		std::wstring error_;
	};

  template <class Exc = win32_error_exception> struct throw_if_err
  {
    inline void operator()(HRESULT hr) {
      if(hr != S_OK)
      {throw Exc(hr);}
    }
    inline void operator()(bool v) {if(!v){throw Exc();}}
  };

}

#define THROW_IF_ERR(x) sf::throw_if_err<>()(x)
