
//
// timedate.cpp - Implementation of insert time/date
// dialog and associated functionality
// NoteXpad 2.0
// Ryan Lederman, ryan@winprog.org
//
#include "timedate.h"

HWND CreateTimeDateDialog( HWND hWndParent )
{
	//
	// CreateTimeDateDialog
	// Creates the time date dialog.
	//
	HWND hWnd = 0;

	if( !IsWindow( hWndParent ) )
		return NULL;

	hWnd = CreateDialog( GetModuleHandle( NULL ), (LPCTSTR)IDD_INSERTTIMEDATE, hWndParent, TimeDateDlgProc );
	
	ShowWindow( hWnd, SW_SHOW );

	return hWnd;
}

BOOL CALLBACK TimeDateDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	//
	// TimeDateDlgProc
	// message handler function for time date dialog
	//
	switch( msg )
	{
		case WM_INITDIALOG: return OnTimeDateDlgCreate( hWnd, wParam, lParam );
		case WM_CLOSE: EndDialog( hWnd, 0 ); break;
		case WM_COMMAND:
			switch( LOWORD(wParam) )
			{
				case IDOK: return OnInsert( hWnd, wParam, lParam );
				case IDCANCEL: EndDialog( hWnd, 0 ); break;
				case IDC_LIST: 
					if( HIWORD(wParam) == LBN_DBLCLK )
						return OnInsert( hWnd, wParam, lParam );
					break;
			}
		break;
	}
	return FALSE;
}

BOOL OnTimeDateDlgCreate( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// OnTimeDateDlgCreate
	// initializes list of usable formats in the list
	//
	TCHAR szTemp[512] = {0};
	TCHAR szAmPm[3]   = {0};
	TCHAR szDay[10]   = {0};
	TCHAR szMon[11]   = {0};
	SYSTEMTIME st_s   = {0};
	SYSTEMTIME st_m   = {0};

	GetLocalTime( &st_s ); // Standard
	GetLocalTime( &st_m ); // Military

	_tcscpy( szAmPm, T("AM") );

	if( st_s.wHour == 0 )
		st_s.wHour = 12;
	if( st_s.wHour > 12 ) {
		st_s.wHour -=12;
		_tcscpy( szAmPm, T("PM") ); }

	//
	// Got time, initialize list box
	//
	DayFromNumber( st_s.wDayOfWeek, szDay );
	MonthFromNumber( st_s.wMonth, szMon );

	// HH:MM:SS (am/pm) - MM/DD/YYYY
	FormatBuffer( szTemp, 
				  512, 
				  T("%02d:%02d:%02d %s - %02d/%02d/%04d"), 
				  st_s.wHour, 
				  st_s.wMinute, 
				  st_s.wSecond,
				  szAmPm,
				  st_s.wMonth,
				  st_s.wDay,
				  st_s.wYear );

	SendMessage( GetDlgItem( hWnd, IDC_LIST ), LB_INSERTSTRING, 0, (LPARAM)szTemp );

	// Day, Month DD
	FormatBuffer( szTemp,
				  512,
				  T("%s, %s %02d"),
				  szDay,
				  szMon,
				  st_s.wDay );
				  
	SendMessage( GetDlgItem( hWnd, IDC_LIST ), LB_INSERTSTRING, 0, (LPARAM)szTemp );

	// Plain old military time
	FormatBuffer( szTemp,
				  512,
				  T("%02d:%02d:%02d"),
				  st_m.wHour,
				  st_m.wMinute,
				  st_m.wSecond );

	SendMessage( GetDlgItem( hWnd, IDC_LIST ), LB_INSERTSTRING, 0, (LPARAM)szTemp );

	// MM.DD.YYYY
	FormatBuffer( szTemp,
				  512,
				  T("%02d.%02d.%02d"),
				  st_s.wMonth,
				  st_s.wDay,
				  st_s.wYear );

	SendMessage( GetDlgItem( hWnd, IDC_LIST ), LB_INSERTSTRING, 0, (LPARAM)szTemp );

	// MMMM DD, YYYY
	FormatBuffer( szTemp,
				  512,
				  T("%s %02d, %04d"),
				  szMon,
				  st_s.wDay,
				  st_s.wYear );

	SendMessage( GetDlgItem( hWnd, IDC_LIST ), LB_INSERTSTRING, 0, (LPARAM)szTemp );

	// DDDD, MMMM DD HH:MM:SS
	FormatBuffer( szTemp,
				  512,
				  T("%s, %s %02d - %02d:%02d:%02d"),
				  szDay,
				  szMon,
				  st_m.wDay,
				  st_m.wHour,
				  st_m.wMinute,
				  st_m.wSecond );

	SendMessage( GetDlgItem( hWnd, IDC_LIST ), LB_INSERTSTRING, 0, (LPARAM)szTemp );
	
	// MM/DD/YYYY
	FormatBuffer( szTemp,
				  512,
				  T("%02d/%02d/%04d"),
				  st_s.wMonth,
				  st_s.wDay,
				  st_s.wYear );

	SendMessage( GetDlgItem( hWnd, IDC_LIST ), LB_INSERTSTRING, 0, (LPARAM)szTemp );

	//
	// Select first item in list
	//
	SendMessage( GetDlgItem( hWnd, IDC_LIST ), LB_SETCURSEL, 0, 0 );
	return FALSE;
}

BOOL OnInsert( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	//
	// Puts the requested format into the view
	// at the caret position
	//
	TCHAR szFormat[512] = {0};
	int   Index         = 0;

	Index = SendMessage( GetDlgItem( hWnd, IDC_LIST ), LB_GETCURSEL, 0, 0 );

	if( Index == LB_ERR ) 
	{	
		MessageBeep( MB_OK );
		return FALSE;
	}

	SendMessage( GetDlgItem( hWnd, IDC_LIST ), LB_GETTEXT, (WPARAM)Index, (LPARAM)szFormat );

	SendMessage( GetDlgItem( GetParent( hWnd ), ID_VIEW ), EM_REPLACESEL, TRUE, (LPARAM)szFormat );
	SetFocus( GetDlgItem( GetParent( hWnd ), ID_VIEW ) );

	EndDialog( hWnd, 0 );

	return FALSE;
}

BOOL DayFromNumber( WORD wDay, TCHAR *szBuffer )
{
	//
	// DayFromNumber
	// returns string identifier of wDay
	//
	if( !szBuffer )
		return FALSE;

	switch( wDay )
	{
		case 0: _tcscpy( szBuffer, T("Sunday") ); break;
		case 1: _tcscpy( szBuffer, T("Monday") ); break;
		case 2: _tcscpy( szBuffer, T("Tuesday") ); break;
		case 3: _tcscpy( szBuffer, T("Wednesday") ); break;
		case 4: _tcscpy( szBuffer, T("Thursday") ); break;
		case 5: _tcscpy( szBuffer, T("Friday") ); break;
		case 6: _tcscpy( szBuffer, T("Saturday") ); break;
	}

	return TRUE;
}

BOOL MonthFromNumber( WORD wMonth, TCHAR *szBuffer )
{
	//
	// MonthFromNumber
	// returns string identifier of wMonth
	//
	if( !szBuffer )
		return FALSE;

	switch( wMonth )
	{
		case 1: _tcscpy( szBuffer, T("January") ); break;
		case 2: _tcscpy( szBuffer, T("Febuary") ); break;
		case 3: _tcscpy( szBuffer, T("March") ); break;
		case 4: _tcscpy( szBuffer, T("April") ); break;
		case 5: _tcscpy( szBuffer, T("May") ); break;
		case 6: _tcscpy( szBuffer, T("June") ); break;
		case 7: _tcscpy( szBuffer, T("July") ); break;
		case 8: _tcscpy( szBuffer, T("August") ); break;
		case 9: _tcscpy( szBuffer, T("September") ); break;
		case 10: _tcscpy( szBuffer, T("October") ); break;
		case 11: _tcscpy( szBuffer, T("November") ); break;
		case 12: _tcscpy( szBuffer, T("December") ); break;
	}

	return TRUE;
}

BOOL FormatBuffer( TCHAR *szBuffer, int bufsize, TCHAR *szFormat, ... )
{
	//
	// FormatBuffer
	// formats szBuffer in the format szFormat
	//
	if( !szBuffer || !szFormat || !*szFormat )
		return FALSE;

	memset( szBuffer, 0, bufsize );

	va_list pArgList;
	va_start ( pArgList, szFormat );
	wvsprintf ( szBuffer, szFormat, pArgList );
	va_end ( pArgList );

	return TRUE;
}