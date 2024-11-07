//
// notexpad.h - Main header file for NoteXpad v2.0
// Ryan Lederman, ryan@winprog.org

#ifndef _NOTEXPAD_H
#define _NOTEXPAD_H

// Includes
#include "StdAfx.h"
#include "files.h"
#include "handlers.h"
#include "shellapi.h"
#include "shlwapi.h"
#include "layered.h" // Roger Clark, drano@entendo.org

// Function signatures
LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
HWND CreateMainWindow( HINSTANCE hInstance );
HWND CreateViewWindow( HWND hWndParent, RECT r, BOOL bWordWrap );
BOOL InitializeWndClass( PWNDCLASSEX wc, HBRUSH hBrushBackground, HICON hIcon );
BOOL InitializeToolbar( HWND hWndToolbar );
BOOL InitializeAToolbarButton( PTBBUTTON pBtn, int iBitmap, int iString, int iID, HIMAGELIST hImageList, HBITMAP hBitmap, HBITMAP hMask );
BOOL UpdateUI( HWND hWnd );
int MessageLoop( HWND hWnd );

#endif /* _NOTEXPAD_H */