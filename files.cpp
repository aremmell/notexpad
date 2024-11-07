
//
// files.cpp - Implementation of File I/O and misc for
// NoteXpad v2.0
// Ryan Lederman, ryan@winprog.org
//
#include "files.h"
#include "macros.h"
#include "recentfiles.h"
#include "util.h"

BOOL NoteXpad_WriteFile( HWND hWnd, TCHAR *szPath, BOOL *bModified, BOOL bUnicode, BOOL *bUniFlag )
{
	//
	// NoteXpad_WriteFile
	// Writes the data from the NoteXpad view to disk
	//
	HANDLE hFile          = NULL;
	HANDLE hBuffer        = NULL;
	PVOID  pVoid          = NULL;
	DWORD  dwWrittenBuffer= 0;
	DWORD  dwToWrite      = 0;
	char   *szANSI        = NULL;
	WORD   wUnicodeOrder  = 0xFEFF;

	if( !szPath || !*szPath )
		return FALSE;
	
	//
	// Open file
	//
	hFile = CreateFile( szPath, 
						GENERIC_WRITE, 
						FILE_SHARE_WRITE,
						NULL,
						CREATE_ALWAYS,
						0,
						0 );

	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;


	//
	// Get the memory buffer of the view
	//
	hBuffer = (PVOID)SendMessage( GetDlgItem( hWnd, ID_VIEW ), EM_GETHANDLE, 0, 0 );

	if( !hBuffer )
	{
		CloseHandle( hFile );
		return FALSE;
	}

	//
	// Lock the buffer so we can access it
	//
	pVoid = LocalLock( hBuffer );

	if( !pVoid )
	{
		CloseHandle( hFile );
		return FALSE;
	}

	//
	// Are we saving as UNICODE or ANSI?
	//
	if( bUnicode )
	{
		//
		// UNICODE
		//
		dwToWrite = _tcslen( (TCHAR*)pVoid )*sizeof(TCHAR);

		// Write UNICODE byte order marker
		WriteFile( hFile, &wUnicodeOrder, 2, &dwWrittenBuffer, 0 );

		// Write file data directly from UNICODE edit box
		if( !WriteFile( hFile, pVoid, dwToWrite, &dwWrittenBuffer, 0 ) )
		{
			LocalUnlock( hBuffer );
			CloseHandle( hFile );
			return FALSE;
		}
		*bUniFlag = TRUE;
	}
	else
	{
		// ANSI
		dwToWrite = _tcslen( (TCHAR*)pVoid );

		// Allocate buffer for converted-to-ANSI text
		szANSI = (char*)malloc( dwToWrite + 1 );

		// Make sure allocation didn't fail
		if( !szANSI ) return FALSE;

		// Convert the contents of the UNICODE text box to ANSI
		WideCharToMultiByte( CP_ACP, 0, (PWSTR)pVoid, dwToWrite*2, szANSI, dwToWrite + 1, NULL, NULL );

		// Write the data to disk
		if( !WriteFile( hFile, szANSI, dwToWrite, &dwWrittenBuffer, 0 ) )
		{
			LocalUnlock( hBuffer );
			CloseHandle( hFile );
			return FALSE;
		}

		free( szANSI );
		*bUniFlag = FALSE;
	}
	
	//
	// Free resources
	//
	LocalUnlock( hBuffer );
	CloseHandle( hFile );

	//
	// Set modified flag to false
	//
	*bModified = FALSE;

	//
	// Success!
	//
	return TRUE;
}

BOOL NoteXpad_ReadFile( HWND hWnd, TCHAR *szFilePath, BOOL *bModified, BOOL *bUnicode )
{
	//
	// NoteXpad_ReadFile
	// Reads a file from disk to the NoteXpad view window
	// Big thanks to Josh (O_6@Efnet) for helping me figure out
	// how the hell Notepad managed to load files so fast.
	//

	HANDLE hFile          = 0;
	HLOCAL hOldBuffer     = 0;
	HLOCAL hLocal         = 0;
	PVOID  pVoidBuf       = 0;
	DWORD  dwRead         = 0;
	HWND   hWndView       = 0;
	TCHAR *szBuffer       = 0;
	TCHAR *szPreConvert   = 0;
	int   iUnicode        = 0;
	int   iFileLength     = 0;
	//
	// Open file
	//
	hFile = CreateFile( szFilePath, 
						GENERIC_READ,
						FILE_SHARE_READ,
						NULL,
						OPEN_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						0 );
	
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	//
	// Discover file size
	//
	iFileLength = GetFileSize( hFile, NULL );

	if( iFileLength > 0x7D000 )
	{
		// If this file is larger than 500KB and word wrap is enabled,
		// this could take forever. Prompt user to turn off word wrap
		if( (GetWindowLong( GetDlgItem( hWnd, ID_VIEW ), GWL_STYLE ) & WS_HSCROLL) == FALSE )
			switch( MessageBox( hWnd, T("The file you are attempting to open is larger")
									  T(" than 500 kilobytes, and word wrap is currently enabled.")
									  T(" opening files with word wrap enabled is exponentially")
									  T(" slower than when it is disabled. Would you like to")
									  T(" disable word wrap now?"),
								T("Long Operation Warning - NoteXpad"),
								MB_YESNO | MB_ICONEXCLAMATION  ) )
		{
			case IDYES: ToggleWordwrap( hWnd, FALSE );
		}
	}

	//
	// Clear destination window
	//
	hWndView = GetDlgItem( hWnd, ID_VIEW );
	SetWindowText( hWndView, T("") );

	//
	// Allocate a buffer to copy the contents of the file to
	//
	hLocal = LocalAlloc( LMEM_MOVEABLE, 10 );	// Allocate small buffer

	if( !hLocal )
		return FALSE;

	//
	// Instruct destination window not to repaint until further notice
	//
	PostMessage( hWndView, EM_LIMITTEXT, 0, 0 );
	PostMessage( hWndView, WM_SETREDRAW, FALSE, 0 );

	//
	// Free destination windows' existing buffer
	//
	hOldBuffer = (HLOCAL)SendMessage( hWndView, EM_GETHANDLE, 0, 0 );
	LocalFree( hOldBuffer );

	//
	// Inform destination window of new buffer
	//
	PostMessage( hWndView, EM_SETHANDLE, (WPARAM)hLocal, 0 );	// Set edit box's handle


	//
	// Read file to temporary buffer
	//
	szPreConvert = (TCHAR*)calloc( iFileLength + (sizeof(TCHAR)*2), sizeof(TCHAR) );

	ReadFile( hFile, (void*)szPreConvert, iFileLength, &dwRead, NULL );

	//
	// Find out if file is UNICODE or ANSI
	//
	iUnicode = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE;

	if( IsTextUnicode( szPreConvert, iFileLength, &iUnicode ) )
	{
		// File is already UNICODE, just copy to the edit buffer
		LocalReAlloc( hLocal, iFileLength+(sizeof(TCHAR)*2), LMEM_MOVEABLE );	// Reallocate buffer
		pVoidBuf = LocalLock( hLocal );	// Lock this shiznit
		if( !pVoidBuf ) return FALSE;
		memcpy( pVoidBuf, szPreConvert+1, iFileLength-1 );
		((TCHAR*)pVoidBuf)[iFileLength/sizeof(TCHAR)] = '\0';
		*bUnicode = TRUE;
	}
	else
	{
		// File is ANSI, convert buffer to UNICODE and copy
		LocalReAlloc( hLocal, (iFileLength*2)+sizeof(TCHAR), LMEM_MOVEABLE );	// Reallocate buffer
		pVoidBuf = LocalLock( hLocal );	// Lock this shiznit
		if( !pVoidBuf ) return FALSE;
		MultiByteToWideChar( CP_ACP, 0, (char*)szPreConvert, iFileLength+1, (PWSTR)pVoidBuf, (iFileLength*2)+sizeof(TCHAR) );
		*bUnicode = FALSE;
	}

	// 
	// Unlock buffers
	//
	free( szPreConvert );
	LocalUnlock( hLocal );
	
	//
	// Tell edit box it can repaint now
	//
	PostMessage( hWndView, WM_SETREDRAW, TRUE, 0 );

	//
	// Close the file
	//
	CloseHandle( hFile );

	//
	// Set modified flag to false
	//
	*bModified = FALSE;

	//
	// Update UI
	//
	UpdateUI( hWnd );

	//
	// Add this file to the recent file list
	//
	SetNewRecentFile( hWnd, szFilePath );

	return TRUE;
}

BOOL NoteXpad_OpenFile( HWND hWnd, BOOL *bModified, TCHAR *szFilePath, BOOL *bUnicode )
{
	//
	// NoteXpad_OpenFile
	// Prompts user for selection of file to open.
	//
	OPENFILENAME ofn = {0};
	TCHAR szFile[MAX_PATH+1] = {0};
	static TCHAR *szFilter   = 
							  T("Text Files\0*.txt\0")
							  T("All Files\0*.*\0\0");

	if( !bModified || !szFilePath )
		return FALSE;
	
	//
	// Fill out data structure
	//
	ofn.Flags             = OFN_HIDEREADONLY;
	ofn.hInstance         = GetModuleHandle( NULL );
	ofn.hwndOwner         = hWnd;
	ofn.lpstrCustomFilter = 0;
	ofn.lpstrFile         = szFile;
	ofn.lpstrFileTitle    = 0;
	ofn.lpstrFilter       = szFilter;
	ofn.lpstrInitialDir   = 0;
	ofn.lpstrTitle        = T("NoteXpad 2.0 - Open");
	ofn.lStructSize       = sizeof( OPENFILENAME );
	ofn.nMaxFile          = MAX_PATH;

	//
	// Show dialog
	//
	if( GetOpenFileName( &ofn ) == 0 )
		return TRUE;

	//
	// Copy file path to global path
	//
	SetNewFileName( szFile, szFilePath );

	//
	// Read the file into the NoteXpad view
	//
	if( !NoteXpad_ReadFile( hWnd, szFilePath, bModified, bUnicode ) )
		return FALSE;

	//
	// Success!
	//
	return TRUE;
}

int NoteXpad_SaveFile( HWND hWnd, BOOL *bModified, TCHAR *szFilePath, BOOL *bUnicode )
{
	//
	// NoteXpad_SaveFile
	// Prompts user to select location to write file
	//

	OPENFILENAME ofn         = {0};
	TCHAR szFile[MAX_PATH+1] = {0};
	static TCHAR *szFilter   = 
							  T("Text Files\0*.txt\0")
							  T("UNICODE text files\0*.txt\0")
							  T("All Files\0*.*\0\0");

	if( !bModified || !szFilePath )
		return FALSE;
	
	//
	// Fill out data structure
	//
	ofn.Flags             = 0;
	ofn.hInstance         = GetModuleHandle( NULL );
	ofn.hwndOwner         = hWnd;
	ofn.lpstrCustomFilter = 0;
	ofn.lpstrFile         = szFile;
	ofn.lpstrFileTitle    = 0;
	ofn.lpstrFilter       = szFilter;
	ofn.lpstrInitialDir   = 0;
	ofn.lpstrTitle        = T("NoteXpad 2.0 - Save As");
	ofn.lStructSize       = sizeof( OPENFILENAME );
	ofn.nMaxFile          = MAX_PATH;

	//
	// Show dialog
	//
	if( GetSaveFileName( &ofn ) == 0 )
		return -1;

	//
	// User chose file name
	//
	if( GetFileAttributes( ofn.lpstrFile ) != 0xFFFFFFFF )
	{
		// File exists, prompt to overwrite
		switch( PromptToOverwrite( ofn.lpstrFile ) )
		{
			case IDCANCEL: 
			case IDNO:     return TRUE;
		}
	}
	//
	// File doesn't exist, just write it
	//
	if( !NoteXpad_WriteFile( hWnd, ofn.lpstrFile, bModified, ofn.nFilterIndex == 2 ? TRUE : FALSE, bUnicode ) )
		return FALSE;

	SetNewFileName( ofn.lpstrFile, szFilePath );
	UpdateUI( hWnd );

	return TRUE;
}

int PromptToSave( TCHAR *szFileName )
{
	TCHAR szTemp[1024] = {0};

	wsprintf( szTemp, T("The contents of '%s' have changed. Do you want to save?"), szFileName );

	return MessageBox( GetForegroundWindow(), szTemp, T("NoteXpad 2.0"), MB_APPLMODAL |MB_YESNOCANCEL | MB_ICONEXCLAMATION );
}

int PromptToOverwrite( TCHAR *szFileName )
{
	TCHAR szTemp[1024] = {0};

	wsprintf( szTemp, T("The file '%s' already exists. Overwrite it?"), szFileName );

	return MessageBox( GetForegroundWindow(), szTemp, T("NoteXpad 2.0"), MB_APPLMODAL |MB_YESNOCANCEL | MB_ICONEXCLAMATION);
}

BOOL SetWindowCaption( HWND hWnd, TCHAR *szFileName, BOOL *bModified )
{
	//
	// SetWindowCaption
	// sets new caption of main NoteXpad window
	//
	TCHAR szCaption[1024]    = {0};
	TCHAR szFile[MAX_PATH+1] = {0};

	if( !IsWindow( hWnd ) ) return FALSE;

	if( !szFileName || !bModified ) return FALSE;
		
	if( _tcscmp( szFileName, T("Untitled") ) != 0 )
	{
		if( FileNameFromPath( szFileName, szFile, MAX_PATH+1 ) != 0 )
			return FALSE;
	}
	else
	{
		_tcscpy( szFile, T("Untitled") );
	}
	
	wsprintf( szCaption, T(" %s%s - NoteXpad"), szFile, *bModified ? T("*") : T("") );

	SetWindowText( hWnd, szCaption );

	return TRUE;
}

int FileNameFromPath( TCHAR *szPath, TCHAR *szDestination, int iDestLen )
{
	//
	// FileNameFromPath
	// Returns a file name from a whole path
	//
	int n = 0;

	if( !szPath || !*szPath || !szDestination || !iDestLen )
		return -1;

	for( n = _tcslen( szPath ); n > 0; n-- )
	{
		if( szPath[n] == '\\' )
		{
			memcpy( szDestination, (&szPath[n]+1), _tcslen( szPath )*sizeof(TCHAR) - (n+sizeof(TCHAR)) );
			return 0;
		}
	}

	// Didn't find slash, file must be by itself
	memcpy( szDestination, szPath, _tcslen( szPath )*sizeof(TCHAR) );

	return 0;
}

BOOL SetNewFileName( TCHAR *szNewFile, TCHAR *szOldFile )
{
	if( !szNewFile || !*szNewFile || !szOldFile )
		return FALSE;

	_tcscpy( szOldFile, szNewFile );

	return TRUE;
}