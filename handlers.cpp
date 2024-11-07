
//
// handlers.cpp - Implementation of message handler functions
// for NoteXpad v2.0
// Ryan Lederman, ryan@winprog.org
//
#include "handlers.h"
#include "files.h"
#include "macros.h"
#include "stdio.h"
#include "options.h"
#include "util.h"
#include "recentfiles.h"
#include "timedate.h"
#include "settings.h"
#include "jumpto.h"
#include "glLogoControl.h" // Josh Hill, O_6@Efnet
#include "hyperlink.h"

extern TCHAR szFilePath[MAX_PATH+1];
extern BOOL  bModified;
extern HBRUSH hBrushView;
extern HFONT  hFontView;
extern BOOL  bUnicode;
extern DIALOGS Dialogs;
CHyperlink link;

LRESULT OnCommand( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	if( HIWORD(wParam) == EN_CHANGE )
	{
		bModified = TRUE;
		UpdateUI( hWnd );
		return 0;
	}

	switch( LOWORD(wParam) )
	{
		case ID_SAVE: return OnSave( hWnd, wParam, lParam );
		case ID_SAVEAS: return OnSaveAs( hWnd, wParam, lParam );
		case ID_OPEN: return OnOpen( hWnd, wParam, lParam );
		case ID_PRINT: return OnPrint( hWnd, wParam, lParam );
		case ID_NEW:  return OnNew( hWnd, wParam, lParam );
		case ID_NEW_WINDOW: return OnNewWindow( hWnd, wParam, lParam );
		case ID_EXIT: return OnClose( hWnd, wParam, lParam );
		case ID_JUMPTO: return OnJumpTo( hWnd, wParam, lParam );
		case ID_FIND: return OnFind( hWnd, wParam, lParam );
		case ID_FORECOLOR: return OnForeColor( hWnd, wParam, lParam );
		case ID_BACKCOLOR: return OnBackColor( hWnd, wParam, lParam );
		case ID_CHANGEFONT: return OnChangeFont( hWnd, wParam, lParam );
		case ID_STAYONTOP: return OnToggleStayontop( hWnd, wParam, lParam );
		case ID_WORDWRAP: return OnToggleWordWrap( hWnd, wParam, lParam );
		case ID_UNDO: return OnUndo( hWnd, wParam, lParam );
		case ID_CUT: return OnCut( hWnd, wParam, lParam );
		case ID_COPY: return OnCopy( hWnd, wParam, lParam );
		case ID_PASTE: return OnPaste( hWnd, wParam, lParam );
		case ID_TAB: return OnTab( hWnd, wParam, lParam );
		case ID_SELECTALL: return OnSelectAll( hWnd, wParam, lParam );
		case ID_FILEPROP: return OnFileProperties( hWnd, wParam, lParam );
		case ID_INSERTTIMEDATE: return OnInsertTimeDate( hWnd, wParam, lParam );
		case ID_SETTINGS: return OnSettings( hWnd, wParam, lParam );
		case ID_ABOUT: return OnAboutDialog( hWnd, wParam, lParam );
		case ID_RECENTFILE1:
		case ID_RECENTFILE2:
		case ID_RECENTFILE3:
		case ID_RECENTFILE4: return OnRecentFile( hWnd, LOWORD(wParam) );
	}
	return 0;
}

LRESULT OnSize( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	int  iSbParts[5] = {0};
	RECT rClient     = {0};
	RECT rToolBar    = {0};
	RECT rStatusBar  = {0};

	GetClientRect( hWnd, &rClient );
	GetClientRect( GetDlgItem( hWnd, ID_STATUSBAR ), &rStatusBar );
	GetClientRect( GetDlgItem( hWnd, ID_TOOLBAR ), &rToolBar );

	iSbParts[4] = ((rClient.right)-20);
	iSbParts[3] = ((rClient.right)-90);
	iSbParts[2] = ((rClient.right)-190);
	iSbParts[1] = ((rClient.right)-290);
	iSbParts[0] = ((rClient.right)-430);

	SendMessage( GetDlgItem( hWnd, ID_STATUSBAR ), SB_SETPARTS, (WPARAM)5, (LPARAM)iSbParts );
	SendMessage( GetDlgItem( hWnd, ID_STATUSBAR ), SB_SETTEXT, (WPARAM) 0 | SBT_NOBORDERS, (LPARAM)"" );
	SendMessage( GetDlgItem( hWnd, ID_STATUSBAR ), WM_SIZE, 0, 0 );
	SendMessage( GetDlgItem( hWnd, ID_TOOLBAR ), TB_AUTOSIZE, 0, 0 );

	MoveWindow( GetDlgItem( hWnd, ID_VIEW ), 
			    0, 
				rToolBar.bottom, 
				(rClient.right - rClient.left),
				(rClient.bottom - rClient.top)-(rStatusBar.bottom - rStatusBar.top) - rToolBar.bottom,
				TRUE );

	return 0;
}

LRESULT OnNotify( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPNMHDR		 nm   = NULL;
	LPNMTBHOTITEM phi = NULL;
	TCHAR szText[512] = {0};
	TCHAR *pPrompt    = NULL;

	nm = (LPNMHDR)lParam;
	
	if( nm->code == TBN_HOTITEMCHANGE )
	{
		phi = (LPNMTBHOTITEM)lParam;

		if( phi->dwFlags & HICF_MOUSE )
		{
			if( !LoadString( GetModuleHandle( NULL ), phi->idNew, szText, 512 ) )
			{
				SendMessage( GetDlgItem( hWnd, ID_STATUSBAR ), SB_SETTEXT, (WPARAM) 0 | SBT_NOBORDERS, (LPARAM)"" );
				return 0;
			}
			pPrompt = _tcsstr( szText, T("\n" ) );

			if( !pPrompt )
				return 0;

			*pPrompt = 0;
			SendMessage( GetDlgItem( hWnd, ID_STATUSBAR ), SB_SETTEXT, (WPARAM) 0 | SBT_NOBORDERS, (LPARAM)szText );
		}
	}
	return 0;
}	

LRESULT OnMenuSelect( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	TCHAR szText[512]= {0};
	TCHAR *pPrompt   = NULL;

	if( !LoadString( GetModuleHandle( NULL ), LOWORD(wParam), szText, 512 ) )
	{
		SendMessage( GetDlgItem( hWnd, ID_STATUSBAR ), SB_SETTEXT, (WPARAM) 0 | SBT_NOBORDERS, (LPARAM)"" );
		return 0;
	}
	
	pPrompt = _tcsstr( szText, T("\n" ) );

	if( !pPrompt )
		return 0;

	*pPrompt = 0;

	SendMessage( GetDlgItem( hWnd, ID_STATUSBAR ), SB_SETTEXT, (WPARAM) 0 | SBT_NOBORDERS, (LPARAM)szText );

	return 0;
}

LRESULT OnNew( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnNew - called when user creates a new document
	//

	//
	// Check if existing document was modified
	// and prompt to save if so
	//
	if( bModified )
	{
		switch( PromptToSave( szFilePath ) )
		{
			case IDYES: 
				OnSave( hWnd, wParam, lParam );
				break;
			case IDCANCEL: return 0;
		}
	}
	//
	// "Create" new document
	//
	SetNewFileName( T("Untitled"), szFilePath );
	SetWindowText( GetDlgItem( hWnd, ID_VIEW ), T("") );
	bModified = FALSE;
	bUnicode  = FALSE;
	UpdateUI( hWnd );

	return 0;
}

LRESULT OnNewWindow( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnNewWindow
	// Executed when user pressed New Window menu, or Ctrl+W
	// Creates a new NoteXpad process.
	//
	TCHAR szFileName[MAX_PATH] = {0};

	GetModuleFileName( GetModuleHandle( NULL ), szFileName, MAX_PATH );
	if( (int)ShellExecute( hWnd, T("open"), szFileName, NULL, NULL, SW_SHOWNORMAL ) <=32 )
		MSGBOX_ERROR( T("Failed to start a new NoteXpad process!") );

	return 0;
}

LRESULT OnSave( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	int iSaveVal = 0;

	if( _tcscmp( szFilePath, T("Untitled") ) == 0 )
	{
		// Need to Save As
		if( (iSaveVal = NoteXpad_SaveFile( hWnd, &bModified, szFilePath, &bUnicode )) == FALSE ) {
			MSGBOX_ERROR( T("Failed to save file!") );
			UpdateUI( hWnd );
			return 0; }
		if( iSaveVal == -1 )
			return -1;
	}
	else
	{
		// Write file
		if( !NoteXpad_WriteFile( hWnd, szFilePath, &bModified, bUnicode, &bUnicode ) )
			MSGBOX_ERROR( T("Failed to save file!") );
		UpdateUI( hWnd );
	}

	return 0;
}

LRESULT OnSaveAs( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnSaveAs
	// called when user attempts to save through save as
	// menu
	if( !NoteXpad_SaveFile( hWnd, &bModified, szFilePath, &bUnicode ) )
		MSGBOX_ERROR( T("Failed to save file!") );

  // Saved the file; add it to the MRU.
  if (FALSE == SetNewRecentFile( hWnd, szFilePath )) {
    MSGBOX_ERROR( T("Failed to add to MRU!"));
  }

	return 0;
}

LRESULT OnOpen( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnOpen
	// called when user chooses to open a new file
	//

	//
	// First, check if current file has been modified, and
	// prompt if so
	//
	if( bModified )
	{
		// Has been modified
		switch( PromptToSave( szFilePath ) )
		{
			case IDYES:
			OnSave( hWnd, wParam, lParam );
			break;
			case IDCANCEL: return 0;
		}
	}

	if( !NoteXpad_OpenFile( hWnd, &bModified, szFilePath, &bUnicode ) )
	{
		MSGBOX_ERROR( T("Could not open that file!") );
		return 0;
	}
	return 0;
}

LRESULT OnPrint( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnPrint
	// prints the current document on the default printer
	//
	PRINTER_INFO_4 *pInfo4 = NULL;
	DWORD  dwNeeded        = 0;
	DWORD  dwReturned      = 0;
	DWORD  dwLinesPerPage  = 0;
	DWORD  dwPageX         = 0;
	DWORD  dwPageY         = 0;
	DWORD  dwCharsPerLine  = 0;
	DWORD  n               = 0;
	DWORD  i               = 0;
	DWORD  dwCurPage       = 0;
	DWORD  dwCurLine       = 0;
	DWORD  dwEOL           = 0;
	DWORD  dwSubLines      = 0;
	TCHAR *szLineBuf       = NULL;
	TCHAR  *szEditBuf      = NULL;
	TCHAR  *szFloater      = NULL;
	HLOCAL hEditHandle     = 0;
	PVOID  pEditVoid       = 0;
	HDC    dc              = 0;
	DOCINFO di             = {0};
	TEXTMETRIC         tm  = {0};

	EnumPrinters( PRINTER_ENUM_LOCAL, NULL, 4, NULL, 0, &dwNeeded, &dwReturned );

	pInfo4 = (PRINTER_INFO_4*)malloc( dwNeeded );

	if( !pInfo4 )
		MSGBOX_ERROR( T("Failed to print!") );

	EnumPrinters( PRINTER_ENUM_LOCAL, NULL, 4, (PBYTE)pInfo4, dwNeeded, &dwNeeded, &dwReturned );

	dc = CreateDC( NULL, pInfo4->pPrinterName, NULL, NULL );

	free( pInfo4 );

	if( !dc ) {
		MSGBOX_ERROR( T("Failed to print!") );
		return 0;
	}

	di.cbSize = sizeof( di );
	di.lpszDocName = szFilePath;

	//
	// Draw on printer DC
	//

	// Get dimensions of font and printer DC
	GetTextMetrics( dc, &tm );
	
	dwPageX = GetDeviceCaps( dc, HORZRES );
	dwPageY = GetDeviceCaps( dc, VERTRES );

	// Calculate how many lines will fit on one page
	dwLinesPerPage = (dwPageY/tm.tmHeight+tm.tmExternalLeading)-2;

	// How many chars per line
	dwCharsPerLine = (dwPageX / tm.tmAveCharWidth);

	hEditHandle = (HLOCAL)SendMessage( GetDlgItem( hWnd, ID_VIEW ), EM_GETHANDLE, 0, 0 );

	pEditVoid   = LocalLock( hEditHandle );

	if( !pEditVoid )
		MSGBOX_ERROR( T("Failed to print!") );

	StartDoc( dc, &di );

	szEditBuf = (TCHAR*)pEditVoid;
	szLineBuf =  szEditBuf;

	StartPage( dc );
	
	dwCurPage = 1;
	dwCurLine = 2;
	PrintPageHeader( dc, dwPageX, szFilePath, dwCurPage );

	do	// While there are newlines
	{	
		szFloater = _tcsstr( szLineBuf, T("\r\n") );

		if( !szFloater ) {
			szFloater = szLineBuf;
			dwEOL = _tcslen( szLineBuf );
		}
		else {
			dwEOL = (szFloater - szLineBuf);
		}
		
		if( dwEOL > dwCharsPerLine ) {		// This line is too long to fit on the paper,
											// so we need to break it up into smaller lines
			dwSubLines = dwEOL/dwCharsPerLine;	// Find out how many lines it will need to be broken into

			for( i = 0; i < dwSubLines; i++ ) {
				if( dwCurLine >= dwLinesPerPage-1 ) {
					EndPage( dc );
					StartPage( dc );
					dwCurPage++;
					PrintPageHeader( dc, dwPageX, szFilePath, dwCurPage );
					dwCurLine = 2; }
				while( *szLineBuf == ' ' ) { szLineBuf++; }
				TextOut( dc, 0, (tm.tmHeight+tm.tmExternalLeading)*dwCurLine, szLineBuf, dwCharsPerLine );
				dwCurLine++;
				szLineBuf+= dwCharsPerLine;
			}
			
		}
		else								// This line will fit
		{
			TextOut( dc, 0, (tm.tmHeight+tm.tmExternalLeading)*dwCurLine, szLineBuf, dwEOL );
			szLineBuf+= dwEOL+2;
			dwCurLine++;
		}
	
		if( dwCurLine >= dwLinesPerPage-1 ) {
			EndPage( dc );
			StartPage( dc );
			dwCurPage++;
			PrintPageHeader( dc, dwPageX, szFilePath, dwCurPage );
			dwCurLine = 2; }

	} while( _tcslen( szLineBuf ) );

	LocalUnlock( hEditHandle );

	EndDoc( dc );

	DeleteDC( dc );

	return 0;
}

LRESULT OnDropFile( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnDropFile - WM_DROPFILES handler
	//
	HDROP hDrop              = (HDROP)wParam;
	TCHAR szFile[MAX_PATH+1] = {0};
	int   iFile              = 0;

	//
	// First, if current file has changes, prompt user to save
	//
	if( bModified )
	{
		switch( PromptToSave( szFilePath ) )
		{
			case IDYES: OnSave( hWnd, wParam, lParam ); break;
			case IDCANCEL: return 0;
		}
	}

	//
	// Find out if too many files were dropped
	//
	iFile = DragQueryFile( hDrop, 0xFFFFFFFF, 0, 0 );

	if( iFile > 1 )
	{
		DragFinish( hDrop );
		MSGBOX_ERROR( T("You can only drop one file at a time into NoteXpad.") );
		return 0;
	}

	//
	// Load dropped file
	//
	DragQueryFile( hDrop, 0, szFile, MAX_PATH+1 );

	if( !NoteXpad_ReadFile( hWnd, szFile, &bModified, &bUnicode ) )
		MSGBOX_ERROR( T("Failed to open that file!") );

	SetNewFileName( szFile, szFilePath );
	UpdateUI( hWnd );

	DragFinish( hDrop );

	return 0;
}

LRESULT OnTimer( HWND hWnd, WPARAM wParam, LPARAM lParam, HICON hIconSaved, HICON hIconUnsaved )
{
	//
	// OnTimer - WM_TIMER handler
	//
	UINT iLineNum  = 0;
	UINT iColNum   = 0;
	UINT iBeginSel = 0;
	UINT iEndSel   = 0;
	UINT iLen      = 0;
	float fFloat   = 0;
	HWND hWndView  = 0;
	HWND hWndStat  = 0;
	TCHAR szPane1[256]  = {0};
	TCHAR szPane2[256]  = {0};

	hWndView = GetDlgItem( hWnd, ID_VIEW );
	hWndStat = GetDlgItem( hWnd, ID_STATUSBAR );

	//
	// Line and column (pane 1)
	//
	SendMessage( hWndView, EM_GETSEL, (WPARAM)&iBeginSel, (LPARAM)&iEndSel );

	iLineNum = SendMessage( hWndView, EM_LINEFROMCHAR, iBeginSel, 0 ) + 1;
	iColNum  = SendMessage( hWndView, EM_LINEINDEX, -1, 0 );

	iColNum = iBeginSel - iColNum +1;

	if( iEndSel - iBeginSel == 0 )
	{
		wsprintf( szPane1, T("\tLn: %d / Col: %d"), iLineNum, iColNum );
		SendMessage( hWndStat, SB_SETTEXT, (WPARAM)1 | 0, (LPARAM)szPane1 );
	}
	else
	{
		SendMessage( hWndStat, SB_SETTEXT, (WPARAM)1 | 0, (LPARAM)T("\t[Selection]") );
	}

	//
	// File size (pane2)
	//
	iLen = GetWindowTextLength( hWndView );

	if( !iLen )
	{
		// That was easy, nothing to calculate
		SendMessage( hWndStat, SB_SETTEXT, (WPARAM)2 | 0, (LPARAM)T("\t(Empty)") );
	}
	else
	{
		// Calculate bytes/KB/MB/GB (just kidding about the GB)
		
		if( iLen == 1 )
		{
			wsprintf( szPane2, T("\t1 char") );
		}
		if( iLen > 1 && iLen <= 1024 )
		{
			// Bytes
			wsprintf( szPane2, T("\t%d chars"), iLen );
		}
		if( iLen > 1024 && iLen < (1024*1024) )
		{
			// Kilobytes
			fFloat = (iLen/1024.0f);
			swprintf( szPane2, T("\t%2.2f KB"), fFloat );

		}
		if( iLen >= (1024.0f*1024.0f) )
		{
			// Megabytes
			fFloat = (iLen/(1024.0f*1024.0f));
			swprintf( szPane2, T("\t%2.2f MB"), fFloat );
		}
		SendMessage( hWndStat, SB_SETTEXT, (WPARAM)2 | 0, (LPARAM)szPane2 );
	}
	

	//
	// Saved/Unsaved (pane3)
	//
	switch( bModified )
	{
		case TRUE:
			SendMessage( hWndStat, SB_SETTEXT, (WPARAM)3 | 0, (LPARAM)T("\t[UNSAVED]") );
			SendMessage( hWndStat, SB_SETICON, (WPARAM)3, (LPARAM)hIconUnsaved );
			break;
		case FALSE:			
			SendMessage( hWndStat, SB_SETTEXT, (WPARAM)3 | 0, (LPARAM)T("\t[SAVED]") );
			SendMessage( hWndStat, SB_SETICON, (WPARAM)3, (LPARAM)hIconSaved );
			break;
	}

	//
	// UNICODE/ANSI (pane4)
	//
	switch( bUnicode )
	{
		case TRUE:
			SendMessage( hWndStat, SB_SETTEXT, (WPARAM)4 | 0, (LPARAM)T("\tUNICODE") );
		break;
		case FALSE:
			SendMessage( hWndStat, SB_SETTEXT, (WPARAM)4 | 0, (LPARAM)T("\tANSI") );
		break;
	}

	return 0;
}

LRESULT OnFind( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnFind - Executed when user presses find/replace
	//
  HWND hDlg = NULL;

	if( !(hDlg = CreateFindReplaceDialog( hWnd )) )
		MSGBOX_ERROR( T("Failed to create find/replace dialog!") );
  else
    Dialogs.SetDlg(hDlg, DIALOGS::Type::FIND);

	return 0;
}

LRESULT OnJumpTo( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnJumpTo
	// called when user clicks jump to line
	//
	if( CreateJumpToDialog( hWnd ) == -1 )
		MSGBOX_ERROR( T("Failed to create jump to dialog!") );

	return 0;
}

LRESULT OnInsertTimeDate( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnInsertTimeDate
	// creates time date insertion dialog
	//
  HWND hDlg = NULL;

	if( !(hDlg = CreateTimeDateDialog( hWnd )) )
		MSGBOX_ERROR( T("Failed to create time/date insertion dialog!") );
  else
    Dialogs.SetDlg(hDlg, DIALOGS::Type::TIME);

	return 0;
}

LRESULT OnForeColor( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnForeColor
	// executed when user chooses to change the forecolor of the view
	//
	CHOOSECOLOR cc = {0};
	COLORREF    cr = {0};
	static COLORREF  cust[16] = {0};

	GetOptionBinary( T("ForeColor"), &cr, sizeof( cr ) );

	cc.lStructSize  = sizeof( cc );
	cc.hwndOwner    = hWnd;
	cc.hInstance    = NULL;
	cc.rgbResult    = cr;
	cc.lpCustColors = cust;
	cc.Flags        = CC_FULLOPEN | CC_RGBINIT;

	if( !ChooseColor( &cc ) )
		return 0;

	SetOptionBinary( T("ForeColor"), &cc.rgbResult, sizeof( cc.rgbResult ) );

	InvalidateRect( GetDlgItem( hWnd, ID_VIEW ), NULL, TRUE );

	return 0;
}

LRESULT OnBackColor( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnBackColor
	// executed when user chooses to change the background color of the view
	//
	CHOOSECOLOR cc = {0};
	COLORREF    cr = {0};
	static COLORREF  cust[16] = {0};

	GetOptionBinary( T("BackColor"), &cr, sizeof( cr ) );

	cc.lStructSize  = sizeof( cc );
	cc.hwndOwner    = hWnd;
	cc.hInstance    = NULL;
	cc.rgbResult    = cr;
	cc.lpCustColors = cust;
	cc.Flags        = CC_FULLOPEN | CC_RGBINIT;

	if( !ChooseColor( &cc ) )
		return 0;

	SetOptionBinary( T("BackColor"), &cc.rgbResult, sizeof( cc.rgbResult ) );

	DeleteObject( hBrushView );
	hBrushView = CreateSolidBrush( cc.rgbResult );

	InvalidateRect( GetDlgItem( hWnd, ID_VIEW ), NULL, TRUE );

	return 0;
}

LRESULT OnChangeFont( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnChangeFont
	// executed when user changes font used for view
	//
	CHOOSEFONT cf = {0};
	LOGFONT    lf = {0};

	GetObject( hFontView, sizeof( lf ), &lf );

	cf.lStructSize = sizeof( cf );
	cf.hwndOwner   = hWnd;
	cf.lpLogFont   = &lf;
	cf.Flags       = CF_SCREENFONTS | CF_NOVERTFONTS | CF_INITTOLOGFONTSTRUCT;
	cf.nFontType   = REGULAR_FONTTYPE;

	if( !ChooseFont( &cf ) )
		return 0;

	SetOptionBinary( T("Font"), &lf, sizeof( lf ) );
	
	DeleteObject( hFontView );
	hFontView = CreateFontIndirect( &lf );

	SendMessage( GetDlgItem( hWnd, ID_VIEW ), WM_SETFONT, (WPARAM)hFontView, (LPARAM)TRUE );

	return 0;
}

LRESULT OnToggleStayontop( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnToggleStayontop
	// called when user toggles app between stayontop and regular
	// modes
	//
	DWORD dwStayontop = 0;

	GetOptionDWORD( T("Stayontop"), &dwStayontop );

	if( dwStayontop )
		ToggleStayontop( hWnd, FALSE );
	else
		ToggleStayontop( hWnd, TRUE );

	return 0;
}

LRESULT OnToggleWordWrap( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnToggleWordWrap
	// toggles view between wrapping/non wrapping mode
	//
	DWORD dwWrap = 0;

	GetOptionDWORD( T("WordWrap"), &dwWrap );

	if( dwWrap )
		ToggleWordwrap( hWnd, FALSE );
	else
		ToggleWordwrap( hWnd, TRUE );
	
	return 0;
}

LRESULT OnUndo( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnUndo
	// executed when user presses undo menu or Ctrl+Z
	//
	SendMessage( GetDlgItem( hWnd, ID_VIEW ), WM_UNDO, 0, 0 );
	return 0;
}

LRESULT OnCut( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnCut
	// called when user presses Ctrl+X, or chooses the
	// cut command from the toolbar or menu
	//
	SendMessage( GetDlgItem( hWnd, ID_VIEW ), WM_CUT, 0, 0 );
	
	return 0;
}

LRESULT OnCopy( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnCopy
	// called when Ctrl+C is pressed, or the user
	// chooses the copy toolbar button or menu item
	//
	SendMessage( GetDlgItem( hWnd, ID_VIEW ), WM_COPY, 0, 0 );

	return 0;
}

LRESULT OnPaste( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnPaste
	// called when Ctrl+V is pressed, or the user
	// chooses the paste toolbar button or menu item
	//
	SendMessage( GetDlgItem( hWnd, ID_VIEW ), WM_PASTE, wParam, lParam );

	return 0;
}

LRESULT OnTab( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnTab
	// Hack to make the tab key actually work
	//
	SendMessage( GetDlgItem( hWnd, ID_VIEW ), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)T("\t") );

	return 0;
}

LRESULT OnSelectAll( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnSelectAll
	// called when user clicks select all menu or presses
	// Ctrl+A
	//
	SendMessage( GetDlgItem( hWnd, ID_VIEW ), 
				 EM_SETSEL, 
				 0, 
				 GetWindowTextLength( GetDlgItem( hWnd, ID_VIEW ) ) );

	return 0;
}

LRESULT OnFileProperties( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnFileProperties
	// called when user wants to display properties for current file
	//
	SHELLEXECUTEINFO sei = {0};

	sei.cbSize = sizeof( sei );
	sei.fMask  = SEE_MASK_INVOKEIDLIST;
	sei.hwnd   = hWnd;
	sei.lpFile = szFilePath;
	sei.lpVerb = T("properties");

	if( !ShellExecuteEx( &sei ) )
		MSGBOX_ERROR( T("Failed to invoke file properties dialog!") );

	return 0;
}

LRESULT OnCtlColorEdit( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnCtlColorEdit
	// called when Windows needs to paint the view window
	//
	COLORREF rgbFore = {0};
	COLORREF rgbBack = {0};

	GetOptionBinary( T("ForeColor"), &rgbFore, sizeof( rgbFore ) );

	if( !GetOptionBinary( T("BackColor"), &rgbBack, sizeof( rgbBack ) ) )
		rgbBack = GetSysColor( COLOR_WINDOW );
			
	SetTextColor( (HDC)wParam, rgbFore );
	SetBkColor( (HDC)wParam, rgbBack );

	return (LRESULT)hBrushView;
}

LRESULT OnSettings( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnSettings
	// invokes the settings dialog
	//
  HWND hDlg = NULL;

	if( !(hDlg = CreateSettingsDialog( hWnd )) )
		MSGBOX_ERROR( T("Failed to show settings dialog!") );
  else
    Dialogs.SetDlg(hDlg, DIALOGS::Type::SETTINGS);


	return 0;
}

LRESULT OnCreate( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnCreate
	// called when main window is created
	//
	WINDOWPLACEMENT wp = {0};
	LOGFONT         lf = {0};
	DWORD  dwStayontop = 0;
	DWORD  dwOpacity   = 0;
	DWORD  dwWrap      = 0;

	if( !GetOptionBinary( T("WindowPlacement"), &wp, sizeof( wp ) ) )
		CenterWindow( GetDesktopWindow(), hWnd );
	else
		SetWindowPos( hWnd, 
					  NULL, 
					  wp.rcNormalPosition.left, 
					  wp.rcNormalPosition.top,
					  wp.rcNormalPosition.right-wp.rcNormalPosition.left,
					  wp.rcNormalPosition.bottom-wp.rcNormalPosition.top,
					  SWP_NOZORDER );

	if( !GetOptionBinary( T("Font"), &lf, sizeof( lf ) ) )
	{
		hFontView = CreateFont( 14,
						0,
						0,
						0,
						FW_NORMAL,
						0,
						0,
						0,
						DEFAULT_CHARSET,
						OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY,
						DEFAULT_PITCH, 
						T("Arial") );
	}
	else
	{
		hFontView = CreateFontIndirect( &lf );
	}

	SendMessage( GetDlgItem( hWnd, ID_VIEW ), WM_SETFONT, (WPARAM)hFontView, (LPARAM)TRUE );
	
	//
	// Stayontop?
	//
	GetOptionDWORD( T("Stayontop"), &dwStayontop );

	if( dwStayontop )
		ToggleStayontop( hWnd, TRUE );

	//
	// Recent file list
	//
	LoadRecentFiles( hWnd );

	//
	// Transparency!
	//
	GetOptionDWORD( T("Opacity"), &dwOpacity );

	if( dwOpacity < 255 && dwOpacity > 0 )
	{
		if( SetLayeredWindowAttributes( hWnd, 0, (BYTE)dwOpacity, LWA_ALPHA ) == 0 )
			MSGBOX_ERROR( T("Failed to set transparency for NoteXpad!!!") );
	}
	else // Take off WS_EX_LAYERED
		SetWindowLong( hWnd, GWL_EXSTYLE, GetWindowLong( hWnd, GWL_EXSTYLE ) & ~WS_EX_LAYERED );

	//
	// Other initialization
	//
	SendMessage( GetDlgItem( hWnd, ID_VIEW ), EM_SETLIMITTEXT, 0, 0 );
	SendMessage( hWnd, WM_SIZE, 0, 0 );
	SetFocus( GetDlgItem( hWnd, ID_VIEW ) );

	SetTimer( hWnd, 1, 200, NULL );
	return 0;
}

LRESULT OnClose( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnClose
	// called when window is destroyed/application is exited
	//
	WINDOWPLACEMENT wp = {0};
	LOGFONT         lf = {0};
	//
	// Save settings
	//

	// Window placement/size
	GetWindowPlacement( hWnd, &wp );
	SetOptionBinary( T("WindowPlacement"), &wp, sizeof( wp ) );

	// Font
	GetObject( hFontView, sizeof( lf ), &lf );
	SetOptionBinary( T("Font"), &lf, sizeof( lf ) );

	if( bModified )
	{
		switch( PromptToSave( szFilePath ) )
		{
			case IDYES: 
				if( OnSave( hWnd, wParam, lParam ) == -1 ) return 0; 
				else
					break;
			case IDCANCEL: return 0;
		}
	}

	PostQuitMessage( 0 );

	return 0;
}

LRESULT OnRecentFile( HWND hWnd, UINT id )
{
	//
	// OnRecentFile
	// Called when user chooses a recent file
	// from the menu
	//
	DWORD dwIndex = 0;
	DWORD dwCount = 0;
	TCHAR szPath[MAX_PATH+1] = {0};
	TCHAR szName[MAX_PATH+1] = {0};

	switch( id )
	{
		case ID_RECENTFILE1:
			dwIndex = 0;
			break;
		case ID_RECENTFILE2:
			dwIndex = 1;
			break;
		case ID_RECENTFILE3:
			dwIndex = 2;
			break;
		case ID_RECENTFILE4:
			dwIndex = 3;
			break;
	}

	while( EnumKeys( szPath, szName, MAX_PATH+1, MAX_PATH+1 ) ) {
		if( dwCount == dwIndex )
		{
			if( bModified )
			{
				switch( PromptToSave( szFilePath ) )
				{
					case IDYES:
					OnSave( hWnd, 0, 0 );
					break;
					case IDCANCEL: return 0;
				}
			}
			if( !NoteXpad_ReadFile( hWnd, szPath, &bModified, &bUnicode ) )
				MSGBOX_ERROR( T("Failed to open file!") );
			SetNewFileName( szPath, szFilePath );
			UpdateUI( hWnd );
			break;
		}
		memset( szPath, 0, MAX_PATH+1 );
		memset( szName, 0, MAX_PATH+1 );
		dwCount++;
	}
			
	return 0;
}

LRESULT OnAboutDialog( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnAboutDialog
	// Displays the about dialog!
	//
	glLogoControlInit();	// Josh Hill, O_6@Efnet
	if( DialogBox( GetModuleHandle( NULL ), (LPCTSTR)IDD_ABOUT, hWnd, AboutProc ) == -1 )
		MSGBOX_ERROR( T("Failed to create dialog!") );

	return 0;
}

BOOL CALLBACK AboutProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	HYPERLINKSTRUCT hs = {0};

	switch( msg )
	{
		case WM_INITDIALOG: 
      {
			// Create hyperlink control
			hs.bBold      = false;
			hs.bUnderline = false;
			hs.bUseBg     = true;
			hs.bUseCursor = false;
			hs.clrBack    = GetSysColor( COLOR_3DFACE );
			hs.clrHilite  = RGB(255,0,0);
			hs.clrText    = RGB(0,0,255);
			hs.coordX     = 11;
			hs.coordY     = 115;
			hs.ctlID      = 0;
			hs.fontsize   = 12;
			hs.height     = 20;
			hs.hWndParent = hWnd;
			hs.pFn        = OnAboutLink;
			hs.szCaption  = T("Click here to download source code");
			hs.szFontName = T("MS Sans Serif");
			hs.width      = 175;
      hs.bFwdMouseMsgs = true;

			link.Create( &hs );
			
			// Initialize text field
			SetDlgItemText( hWnd, IDC_ABOUT, T("NoteXpad 2.0.2 - (05:20 PM - 10/29/2003)")
											 T("\r\nRyan Lederman - ryan@ript.net")
											 T("\r\n----------------")
											 T("\r\nSpecial thanks:")
											 T("\r\nJosh Hill, (ohsix@winprog.org,O_6@EFNet)")
											 T("  for helping with optimising NoteXpad_ReadFile(), and the OpenGL control")
											 T(" on this very dialog")
											 T("\r\n\r\nCarl Corcoran, (tenfour@winprog.org,tenfour@EFNet)")
											 T(" for finding bugs, helping debug, and for suggesting features (UNICODE support, and more).")
											 T("\r\n\r\nBrook Miles, (forger@winprog.org,theForger@EFNet)")
											 T(" for testing, finding bugs, and consulting")
											 T("\r\n----------------")
											 T("\r\nNotes:")
											 T("\r\nPlease report bugs to the author (ryan@ript.net).")
											 T("\r\nFeel free to use/modify the NoteXpad sources,")
											 T(" but do not reuse the code in commercial applications")
											 T(" without written consent of the author.")
											 T(" Think you can do better? Please, if you have ideas and want")
											 T(" to implement them, go ahead and I'll add them to")
											 T(" the original codebase.")
											 );

      // Center in the parent window
      CenterWindow(GetParent(hWnd), hWnd);

			return TRUE;
      }
		case WM_CLOSE:
			link.Destroy();
			EndDialog( hWnd, 0 ); 
			break;
		case WM_COMMAND: switch( LOWORD(wParam) ) {
		case IDOK: 
			link.Destroy();
			EndDialog( hWnd, 0 ); break;
    case IDCANCEL:
      link.Destroy();
      EndDialog( hWnd, 0 ); break;
      
		} 
		break;
	}

	return FALSE;
}

void OnAboutLink( void *pVoid )
{
	//
	// OnAboutLink
	// Executed when user clicks hyperlink on About dialog
	//
	ShellExecute( NULL, T("open"), T("https://github.com/aremmell/notexpad"), NULL, NULL, SW_SHOWNORMAL );
}