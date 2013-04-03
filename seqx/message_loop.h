#pragma once
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
 *  @brief 
 *  @author S.F. (Satoshi Fujiwara)
 */
namespace sf {
	struct run_message_loop
	{
		run_message_loop(){};
		inline WPARAM operator()()
		{
			MSG msg;
			while (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
      return msg.wParam;
		}

		~run_message_loop(void){};
	};

	struct peek_message_loop
	{
		typedef boost::function<void ()> func_type;
		explicit peek_message_loop(func_type func)  {func_ =  func;};
		inline WPARAM operator()()
		{
			MSG msg = {0};
			while( WM_QUIT != msg.message )
			{
				if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
				else
				{
					func_();  // Do some rendering
				}
			}
      return msg.wParam;
		};
	private:
		func_type func_;
	};

  struct dialog_message_loop
	{
		dialog_message_loop(HWND dialog) : dialog_(dialog) {};
		inline WPARAM operator()()
		{
			MSG msg;
			while (GetMessage(&msg, NULL, 0, 0))
			{
        if(IsDialogMessage(dialog_,&msg) == FALSE)
        {
				  TranslateMessage(&msg);
				  DispatchMessage(&msg);
        }
			}
      return msg.wParam;
		}
		~dialog_message_loop(){};
  private:
    HWND dialog_;

	};
}
