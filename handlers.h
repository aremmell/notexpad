
//
// handlers.h - Definition of message handler functions
// for NoteXpad v2.0
// Ryan Lederman, ryan@winprog.org
//

#ifndef _HANDLERS_H
#define _HANDLERS_H

#include "windows.h"
#include "tchar.h"
#include "resource.h"
#include "findreplace.h"

LRESULT OnSize( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnMenuSelect( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnNotify( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnCommand( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnNew( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnNewWindow( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnSave( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnSaveAs( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnOpen( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnPrint( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnDropFile( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnTimer( HWND hWnd, WPARAM wParam, LPARAM lParam, HICON hIconSaved, HICON hIconUnsaved );
LRESULT OnFind( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnJumpTo( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnForeColor( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnBackColor( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnChangeFont( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnToggleStayontop( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnToggleWordWrap( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnUndo( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnCut( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnCopy( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnPaste( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnTab( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnSelectAll( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnFileProperties( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnInsertTimeDate( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnSettings( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnCtlColorEdit( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnCreate( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnClose( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT OnRecentFile( HWND hWnd, UINT id );
LRESULT OnAboutDialog( HWND hWnd, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK AboutProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
void OnAboutLink( void *pVoid );
#endif /* _HANDLERS_H */