
//
// findreplace.cpp - Implementation of
// Find and Replace functionality
// NoteXpad 2.0, Ryan Lederman, ryan@winprog.org
//
#include "findreplace.h"
#include "macros.h"
#include "options.h"
#include "util.h"
#include "stdlib.h"

HWND CreateFindReplaceDialog( HWND hWndParent )
{
	HWND hWnd = 0;

	if( !IsWindow( hWndParent ) )
		return NULL;

	hWnd = CreateDialog( GetModuleHandle( NULL ), 
						 (LPCTSTR)IDD_FINDREPLACE, 
						 hWndParent, 
						 FRDialogProc );

	CenterWindow( hWndParent, hWnd );

	ShowWindow( hWnd, SW_SHOW );

	return hWnd;
}

BOOL CALLBACK FRDialogProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

	switch( msg )
	{
		case WM_INITDIALOG: return OnInitDialog( hWnd, wParam, lParam );
		case WM_COMMAND:
			switch( LOWORD(wParam) )
			{
				case IDCANCEL: 
					EndDialog( hWnd, 0 );
					break;
				case IDC_FINDNEXT: return FindReplace_OnFind( hWnd, wParam, lParam );
				case IDC_REPLACE:  return FindReplace_OnReplace( hWnd, wParam, lParam );
				case IDC_REPLACEALL: return FindReplace_OnReplaceAll( hWnd, wParam, lParam );
				case IDC_CASESENSITIVE:
					switch( IsDlgButtonChecked( hWnd, IDC_CASESENSITIVE ) )
					{
						case BST_CHECKED:
							SetOptionDWORD( T("CaseSensitive"), 1 );
							break;
						case BST_UNCHECKED:
							SetOptionDWORD( T("CaseSensitive"), 0 );
							break;
					}
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog( hWnd, 0 );
			break;
	}

	return FALSE;
}

BOOL OnInitDialog( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnInitDialog
	// Called when find/replace dialog is created
	//
	TCHAR szToFind[1024] = {0};
	HWND hWndView        = 0;
	DWORD dwSelStart     = 0;
	DWORD dwSelEnd       = 0;
	HLOCAL hEditHandle   = 0;
	PVOID  pEditVoid     = 0;
	DWORD dwCaseSensitive= 0;

	SendMessage( GetDlgItem( hWnd, IDC_FINDFIELD ), EM_LIMITTEXT, 0, 0 );
	SendMessage( GetDlgItem( hWnd, IDC_REPLACEFIELD ), EM_LIMITTEXT, 0, 0 );

	//
	// If there is a current selection in the view, make that what to find
	//
	hWndView = GetDlgItem( GetParent( hWnd ), ID_VIEW );

	SendMessage( hWndView, EM_GETSEL, (WPARAM)&dwSelStart, (LPARAM)&dwSelEnd );
	
	if( dwSelEnd != 0 )
	{
		hEditHandle = (HLOCAL)SendMessage( hWndView, EM_GETHANDLE, 0, 0 );

		if( !hEditHandle )
			return FALSE;

		pEditVoid = LocalLock( hEditHandle );

		if( !pEditVoid )
			return FALSE;

		memcpy( szToFind, (TCHAR*)((TCHAR*)pEditVoid+dwSelStart), (dwSelEnd-dwSelStart) );

		SetWindowText( GetDlgItem( hWnd, IDC_FINDFIELD ), szToFind );

		LocalUnlock( hEditHandle );
	}

	SetFocus( GetDlgItem( hWnd, IDC_FINDFIELD ) );

	//
	// Check case sensitive option
	//
	GetOptionDWORD( T("CaseSensitive"), &dwCaseSensitive );

	if( dwCaseSensitive )
		CheckDlgButton( hWnd, IDC_CASESENSITIVE, BST_CHECKED );

	return TRUE;
}

BOOL FindReplace_OnFind( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// FindReplace_OnFind
	// Called when user presses find button on find/replace dialog
	//
	HLOCAL hEditHandle = 0;
	PVOID  pEditVoid   = 0;
	PVOID  pLast       = 0;
	DWORD  dwSelStart  = 0;
	DWORD  dwSelEnd    = 0;
	DWORD  dwIndex     = 0;
	HWND   hWndView    = 0;
	TCHAR  szToFind[1024] = {0};
	
	//
	// First, let's see if there is even something to search in the view.
	//
	hWndView = GetDlgItem( GetParent( hWnd ), ID_VIEW );
	hEditHandle = (HLOCAL)SendMessage( hWndView, EM_GETHANDLE, 0, 0 );

	if( !hEditHandle ) 
		return FALSE;

	pEditVoid = LocalLock( hEditHandle );

	if( !pEditVoid ) 
		return FALSE;

	if( !*((TCHAR*)pEditVoid) ) {
		LocalUnlock( hEditHandle );
		return FALSE;
	}
	
	//
	// Ok, so there's something in the view
	//
	GetDlgItemText( hWnd, IDC_FINDFIELD, szToFind, 1024 );

	if( _tcslen( szToFind ) <= 0 )
	{
		// Nothing to search for
		SetFocus( GetDlgItem( hWnd, IDC_FINDFIELD ) );
		SendMessage( GetDlgItem( hWnd, IDC_FINDFIELD ), EM_SETSEL, 0, GetWindowTextLength( GetDlgItem( hWnd, IDC_FINDFIELD ) ) );
		LocalUnlock( hEditHandle );
		MessageBeep( MB_OK );
	}

	// We have something to find, let's get crackin
	// First things first, discover address of first instance of search query
	
	SendMessage( hWndView, EM_GETSEL, (WPARAM)&dwSelStart, (LPARAM)&dwSelEnd );

	if( IsDlgButtonChecked( hWnd, IDC_CASESENSITIVE ) == BST_CHECKED )
		pLast = (PVOID)_tcsstr( (TCHAR*)pEditVoid+dwSelEnd, szToFind );
	else
		pLast = (PVOID)StrStrI( (TCHAR*)pEditVoid+dwSelEnd, szToFind );

	if( !pLast )
	{
		// Shiiiit, that query doesn't even exist in the document.
		SetFocus( GetDlgItem( hWnd, IDC_FINDFIELD ) );
		SendMessage( GetDlgItem( hWnd, IDC_FINDFIELD ), EM_SETSEL, 0, GetWindowTextLength( GetDlgItem( hWnd, IDC_FINDFIELD ) ) );
		LocalUnlock( hEditHandle );
		MessageBeep( MB_OK );
		return FALSE;
	}

	dwIndex = ((TCHAR*)pLast - (TCHAR*)pEditVoid);

	SendMessage( hWndView, EM_SETSEL, (WPARAM)dwIndex, (LPARAM)dwIndex+_tcslen( szToFind ) );
	SendMessage( hWndView, EM_SCROLLCARET, 0, 0 ); // Ensures our selection is visible

	LocalUnlock( hEditHandle );

	return FALSE;
}

BOOL FindReplace_OnReplace( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// FindReplace_OnReplace
	// Called when user presses replace button
	//
	TCHAR szToReplace[1024] = {0};
	DWORD dwSelStart        = 0;
	DWORD dwSelEnd          = 0;
	HWND  hWndView          = 0;
	
	hWndView = GetDlgItem( GetParent( hWnd ), ID_VIEW );
	SendMessage( hWndView, EM_GETSEL, (WPARAM)&dwSelStart, (LPARAM)&dwSelEnd );

	if( !dwSelEnd )
	{
		// There is no selection to replace
		SetFocus( GetDlgItem( hWnd, IDC_REPLACEFIELD ) );
		SendMessage( GetDlgItem( hWnd, IDC_REPLACEFIELD ), EM_SETSEL, 0, GetWindowTextLength( GetDlgItem( hWnd, IDC_FINDFIELD ) ) );
		MessageBeep( MB_OK );
		return FALSE;
	}
	
	GetDlgItemText( hWnd, IDC_REPLACEFIELD, szToReplace, 1024 );
	SendMessage( hWndView, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)szToReplace );

	return FALSE;
}

BOOL FindReplace_OnReplaceAll( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// FindReplace_OnReplaceAll
	// Called when user presses replace all button
	//
	TCHAR szToFind[1204]    = {0};
	TCHAR szToReplace[1024] = {0};
	HLOCAL hEditHandle      = 0;
	HLOCAL hNewHandle       = 0;
	HLOCAL pNewVoid         = 0;
	PVOID pEditVoid         = 0;
	HWND  hWndView          = 0;
	TCHAR *szTempBuffer     = 0;
	TCHAR *szTemp           = 0;
	DWORD dwNewBufLen       = 0;
	REPLACESTRUCT rs        = {0};

	GetDlgItemText( hWnd, IDC_FINDFIELD, szToFind, 1024 );
	GetDlgItemText( hWnd, IDC_REPLACEFIELD, szToReplace, 1024 );

	hWndView = GetDlgItem( GetParent( hWnd ), ID_VIEW );

	if( _tcslen( szToFind ) <= 0 ) return FALSE;


	//
	// Allocate new buffer
	//
	hNewHandle = LocalAlloc( LMEM_MOVEABLE, 10 );

	if( !hNewHandle ) return FALSE;

	//
	// Ok, let's get this show on the road
	//
	PostMessage( hWndView, EM_LIMITTEXT, 0, 0 );
	SendMessage( hWndView, WM_SETREDRAW, FALSE, 0 );

	hEditHandle = (HLOCAL)SendMessage( hWndView, EM_GETHANDLE, 0, 0 );
	//
	// Got access to the internal buffer of the edit box
	//

	pEditVoid = LocalLock( hEditHandle );

	if( !pEditVoid ) return FALSE;
	//
	// Do replacment
	//(TCHAR*)pEditVoid, &szTempBuffer, &dwNewBufLen, szToFind, szToReplace
	if( IsDlgButtonChecked( hWnd, IDC_CASESENSITIVE ) == BST_CHECKED )
		rs.bCaseSensitive = TRUE;
	else
		rs.bCaseSensitive = FALSE;

	rs.iDestLen  = &dwNewBufLen;
	rs.szData    = (TCHAR*)pEditVoid;
	rs.szDest    = &szTempBuffer;
	rs.szFind    = szToFind;
	rs.szReplace = szToReplace;

	if( Replace( &rs ) > 0 )
	{
		LocalUnlock( hEditHandle );
		LocalFree( hEditHandle );

		PostMessage( hWndView, EM_SETHANDLE, (WPARAM)hNewHandle, 0 );

		LocalReAlloc( hNewHandle, (dwNewBufLen*sizeof(TCHAR))+sizeof(TCHAR), LMEM_MOVEABLE );

		pNewVoid = LocalLock( hNewHandle );

		memcpy( pNewVoid, szTempBuffer, dwNewBufLen*sizeof(TCHAR) );

		free( szTempBuffer );

		szTemp = (TCHAR*)pNewVoid;
		szTemp[dwNewBufLen] = '\0';
		
		LocalUnlock( hNewHandle );
	}
	PostMessage( hWndView, WM_SETREDRAW, TRUE, 0 );

	return FALSE;
}

int Replace( PREPLACESTRUCT prs )
{
	//
	// Replaces all occurences of szFind with szReplace
	// Assigns *szDest to the address of the newly created buffer
	// and *iDestLen to the number of bytes copied to the new buffer
	//
	TCHAR *szBuffer = 0;
	TCHAR *szFound  = 0;
	DWORD dwCopied  = 0;
	DWORD dwLength  = 0;
	DWORD dwNewLen  = 0;
	DWORD dwBufLen  = 0;
	DWORD dwNum     = 0;
	const TCHAR *szEnd    = 0;
	const TCHAR *szFloater= 0;

	if( !prs ) return -1;

	if( !prs->szData || !*prs->szData || !prs->szFind || !*prs->szFind || !prs->szReplace )
		return -1;

	if( !_tcslen( prs->szFind ) || !_tcslen( prs->szData ) )
		return -1;

	szEnd = prs->szData + _tcslen( prs->szData );
	szFloater = prs->szData;

	if( !szEnd || !szFloater )
		return -1;

	dwLength = _tcslen( prs->szData );

	while( szFloater < szEnd )
	{
		szFloater = _tcsstr( szFloater, prs->szFind );
		
		if( !szFloater )
			break;

		szFloater += _tcslen( prs->szFind );
		
		if( _tcslen( prs->szReplace ) > _tcslen( prs->szFind ) )
			dwLength += (_tcslen( prs->szReplace ) - _tcslen( prs->szFind ));
		else
			dwLength -= (_tcslen( prs->szReplace ) - _tcslen( prs->szFind ));

		dwNum++;
	}

	if( !dwNum )
		return 0;

	szBuffer = (TCHAR*)calloc( dwLength+1, sizeof(TCHAR) );

	if( !szBuffer )
		return -1;

	szFloater = prs->szData;

	while( *szFloater ) {
		if( prs->bCaseSensitive ) {
			szFound = const_cast<TCHAR*>(_tcsstr( szFloater, prs->szFind )); }
		else {
			szFound = StrStrI( szFloater, prs->szFind ); }

		if( szFound ) {
			// String is found, copy up to the beginning of the string
			// to the buffer, and then szReplace, and begin loop again
			// offset at last occurance of szFind+len(szFind)

			// Copy to buffer at current offset from current read point
			// to found string -1
			memcpy( (void*)(szBuffer+dwBufLen), (void*)szFloater, (szFound - szFloater)*sizeof(TCHAR) );

			dwBufLen += (szFound - szFloater);	// Increment offset by how much we just copied
			szFloater += (szFound - szFloater)+ _tcslen( prs->szFind );	// Increment read offset by how much we read + length of szFind

    		memcpy( (void*)(szBuffer+dwBufLen), prs->szReplace, _tcslen( prs->szReplace )*sizeof(TCHAR) ); // Copy 'replaced' word
			dwBufLen += _tcslen( prs->szReplace );

			// Do it all over again
		}
		else
		{
			// Word not found, copy rest of data to buffer
			if( dwBufLen < dwLength )
				memcpy( (void*)(szBuffer+dwBufLen), (void*)szFloater, (dwLength-dwBufLen)*sizeof(TCHAR) );	
			break;
		}
	}

	// Done, return new buffer
	szBuffer[dwLength] = '\0';

	*prs->iDestLen = dwLength;
	*prs->szDest   = szBuffer;

	return dwNum;
}