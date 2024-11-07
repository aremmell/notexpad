
//
// NoteXpad v2.0, Win32
// Ryan Lederman, ryan@winprog.org
// http://ryan.ript.net/notexpad
// Whoa, free code!
//

#include "notexpad.h"
#include "resource.h"
#include "util.h"
#include "options.h"
#include "macros.h"

HIMAGELIST hImageList             = 0;
BOOL       bModified              = FALSE;
BOOL       bUnicode               = FALSE;
TCHAR      szFilePath[MAX_PATH+1] = {0};
HICON      hIconSaved             = 0;
HICON      hIconUnsaved           = 0;
HBRUSH     hBrushView             = 0;
HFONT      hFontView              = 0;
DIALOGS    Dialogs;

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	//
	// WinMain - Entry point for the application,
	// initialises main window and enters message loop
	//
	WNDCLASSEX wc     = {0};
	HWND hWnd         = 0;
	HBRUSH hBrushBack = 0;
	HICON  hIcon      = 0;
	HLOCAL hEditHandle= 0;
	COLORREF rgbView  = {0};
	int  iMessageLoop = 0;
	TCHAR *szCmdLine  = 0;
	TCHAR **szArgv    = 0;
	TCHAR szRealPath[MAX_PATH] = {0};
	int  iArgc        = 0;
	INITCOMMONCONTROLSEX iccex = {0};

	//
	// Windows 2000 and higher ONLY!
	//
	if( GetVersion() < 0x80000000 && LOBYTE(LOWORD(GetVersion())) < 5 ) {
		MessageBox( GetForegroundWindow(), T("NoteXpad requires atleast Windows 2000."), 
					T("Operating System too old"), MB_OK | MB_ICONSTOP );  return 0; }

	hIcon      = (HICON)LoadImage( hInstance, (LPCTSTR)IDI_NOTEXPAD, IMAGE_ICON, 16, 16, 0 );
	hBrushBack = CreateSolidBrush( GetSysColor( COLOR_3DFACE ) );

	if( !InitializeWndClass( &wc, hBrushBack, hIcon ) )
	{
		MSGBOX_ERROR( T("Failed to register main window class.") );
		return 1;
	}

	hWnd  = CreateMainWindow( hInstance );

	if( !hWnd )
	{
		MSGBOX_ERROR( T("Failed to create main window.") );
		return 1;
	}

	szCmdLine = GetCommandLine();

	szArgv = CommandLineToArgvW( szCmdLine, &iArgc );

	if( szArgv[1] && iArgc > 1 )
	{		
		RealPathFromStupidPath( szArgv[1], szRealPath );
		SetNewFileName( szRealPath, szFilePath );

		if( !NoteXpad_ReadFile( hWnd, szFilePath, &bModified, &bUnicode ) )
		{
			MSGBOX_ERROR( T("Failed to open that file!") );
			goto command_line_error;
		}
	}
	else
	{
command_line_error:
		SetNewFileName( T("Untitled"), szFilePath );
		UpdateUI( hWnd );
	}

	GlobalFree( szArgv ); // be a good little soldier :)
	//
	// Initialize Common Controls
	//
	iccex.dwSize = sizeof( iccex );
	iccex.dwICC  = ICC_BAR_CLASSES;

	InitCommonControlsEx( &iccex );

	hIconSaved   = (HICON)LoadImage( hInstance, (LPCTSTR)IDI_SAVED, IMAGE_ICON, 12, 12, 0 );
	hIconUnsaved = (HICON)LoadImage( hInstance, (LPCTSTR)IDI_UNSAVED, IMAGE_ICON, 12, 12, 0 );

	if( !GetOptionBinary( T("BackColor"), &rgbView, sizeof( rgbView ) ) )
		hBrushView   = CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );
	else
		hBrushView   = CreateSolidBrush( rgbView );

	DragAcceptFiles( hWnd, TRUE );

	ShowWindow( hWnd, SW_SHOW );

	iMessageLoop = MessageLoop( hWnd );

	hEditHandle = (HLOCAL)SendMessage( GetDlgItem( hWnd, ID_VIEW ), EM_GETHANDLE, 0, 0 );
	LocalUnlock( hEditHandle );
	LocalFree( hEditHandle );

	DestroyIcon( hIcon );
	DestroyIcon( hIconSaved );
	DestroyIcon( hIconUnsaved );
	DeleteObject( hBrushBack );
	DeleteObject( hBrushView );
	DeleteObject( hFontView );
	ImageList_Destroy( hImageList );
	KillTimer( hWnd, 1 );

	return iMessageLoop;
}

int MessageLoop( HWND hWnd )
{
	//
	// MessageLoop - The whole program!
	//
	MSG msg       = {0};
	HACCEL hAccel = 0;

	hAccel = LoadAccelerators( GetModuleHandle( NULL ), (LPCTSTR)IDR_ACCELERATOR );

	while( GetMessage( &msg, NULL, 0, 0 ) ) {

    if (Dialogs.IsNull() || !Dialogs.IsDlgMsg(&msg))
	  {
      if (!TranslateAccelerator( hWnd, hAccel, &msg )) {

		    TranslateMessage( &msg );
		    DispatchMessage( &msg );

      }
    }

	}

	DestroyAcceleratorTable( hAccel );
	

	return msg.wParam;
}

HWND CreateMainWindow( HINSTANCE hInstance )
{
	//
	// CreateMainWindow
	// creates the main window!
	//
	HWND hWnd                   = 0;
	HWND hStatusBar             = 0;
	HWND hWndToolBar            = 0;
	HWND hWndView               = 0;
	RECT rClient		            = {0};
	RECT rStatusBar             = {0};
	RECT rToolbar               = {0};
	RECT rView                  = {0};
	DWORD dwWrap                = 0;
	INITCOMMONCONTROLSEX iccsex = {0};


	hWnd = CreateWindowEx( WS_EX_LAYERED,			// Extended styles
						   T("NoteXpad_Wnd"),		// Class name
						   T(" NoteXpad"),			// Window title
						   WS_OVERLAPPEDWINDOW,		// Styles
						   CW_USEDEFAULT,			// x
						   CW_USEDEFAULT,			// y
						   CW_USEDEFAULT,			// cx
						   CW_USEDEFAULT,			// cy
						   NULL,					// Parent window
						   NULL,    				// HMENU
						   hInstance,				// App instance
						   NULL );					// Reserved or something

	if( !hWnd )
		return 0;

	iccsex.dwSize = sizeof( iccsex );
	iccsex.dwICC = ICC_BAR_CLASSES | ICC_COOL_CLASSES ;

	if( !InitCommonControlsEx( &iccsex ) )
		return 0;

	hStatusBar = CreateWindow( STATUSCLASSNAME, 
							   T(""), 
							   WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 
							   0, 
							   0, 
							   0, 
							   0,
							   hWnd,
							   (HMENU)ID_STATUSBAR, 
							   hInstance, 
							   NULL );

	if( !hStatusBar )
		return 0;

	GetClientRect( hStatusBar, &rStatusBar );

	hWndToolBar = CreateWindow( TOOLBARCLASSNAME,
								T(""),
								WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
								0,
								0,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								hWnd,
								(HMENU)ID_TOOLBAR,
								hInstance,
								NULL );

	if( !hWndToolBar )
		return 0;

	if( !InitializeToolbar( hWndToolBar ) )
		return 0;

	GetClientRect( hWndToolBar, &rToolbar );

	GetClientRect( hWnd, &rClient );

	rView.left   = 0;
	rView.top    = rToolbar.bottom;
	rView.right  = (rClient.right - rClient.left);
	rView.bottom = (rClient.bottom - rClient.top)-(rStatusBar.bottom - rStatusBar.top)-rToolbar.bottom;
	
	GetOptionDWORD( T("WordWrap"), &dwWrap );

	ToggleWordwrap( hWnd, dwWrap ? TRUE : FALSE );

	OnCreate( hWnd, 0, 0 );
	
	return hWnd;
}

HWND CreateViewWindow( HWND hWndParent, RECT r, BOOL bWordWrap )
{
	//
	// Creates the view window
	//
	DWORD dwStyles = 0;
	HWND  hWndView = 0;

	dwStyles = WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | ES_NOHIDESEL | ES_LEFT | ES_WANTRETURN;

	if( !bWordWrap )
		dwStyles |= WS_HSCROLL;

	hWndView = CreateWindowEx( WS_EX_CLIENTEDGE, 
						   T("EDIT"),
						   T(""),
						   dwStyles,
						   r.left,
						   r.top,
						   r.right,
						   r.bottom,
						   hWndParent,
						   (HMENU)ID_VIEW,
						   GetModuleHandle( NULL ),
						   NULL );

	SendMessage( hWndView, WM_SETFONT, (WPARAM)hFontView, (LPARAM)TRUE );

	return hWndView;
}

BOOL InitializeWndClass( PWNDCLASSEX wc, HBRUSH hBrushBackground, HICON hIcon )
{
	//
	// InitializeWndClass
	// fills out a window class structure for our main window
	//
	if( !wc )
		return FALSE;

	wc->cbSize = sizeof( WNDCLASSEX );
	wc->cbClsExtra = 0;
	wc->hbrBackground = hBrushBackground;
	wc->hCursor       = LoadCursor( NULL, IDC_ARROW );
	wc->hIcon         = hIcon;
	wc->hIconSm       = hIcon;
	wc->hInstance     = GetModuleHandle( NULL );
	wc->lpfnWndProc   = WndProc;
	wc->lpszClassName = T("NoteXpad_Wnd");
	wc->lpszMenuName  = (LPCTSTR)IDR_MAINMENU;
	wc->style         = CS_DBLCLKS;

	if( !RegisterClassEx( wc ) )
		return FALSE;

	return TRUE;
}

BOOL InitializeToolbar( HWND hWndToolbar )
{
	//
	// InitializeToolbar
	// creates and adds buttons to the toolbar
	//
	TBBUTTON buttons[11] = {0};
	HBITMAP  hBitmap     = 0;
	HBITMAP  hMask       = 0;

	hImageList = ImageList_Create( 16, 16, ILC_COLOR32 | ILC_MASK, 1, 1 );

	if( !hImageList )
		return FALSE;

	SendMessage( hWndToolbar, TB_SETIMAGELIST, 0, (LPARAM)hImageList );

	hBitmap = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_NEW );
	hMask   = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_NEWMASK );
	InitializeAToolbarButton( &buttons[0], 0, 0, ID_NEW, hImageList, hBitmap, hMask );

	hBitmap = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_OPEN );
	hMask   = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_OPENMASK );
	InitializeAToolbarButton( &buttons[1], 1, 0, ID_OPEN, hImageList, hBitmap, hMask );

	hBitmap = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_SAVE );
	hMask   = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_SAVEMASK );
	InitializeAToolbarButton( &buttons[2], 2, 0, ID_SAVE, hImageList, hBitmap, hMask );

	hBitmap = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_PRINT );
	hMask   = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_PRINTMASK );
	InitializeAToolbarButton( &buttons[3], 3, 0, ID_PRINT, hImageList, hBitmap , hMask);

	hBitmap = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_CUT );
	hMask   = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_CUTMASK );
	InitializeAToolbarButton( &buttons[5], 4, 0, ID_CUT, hImageList, hBitmap, hMask );

	hBitmap = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_COPY );
	hMask   = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_COPYMASK );
	InitializeAToolbarButton( &buttons[6], 5, 0, ID_COPY, hImageList, hBitmap, hMask );

	hBitmap = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_PASTE );
	hMask   = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_PASTEMASK );
	InitializeAToolbarButton( &buttons[7], 6, 0, ID_PASTE, hImageList, hBitmap, hMask );

	hBitmap = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_FIND );
	hMask   = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_FINDMASK );
	InitializeAToolbarButton( &buttons[9], 7, 0, ID_FIND, hImageList, hBitmap, hMask );

	hBitmap = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_STAYONTOP );
	hMask   = (HBITMAP)LoadBitmap( GetModuleHandle( NULL ), (LPCTSTR)IDB_STAYONTOPMASK );
	InitializeAToolbarButton( &buttons[10], 8, 0, ID_STAYONTOP, hImageList, hBitmap, hMask );

	buttons[4].fsStyle = TBSTYLE_SEP;
	buttons[4].iBitmap = 3;
	buttons[8].fsStyle = TBSTYLE_SEP;
	buttons[8].iBitmap = 3;

	SendMessage( hWndToolbar, TB_BUTTONSTRUCTSIZE, sizeof( TBBUTTON ), 0 );
	SendMessage( hWndToolbar, TB_ADDBUTTONS, (WPARAM)11, (LPARAM)&buttons );
	SendMessage( hWndToolbar, TB_AUTOSIZE, 0, 0 );
	ShowWindow( hWndToolbar, SW_SHOWNORMAL );

	return TRUE;
}

BOOL InitializeAToolbarButton( PTBBUTTON pBtn, int iBitmap, int iString, int iID, HIMAGELIST hImageList, HBITMAP hBitmap, HBITMAP hMask )
{
	//
	// InitializeAToolbarButton
	// Fills out a data structure for adding a button to the toolbar
	//

	if( !pBtn )
		return FALSE;

	pBtn->dwData    = 0;
	pBtn->fsState   = TBSTATE_ENABLED;
	pBtn->fsStyle   = (BYTE)(TBSTYLE_FLAT |TBSTYLE_TOOLTIPS);
	pBtn->iBitmap   = iBitmap;
	pBtn->idCommand = iID;
	pBtn->iString   = iString;

	ImageList_Add( hImageList, hBitmap, hMask );
	DeleteObject( hBitmap );
	DeleteObject( hMask );

	return TRUE;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	//
	// WndProc
	// Window procedure for the main window, routes all
	// pertinent messages to its proper function
	//
	switch( msg )
	{
		case WM_CLOSE: return OnClose( hWnd, wParam, lParam );
		case WM_SIZE: return OnSize( hWnd, wParam, lParam );
		case WM_MENUSELECT: return OnMenuSelect( hWnd, wParam, lParam );
		case WM_NOTIFY: return OnNotify( hWnd, wParam, lParam );
		case WM_COMMAND: return OnCommand( hWnd, wParam, lParam );
		case WM_DROPFILES: return OnDropFile( hWnd, wParam, lParam );
		case WM_TIMER: return OnTimer( hWnd, wParam, lParam, hIconSaved, hIconUnsaved );
		case WM_CTLCOLOREDIT: return OnCtlColorEdit( hWnd, wParam, lParam );
		case WM_SETFOCUS: SetFocus( GetDlgItem( hWnd, ID_VIEW ) ); break;
		default:
			return DefWindowProc( hWnd, msg, wParam, lParam );
	}
	return DefWindowProc( hWnd, msg, wParam, lParam );
}

BOOL UpdateUI( HWND hWnd )
{
	//
	// UpdateUI
	// Updates User Interface
	// according to state of file
	//

	SetWindowCaption( hWnd, szFilePath, &bModified );
	OnTimer( hWnd, 0, 0, hIconSaved, hIconUnsaved );
	return TRUE;
}
