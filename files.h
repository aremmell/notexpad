
//
// files.cpp - Defintitions for File I/O and misc for
// NoteXpad v2.0
// Ryan Lederman, ryan@winprog.org
//
#ifndef _FILES_H
#define _FILES_H

#include "Windows.h"
#include "resource.h"
#include "notexpad.h"
#include "tchar.h"

BOOL NoteXpad_ReadFile( HWND hWnd, TCHAR *szFilePath, BOOL *bModified, BOOL *bUnicode );
BOOL NoteXpad_WriteFile( HWND hWnd, TCHAR *szPath, BOOL *bModified, BOOL bUnicode, BOOL *bUniFlag );
BOOL NoteXpad_OpenFile( HWND hWnd, BOOL *bModified, TCHAR *szFilePath, BOOL *bUnicode );
int NoteXpad_SaveFile( HWND hWnd, BOOL *bModified, TCHAR *szFilePath, BOOL *bUnicode );
int PromptToSave( TCHAR *szFileName );
int PromptToOverwrite( TCHAR *szFileName );
int FileNameFromPath( TCHAR *szPath, TCHAR *szDestination, int iDestLen );
BOOL SetWindowCaption( HWND hWnd, TCHAR *szFileName, BOOL *bModified );
BOOL SetNewFileName( TCHAR *szNewFile, TCHAR *szOldFile );

#endif /* _FILES_H */