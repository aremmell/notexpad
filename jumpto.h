
//
// jumpto.h - Definitions for jump to line 
// dialog and associated code
// for NoteXpad 2.0
// Ryan Lederman, ryan@winprog.org
//
#ifndef _JUMPTO_H
#define _JUMPTO_H

#include "windows.h"
#include "tchar.h"
#include "macros.h"

int CreateJumpToDialog( HWND hWndParent );
BOOL CALLBACK JumpToDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

#endif /* _JUMPTO_H */