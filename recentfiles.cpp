
//
// recentfiles.cpp - Implementation of recent file
// list functionality, NoteXpad 2.0
// Ryan Lederman, ryan@winprog.org
//
#include "recentfiles.h"
#include "files.h"

BOOL LoadRecentFiles( HWND hWnd )
{
	//
	// Do recent file menu
	//
	TCHAR  szBuffer[MAX_PATH+1] = {0};
	TCHAR  szName[MAX_PATH+1]   = {0};
	TCHAR  szTemp[MAX_PATH+3]   = {0};
	MENUITEMINFO  mii           = {0};
	DWORD  dwRecent             = 0;

	while( EnumKeys( szBuffer, szName, MAX_PATH+1, MAX_PATH+1 ) ) {
		if( _tcslen( szBuffer ) > 0 ) {
			
			// Add menu item
			mii.cbSize = sizeof( mii );
			mii.fMask  = 0x00000040 | MIIM_ID;
			mii.fType  = MFT_STRING;
			if( dwRecent == 0 )
				mii.wID = ID_RECENTFILE1;
			if( dwRecent == 1 )
				mii.wID = ID_RECENTFILE2;
			if( dwRecent == 2 )
				mii.wID = ID_RECENTFILE3;
			if( dwRecent == 3 )
				mii.wID = ID_RECENTFILE4;
			wsprintf( szTemp, T("%d) %s"), dwRecent+1, szName );
			mii.dwTypeData = szTemp;
			InsertMenuItem( GetSubMenu( GetMenu( hWnd ), 0 ), 9+dwRecent, TRUE, &mii );
			memset( szTemp, 0, MAX_PATH+3 );
			dwRecent++;
      
		}
	}

	if( !dwRecent )
	{
		mii.cbSize = sizeof( mii );
		mii.fMask  = 0x00000040 | MIIM_STATE | MIIM_ID;
		mii.fType  = MFT_STRING;
		mii.fState = MFS_GRAYED;
		mii.wID    = ID_NORECENTFILES;
		mii.dwTypeData = T("[Empty]");
		InsertMenuItem( GetSubMenu( GetMenu( hWnd ), 0 ), 9, TRUE, &mii );
	}

	return TRUE;
}

BOOL SetNewRecentFile( HWND hWnd, TCHAR *szFile )
{
	TCHAR szName[4][MAX_PATH+1]   = {0};
	TCHAR szBuffer[4][MAX_PATH+1] = {0};
	TCHAR szTempName[MAX_PATH+1]  = {0};
	DWORD dwCount                 = 0;

	if( !szFile || !*szFile )
		return FALSE;
	
	//	Find out if there is already 4 values
	while( EnumKeys( szBuffer[dwCount], szName[dwCount], MAX_PATH+1, MAX_PATH+1 ) ) {
		dwCount++;
	}

	if( dwCount == 4 )
	{
		// Max recent files reached, delete first key, then add
		DeleteRegistryKey( ROOT_KEY, ROOT_RECENT_KEY, szName[0] );
	}

	FileNameFromPath( szFile, szTempName, MAX_PATH+1 );

	if( !WriteRegistryKey( ROOT_KEY, ROOT_RECENT_KEY, szTempName, szFile ) )
		return FALSE;

	//
	// Now, remove all items from menu and refresh
	//
	RemoveMenu( GetSubMenu( GetMenu( hWnd ), 0 ), ID_RECENTFILE1, MF_BYCOMMAND );
	RemoveMenu( GetSubMenu( GetMenu( hWnd ), 0 ), ID_RECENTFILE2, MF_BYCOMMAND );
	RemoveMenu( GetSubMenu( GetMenu( hWnd ), 0 ), ID_RECENTFILE3, MF_BYCOMMAND );
	RemoveMenu( GetSubMenu( GetMenu( hWnd ), 0 ), ID_RECENTFILE4, MF_BYCOMMAND );
	RemoveMenu( GetSubMenu( GetMenu( hWnd ), 0 ), ID_NORECENTFILES, MF_BYCOMMAND );

	LoadRecentFiles( hWnd );

	return TRUE;
}