
//
// options.h - Definition of option get/save and misc
// for NoteXpad 2.0
// Ryan Lederman, ryan@winprog.org
//
#include "windows.h"
#include "tchar.h"
#include "reg.h"
#include "macros.h"

#ifndef _OPTIONMGR_H
#define _OPTIONMGR_H

#define ROOT_KEY		 HKEY_LOCAL_MACHINE
#define ROOT_OPTIONS_KEY T("Software\\NoteXpad\\Options")
#define ROOT_RECENT_KEY  T("Software\\NoteXpad\\Recent")
//
// Get
//
BOOL GetOptionString( TCHAR *szOption, TCHAR *szBuffer, DWORD dwBufLen );
BOOL GetOptionDWORD( TCHAR *szOption, DWORD *dwBuffer );
BOOL GetOptionBinary( TCHAR *szOption, void *pBuffer, DWORD dwBufLen );

//
// Set
//
BOOL SetOptionString( TCHAR *szOption, TCHAR *szBuffer );
BOOL SetOptionDWORD( TCHAR *szOption, DWORD dwOption );
BOOL SetOptionBinary( TCHAR *szOption, void *pBuffer, DWORD dwBufLen );

#endif /* _OPTIONMGR_H */