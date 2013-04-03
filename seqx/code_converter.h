#pragma once


/** @file
 *  @author S.F. (Satoshi Fujiwara)
 *  @brief wchar_t <--> char の変換ユーティリティ
 *  引数として文字列を引き渡す際、変換が必要な場合、下記のようにアダプタ的に使用することを想定している。
 *  std::wstring a(L"abcd");
 *  std::wstring b((sf::code_converter<wcha_t,char>(a)));
 *
 *　後、ATLコード差し替え用のtypedefも用意している。
 *  これは、アダプタとしての利用のみに限定される。
 *  sf::ct2a hoge_(L"abcef"); // 想定していない（動作する場合もあるし、ダメな場合もある）
 */

namespace sf 
{
    /** 雛形 */
    template <typename SrcChar,typename DestChar>
    struct code_converter
    {
    public:
        explicit code_converter(SrcChar* p);
        explicit code_converter(std::basic_string<SrcChar> & p);
        operator DestChar*();
    };

    /** char -> wchar_tへの変換 */
    template <>
    struct code_converter<char,wchar_t>
    {
        explicit code_converter(const char* p);
        explicit code_converter(const std::string& p);

        operator wchar_t*() const {return m_dest.get();}
    private:
        boost::scoped_array<wchar_t> m_dest;
    };

    /** wchar_t -> char */
    template <>
    struct code_converter<wchar_t,char>
    {
        explicit code_converter(const wchar_t* p);
        explicit code_converter(const std::wstring& p);
        operator char* () const {return m_dest.get();}
    private:
        boost::scoped_array<char> m_dest;
    };

    /** ATLコード差し替えのためのエイリアス */
    typedef sf::code_converter<char,wchar_t> ca2w;

    /** ATLコード差し替えのためのエイリアス */
    typedef sf::code_converter<wchar_t,char> cw2a;

    #ifdef _UNICODE

        /** ATLコード差し替えのためのエイリアス */
        typedef sf::code_converter<char,wchar_t> ca2t;
        /** ATLコード差し替えのためのエイリアス */
        typedef sf::code_converter<wchar_t,char> ct2a;

        /** ATLコード差し替えのためのエイリアス */
        inline const wchar_t* ct2w(const wchar_t* p) { return p;};
        /** ATLコード差し替えのためのエイリアス */
        inline const wchar_t* cw2t(const wchar_t* p) { return p;};

    #else

        /** ATLコード差し替えのためのエイリアス */
        inline const char* ct2a(const char* p) { return p;};
        /** ATLコード差し替えのためのエイリアス */
        inline const char* ca2t(const char* p) { return p;};

    #endif
}


