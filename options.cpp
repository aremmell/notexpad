
//
// options.cpp - Implementation of option get/save and misc
// for NoteXpad 2.0
// Ryan Lederman, ryan@winprog.org
//

#include "options.h"
#include "macros.h"
#include "files.h"

//
// Get
//
BOOL GetOptionString( TCHAR *szOption, TCHAR *szBuffer, DWORD dwBufLen )
{
	if( !szOption || !*szOption || !szBuffer || !dwBufLen )
		return FALSE;

	return ReadRegistryKey( ROOT_KEY, ROOT_OPTIONS_KEY, szOption, szBuffer, dwBufLen );
}

BOOL GetOptionDWORD( TCHAR *szOption, DWORD *dwBuffer )
{
	if( !szOption || !*szOption || !dwBuffer )
		return FALSE;

	*dwBuffer = ReadRegistryDWORD( ROOT_KEY, ROOT_OPTIONS_KEY, szOption );

	return TRUE;
}

BOOL GetOptionBinary( TCHAR *szOption, void *pBuffer, DWORD dwBufLen )
{
	if( !szOption || !*szOption || !pBuffer || !dwBufLen )
		return FALSE;

	return ReadRegistryBinary( ROOT_KEY, ROOT_OPTIONS_KEY, szOption, pBuffer, dwBufLen );
}

//
// Set
//
BOOL SetOptionString( TCHAR *szOption, TCHAR *szBuffer )
{
	if( !szOption || !*szOption || !szBuffer )
		return FALSE;

	return WriteRegistryKey( ROOT_KEY, ROOT_OPTIONS_KEY, szOption, szBuffer );
}

BOOL SetOptionDWORD( TCHAR *szOption, DWORD dwOption )
{
	if( !szOption || !*szOption )
		return FALSE;

	return WriteRegistryDWORD( ROOT_KEY, ROOT_OPTIONS_KEY, szOption, dwOption );
}

BOOL SetOptionBinary( TCHAR *szOption, void *pBuffer, DWORD dwBufLen )
{
	if( !szOption || !*szOption || !pBuffer || !dwBufLen )
		return FALSE;

	return WriteRegistryBinary( ROOT_KEY, ROOT_OPTIONS_KEY, szOption, pBuffer, dwBufLen );
}

