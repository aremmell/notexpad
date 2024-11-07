
//
// recentfiles.h - Definitions for recent file
// list functionality, NoteXpad 2.0
// Ryan Lederman, ryan@winprog.org
//
#ifndef _RECENTFILES_H
#define _RECENTFILES_H

#include "Windows.h"
#include "tchar.h"
#include "macros.h"
#include "reg.h"
#include "resource.h"

BOOL LoadRecentFiles( HWND hWnd );
BOOL SetNewRecentFile( HWND hWnd, TCHAR *szFile );

#endif /* _RECENTFILES_H */