
//
// findreplace.h - Definitions for
// Find and Replace functionality
// NoteXpad 2.0, Ryan Lederman, ryan@winprog.org
//

#ifndef _FINDREPLACE_H
#define _FINDREPLACE_H

#include "notexpad.h"

typedef struct {
	const TCHAR *szData;
	TCHAR **szDest;
	DWORD *iDestLen;
	TCHAR *szFind;
	const TCHAR *szReplace;
	BOOL  bCaseSensitive;
} REPLACESTRUCT, *PREPLACESTRUCT;

HWND CreateFindReplaceDialog( HWND hWndParent );
BOOL CALLBACK FRDialogProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL OnInitDialog( HWND hWnd, WPARAM wParam, LPARAM lParam );
BOOL FindReplace_OnFind( HWND hWnd, WPARAM wParam, LPARAM lParam );
BOOL FindReplace_OnReplace( HWND hWnd, WPARAM wParam, LPARAM lParam );
BOOL FindReplace_OnReplaceAll( HWND hWnd, WPARAM wParam, LPARAM lParam );
int Replace( PREPLACESTRUCT prs );

#endif _FINDREPLACE_H