
//
// timedate.h - Definitions for insert time/date
// dialog and associated functionality
// NoteXpad 2.0
// Ryan Lederman, ryan@winprog.org
//

#ifndef _TIMEDATE_H
#define _TIMEDATE_H

#include "Windows.h"
#include "tchar.h"
#include "macros.h"
#include "resource.h"

HWND CreateTimeDateDialog( HWND hWndParent );
BOOL CALLBACK TimeDateDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL OnInsert( HWND hWnd, WPARAM wParam, LPARAM lParam );
BOOL OnTimeDateDlgCreate( HWND hWnd, WPARAM wParam, LPARAM lParam );
BOOL DayFromNumber( WORD wDay, TCHAR *szBuffer );
BOOL MonthFromNumber( WORD wMonth, TCHAR *szBuffer );
BOOL FormatBuffer( TCHAR *szBuffer, int bufsize, TCHAR *szFormat, ... );

#endif /* _TIMEDATE_H */