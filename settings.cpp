
//
// settings.cpp - Implementation of settings dialog and
// associated code
// NoteXpad 2.0
// Ryan Lederman, ryan@winprog.org
//
#include "settings.h"
#include "options.h"
#include "resource.h"
#include "util.h"
#include "commctrl.h"

HWND CreateSettingsDialog( HWND hWndParent )
{
	HWND hWnd = 0;

	hWnd = CreateDialog( GetModuleHandle( NULL ), (LPCTSTR)IDD_SETTINGS, hWndParent, SettingsDlgProc );
	
	if( hWnd )
		ShowWindow( hWnd, SW_SHOW );

	return hWnd;
}

BOOL CALLBACK SettingsDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	TCHAR szAppPath[MAX_PATH] = {0};

	switch( msg )
	{
		case WM_INITDIALOG: return OnInitSettingsDlg( hWnd, wParam, lParam );
		case WM_CLOSE: EndDialog( hWnd, 0 ); break;
		case WM_COMMAND:
			switch( LOWORD(wParam) )
			{
				case IDOK: return OnOK( hWnd, wParam, lParam ); 
				case IDCANCEL: return OnCancel( hWnd, wParam, lParam );
				case IDC_ASSOCIATE:
					switch( MessageBox( hWnd, T("Associating NoteXpad with .TXT files")
										T(" will remove any existing association, which")
										T(" you will have to restore manually if you decide")
										T(" not to use NoteXpad any longer. Are you sure")
										T(" you want to do this?"), T("File association - NoteXpad"),
										MB_YESNO | MB_ICONEXCLAMATION ) )
					{
						case IDYES:
							GetModuleFileName( GetModuleHandle( NULL ), szAppPath, MAX_PATH );
							if( SetFileAssociation( szAppPath, T(".TXT"), 1 ) != 0 )
								MSGBOX_ERROR( T("Failed to set file association!") );
							break;
					}
					break;
			}
		break;
		case WM_HSCROLL:
			if( LOWORD(wParam) == TB_THUMBTRACK )
				SaveSettings( hWnd );
		break;
	}

	return FALSE;
}

BOOL OnInitSettingsDlg( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnInitSettingsDlg
	// Called when dialog is loaded, sets options to current value
	//
	DWORD dwOpacity   = 0;

	GetOptionDWORD( T("Opacity"), &dwOpacity );
	if( dwOpacity == 0 )
		dwOpacity = 255;
	
	SendMessage( GetDlgItem( hWnd, IDC_TRANSPARENCY ), TBM_SETRANGEMAX, FALSE, (LPARAM)255 );
	SendMessage( GetDlgItem( hWnd, IDC_TRANSPARENCY ), TBM_SETPOS, TRUE, (LPARAM)dwOpacity );

	return FALSE;
}

BOOL OnOK( HWND hWnd, WPARAM wParam, LPARAM lParam )
{

	//
	// OnOK
	// executed on OK button press
	//

	SaveSettings( hWnd );
	InvalidateRect( GetParent( hWnd ), NULL, TRUE );
	EndDialog( hWnd, 0 );

	return FALSE;
}

BOOL OnCancel( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnCancel
	// executed on Cancel button press
	// closes settings dialog
	//
	EndDialog( hWnd, 0 );

	return FALSE;
}

BOOL SaveSettings( HWND hWnd )
{
	//
	// SaveSettings
	// Uh, saves the settings.
	//
	BYTE  bOpacity  = 0;

	//
	// Update main window
	//
	bOpacity = (BYTE)SendMessage( GetDlgItem( hWnd, IDC_TRANSPARENCY ), TBM_GETPOS, 0, 0 );

	SetOptionDWORD( T("Opacity"), (DWORD)bOpacity );

	if( bOpacity < 255 )
	{
		SetWindowLong( GetParent( hWnd ), GWL_EXSTYLE, GetWindowLong( GetParent( hWnd ), GWL_EXSTYLE ) | WS_EX_LAYERED );
		SetLayeredWindowAttributes( GetParent( hWnd ), 0, bOpacity, LWA_ALPHA );
	}
	else
	{
		SetLayeredWindowAttributes( GetParent( hWnd ), 0, 255, LWA_ALPHA );
		SetWindowLong( GetParent( hWnd ), GWL_EXSTYLE, GetWindowLong( GetParent( hWnd ), GWL_EXSTYLE ) & ~WS_EX_LAYERED );
		InvalidateRect( GetParent( hWnd ), NULL, TRUE );
	}

	return FALSE;
}
