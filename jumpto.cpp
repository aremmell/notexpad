
//
// jumpto.cpp - Implementation of jump to line 
// dialog and associated code
// for NoteXpad 2.0
// Ryan Lederman, ryan@winprog.org
//
#include "jumpto.h"
#include "resource.h"

int CreateJumpToDialog( HWND hWndParent )
{
	return DialogBox( GetModuleHandle( NULL ), (LPCTSTR)IDD_JUMPTOLINE, hWndParent, JumpToDlgProc );
}

BOOL CALLBACK JumpToDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	DWORD dwOldLine = 0;
	DWORD dwReqLine = 0;
	DWORD dwNewLine = 0;
	HWND  hWndView  = 0;

	hWndView = GetDlgItem( GetParent( hWnd ), ID_VIEW );

	switch( msg )
	{
		case WM_CLOSE: EndDialog( hWnd, 0 ); break;
		case WM_INITDIALOG:
			dwOldLine = SendMessage( hWndView, EM_LINEFROMCHAR, -1, 0 )+1;
			SetDlgItemInt( hWnd, IDC_LINENUM, dwOldLine, TRUE );

		case WM_COMMAND:
			switch( LOWORD(wParam) )
			{
			case IDOK:
				dwOldLine = SendMessage( hWndView, EM_LINEFROMCHAR, -1, 0 )+1;
				dwNewLine = GetDlgItemInt( hWnd, IDC_LINENUM, NULL, TRUE );
			
				SendMessage( hWndView, EM_LINESCROLL, 0, dwNewLine-dwOldLine );

				SetFocus( hWndView );
				break;

			case IDCANCEL: EndDialog( hWnd, 0 ); break;
			}
		break;
	}

	return FALSE;
}