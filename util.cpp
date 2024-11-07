
//
// util.cpp - Miscellaneous functions
// for NoteXpad v2.0
// Ryan Lederman, ryan@winprog.org
//
#include "util.h"
#include "notexpad.h"
#include "options.h"
#include "macros.h"
#include "stdlib.h"

BOOL ScreenToClientRect( RECT* pRect, HWND hWnd )
{
	POINT p1, p2;
	
	if( pRect == NULL )
		return FALSE;
	if( !IsWindow( hWnd ) )
		return FALSE;

	p1.x = pRect->left;
	p1.y = pRect->top;

	p2.x = pRect->right;
	p2.y = pRect->bottom;

	ScreenToClient( hWnd, &p1 );
	ScreenToClient( hWnd, &p2 );

	pRect->left = p1.x;
	pRect->top  = p1.y;
	pRect->right = p2.x;
	pRect->bottom = p2.y;

	return TRUE;
}

void CenterWindow( HWND hParent, HWND hChild )
{
	RECT pRect;
	RECT cRect;
	
	int x = 0, y = 0;
	int pWidth = 0, pHeight = 0;
	int cWidth = 0, cHeight = 0;

	GetWindowRect( hParent, &pRect );
	GetWindowRect( hChild, &cRect );

	pWidth  = (pRect.right - pRect.left);
	pHeight = (pRect.bottom - pRect.top);
	
	cWidth = (cRect.right - cRect.left);
	cHeight = (cRect.bottom - cRect.top);

	x = (pWidth / 2) - (cWidth / 2) + pRect.left;
	y = (pHeight /2) - (cHeight / 2) + pRect.top;

	SetWindowPos( hChild, NULL, x, y, 0,	
				0, SWP_NOSIZE | SWP_NOZORDER );

}

void ToggleStayontop( HWND hWnd, BOOL bOnTop )
{

	switch( bOnTop )
	{
		case TRUE:
			SendMessage( GetDlgItem( hWnd, ID_TOOLBAR ), TB_CHECKBUTTON, (WPARAM)ID_STAYONTOP, (LPARAM)MAKELONG( TRUE, 0 ) );
			SetWindowPos( hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE );	
			CheckMenuItem( GetMenu( hWnd ), ID_STAYONTOP, MF_CHECKED | MF_BYCOMMAND );
			SetOptionDWORD( T("Stayontop"), 1 );
		break;
		case FALSE:
			SendMessage( GetDlgItem( hWnd, ID_TOOLBAR ), TB_CHECKBUTTON, (WPARAM)ID_STAYONTOP, (LPARAM)MAKELONG( FALSE, 0 ) );
			SetWindowPos( hWnd, HWND_NOTOPMOST, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE );	
			CheckMenuItem( GetMenu( hWnd ), ID_STAYONTOP, MF_UNCHECKED | MF_BYCOMMAND );
			SetOptionDWORD( T("Stayontop"), 0 );
		break;
	}
}

void ToggleWordwrap( HWND hWnd, BOOL bWrap )
{
	RECT rClient = {0};
	RECT rView   = {0};
	RECT rToolbar= {0};
	RECT rStatusBar = {0};
	HWND hWndView = 0;
	HWND hNewView = 0;
	HLOCAL hOldHandle = 0;
	HLOCAL hNewHandle = 0;
	PVOID  pOldVoid   = 0;
	PVOID  pNewVoid   = 0;
	DWORD dwOldLength = 0;

	GetClientRect( hWnd, &rClient );
	GetClientRect( GetDlgItem( hWnd, ID_TOOLBAR ), &rToolbar );
	GetClientRect( GetDlgItem( hWnd, ID_STATUSBAR ), &rStatusBar );

	rView.left   = 0;
	rView.top    = rToolbar.bottom;
	rView.right  = (rClient.right - rClient.left);
	rView.bottom = (rClient.bottom - rClient.top)-(rStatusBar.bottom - rStatusBar.top)-rToolbar.bottom;

	hWndView = GetDlgItem( hWnd, ID_VIEW );

	if( hWndView )
	{
		// An edit exists already (i.e. not first call)
		hOldHandle = (HLOCAL)SendMessage( hWndView, EM_GETHANDLE, 0, 0 );
	}

	switch( bWrap )
	{
		case TRUE:
			CheckMenuItem( GetMenu( hWnd ), ID_WORDWRAP, MF_CHECKED | MF_BYCOMMAND );
			hNewView = CreateViewWindow( hWnd, rView, TRUE );
			SetOptionDWORD( T("WordWrap"), 1 );
		break;
		case FALSE:
			CheckMenuItem( GetMenu( hWnd ), ID_WORDWRAP, MF_UNCHECKED | MF_BYCOMMAND );
			hNewView = CreateViewWindow( hWnd, rView, FALSE );
			SetOptionDWORD( T("WordWrap"), 0 );
		break;
	}

	dwOldLength = GetWindowTextLength( hWndView );

	if( hOldHandle && dwOldLength )
	{
		// Release new edit box's handle
		hNewHandle = (HLOCAL)SendMessage( hNewView, EM_GETHANDLE, 0, 0 );
		LocalFree( hNewHandle );
		// Allocate a new handle
		hNewHandle = LocalAlloc( LMEM_MOVEABLE, 10 );
		// Lock old buffer
		pOldVoid   = LocalLock( hOldHandle );
		// Reallocate new buffer
		LocalReAlloc( hNewHandle, (dwOldLength*sizeof(TCHAR))+sizeof(TCHAR), LMEM_MOVEABLE );
		// Lock new buffer
		pNewVoid   = LocalLock( hNewHandle );
		// Lock painting of new edit
		SendMessage( hNewView, WM_SETREDRAW, FALSE, 0 );
		// Transfer data
		memcpy( pNewVoid, pOldVoid, dwOldLength*sizeof(TCHAR) );
		// Null terminate
		((TCHAR*)pNewVoid)[dwOldLength] = '\0';
		// Unlock old buffer
		LocalUnlock( hOldHandle );
		// Destroy old edit box
		SendMessage( hWndView, WM_CLOSE, 0, 0 );
		// Set new view's handle
		SendMessage( hNewView, EM_SETHANDLE, (WPARAM)hNewHandle, 0 );
		// Allow new edit to repaint
		SendMessage( hNewView, WM_SETREDRAW, TRUE, 0 );
		// Set Focus
		SetFocus( hNewView );
		// Set caret to end
		SendMessage( hNewView, EM_SETSEL, -1, -1 );
	}

	SendMessage( hWndView, WM_CLOSE, 0, 0 );
}

void PrintPageHeader( HDC dc, int width, TCHAR *szFileName, int page )
{
	TCHAR szHeader[MAX_PATH+9] = {0};
	TCHAR szFile[MAX_PATH]     = {0};
	SIZE  s                    = {0};

	if( !szFileName ) return;
	
	FileNameFromPath( szFileName, szFile, MAX_PATH );
	wsprintf( szHeader, T("%s, page %d"), szFile, page );

	GetTextExtentPoint32( dc, szHeader, _tcslen( szHeader ), &s );

	TextOut( dc, (width/2)-(s.cx/2), 0, szHeader, _tcslen( szHeader ) );
}

int SetFileAssociation( TCHAR *szApp, TCHAR *szFileType, int iconNum )
{
	//
	// SetFileAssociation
	// Associates a given file type (extension)
	// with the application specified in szApp.
	//
	
	HKEY hKeyTemp             = 0;
	TCHAR szPath[MAX_PATH]    = {0};
	TCHAR szTemp[MAX_PATH+3]  = {0};
	TCHAR szName[MAX_PATH]    = {0};
	DWORD dwResult            = 0;
	int   i                   = 0;

	if( !szApp || !*szApp || !szFileType || !*szFileType )
		return -1;

	FileNameFromPath( szApp, szName, MAX_PATH );
	for( i = _tcslen( szName ); i > 0; i-- ) {
	 if( szName[i] == '.' ) { szName[i] = '\0'; break; } 
	}

	//
	// HKCR\AppName
	//
	wsprintf( szPath, T("%s"), szName );

	if( RegCreateKeyEx( HKEY_CLASSES_ROOT, 
						szPath, 
						0, 
						0, 
						REG_OPTION_NON_VOLATILE, 
						KEY_ALL_ACCESS, 
						0, 
						&hKeyTemp, 
						&dwResult ) != ERROR_SUCCESS )
		return -2;

	if( RegSetValueEx( hKeyTemp, 
					 NULL,
					 0,
					 REG_SZ, 
					 (BYTE*)T("Text Document"), 
					 13*sizeof(TCHAR) ) != ERROR_SUCCESS )
			return -3;

	//
	// shell\open\command
	//
	memset( szPath, 0, MAX_PATH );
	wsprintf( szPath, T("%s\\shell\\open\\command"), szName );
	RegCloseKey( hKeyTemp );

	if( RegCreateKeyEx( HKEY_CLASSES_ROOT, 
						szPath, 
						0, 
						0, 
						REG_OPTION_NON_VOLATILE, 
						KEY_ALL_ACCESS, 
						0, 
						&hKeyTemp, 
						&dwResult ) != ERROR_SUCCESS )
			return -4;

	wsprintf( szTemp, T("%s %%1"), szApp );

	if( RegSetValueEx( hKeyTemp, 
					 NULL,
					 0,
					 REG_SZ, 
					 (BYTE*)szTemp, 
					 _tcslen( szTemp )*sizeof(TCHAR) ) != ERROR_SUCCESS )
			return -5;

	//
	// DefaultIcon
	//
	memset( szPath, 0, MAX_PATH );
	wsprintf( szPath, T("%s\\DefaultIcon"), szName );
	RegCloseKey( hKeyTemp );

	if( RegCreateKeyEx( HKEY_CLASSES_ROOT, 
						szPath, 
						0, 
						0, 
						REG_OPTION_NON_VOLATILE, 
						KEY_ALL_ACCESS, 
						0, 
						&hKeyTemp, 
						&dwResult ) != ERROR_SUCCESS )
			return -6;

	memset( szTemp, 0, MAX_PATH+3 );
	wsprintf( szTemp, T("%s,%d"), szApp, iconNum );

	if( RegSetValueEx( hKeyTemp, 
					 NULL,
					 0,
					 REG_SZ, 
					 (BYTE*)szTemp, 
					 _tcslen( szTemp )*sizeof(TCHAR) ) != ERROR_SUCCESS )
			return -7;

	//
	// Open HKCR\.extension
	//
	memset( szPath, 0, MAX_PATH );
	wsprintf( szPath, T("%s"), szFileType );
	RegCloseKey( hKeyTemp );

	if( RegCreateKeyEx( HKEY_CLASSES_ROOT, 
						szPath, 
						0, 
						0, 
						REG_OPTION_NON_VOLATILE, 
						KEY_ALL_ACCESS, 
						0, 
						&hKeyTemp, 
						&dwResult ) != ERROR_SUCCESS )
		return -8;



	//
	// Set the app name to (Default)
	//
	if( RegSetValueEx( hKeyTemp,
					 NULL, 
					 0,
					 REG_SZ, 
					 (BYTE*)szName, 
					 _tcslen( szApp )*sizeof(TCHAR) - (4*sizeof(TCHAR)) ) != ERROR_SUCCESS )
		return -9;

	RegCloseKey( hKeyTemp );

	//
	// Open DefaultIcon
	//
	memset( szPath, 0, MAX_PATH );
	wsprintf( szPath, T("%s\\DefaultIcon\\"), szFileType );

	if( RegCreateKeyEx( HKEY_CLASSES_ROOT, 
						szPath, 
						0, 
						0, 
						REG_OPTION_NON_VOLATILE, 
						KEY_ALL_ACCESS, 
						0, 
						&hKeyTemp, 
						&dwResult ) != ERROR_SUCCESS )
		return -10;

	//
	// Set DefaultIcon
	//
	memset( szTemp, 0, MAX_PATH+3 );
	wsprintf( szTemp, T("%s,%d"), szApp, iconNum );
	if( RegSetValueEx( hKeyTemp,
					 NULL,
					 0,
					 REG_SZ,
					 (BYTE*)szTemp,
					 _tcslen( szTemp )*sizeof(TCHAR) ) != ERROR_SUCCESS )
		return -11;

	//
	// shell/open/command/szApp
	//
	RegCloseKey( hKeyTemp );
	memset( szPath, 0, MAX_PATH );
	wsprintf( szPath, T("%s\\shell\\open\\command\\"), szFileType );

	if( RegCreateKeyEx( HKEY_CLASSES_ROOT, 
						szPath, 
						0, 
						0, 
						REG_OPTION_NON_VOLATILE, 
						KEY_ALL_ACCESS, 
						0, 
						&hKeyTemp, 
						&dwResult ) != ERROR_SUCCESS )
		return -12;

	memset( szTemp, 0, MAX_PATH+3 );
	wsprintf( szTemp, T("%s %%1"), szApp );
	if( RegSetValueEx( hKeyTemp, 
					 szName,
					 0,
					 REG_SZ, 
					 (BYTE*)szTemp, 
					 _tcslen( szTemp )*sizeof(TCHAR) ) != ERROR_SUCCESS )
		return -13;

	//
	// SUCCESS!
	//
	RegCloseKey( hKeyTemp );

	MessageBox( GetForegroundWindow(), T("NoteXpad is now associated with .TXT")
									   T(" files on your system. You may need to")
									   T(" restart in order for the icons to change."),
				T("File Association - NoteXpad"), MB_OK | MB_ICONINFORMATION );
	return 0;
}

BOOL RealPathFromStupidPath( TCHAR *szStupidPath, TCHAR *szRealPath )
{
	//
	// RealPathFromStupidPath
	// Converts a file name from stupid form (TEXTFI~1.TXT)
	// to real form (TEXTFILE.TXT)
	//
	GetLongPathName( szStupidPath, szRealPath, MAX_PATH );
	return TRUE;
}
