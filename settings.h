
//
// settings.h - Definitions for settings dialog and
// associated code
// NoteXpad 2.0
// Ryan Lederman, ryan@winprog.org
//
#ifndef _SETTINGS_H
#define _SETTINGS_H

#include "Windows.h"
#include "tchar.h"
#include "macros.h"

HWND CreateSettingsDialog( HWND hWndParent );
BOOL CALLBACK SettingsDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL OnInitSettingsDlg( HWND hWnd, WPARAM wParam, LPARAM lParam );
BOOL OnOK( HWND hWnd, WPARAM wParam, LPARAM lParam );
BOOL OnCancel( HWND hWnd, WPARAM wParam, LPARAM lParam );
BOOL SaveSettings( HWND hWnd );

#endif /* _SETTINGS_H */