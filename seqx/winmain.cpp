#include "stdafx.h"
#include "application.h"

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif




//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
/** WinMain */
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	int ret = 0;
  try {
    ret  = sf::application::instance()
    ->execute( hInstance,hPrevInstance,lpCmdLine,nCmdShow);
  } catch(sf::win32_error_exception& e)
  {
    ::MessageBox(nullptr,e.error().data(),L"アプリケーション実行エラー",MB_OK);
    // 強制終了する
    TerminateProcess(NULL,-1);
  }
  return ret;
}
