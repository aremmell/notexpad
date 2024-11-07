
//
// util.h - Miscellaneous functions
// for NoteXpad v2.0
// Ryan Lederman, ryan@winprog.org
//
#include "windows.h"

#ifndef _UTIL_H
#define _UTIL_H

BOOL ScreenToClientRect( RECT* pRect, HWND hWnd );
void CenterWindow( HWND hParent, HWND hChild );
void ToggleStayontop( HWND hWnd, BOOL bOnTop );
void ToggleWordwrap( HWND hWnd, BOOL bWrap );
void PrintPageHeader( HDC dc, int width, TCHAR *szFileName, int page );
int SetFileAssociation( TCHAR *szApp, TCHAR *szFileType, int iconNum );
BOOL RealPathFromStupidPath( TCHAR *szStupidPath, TCHAR *szRealPath );

typedef struct tagDIALOGS {

  enum Type {

    FIND = 0,
    TIME,
    SETTINGS

  };

  tagDIALOGS() {

    hFindDlg      = NULL;
    hTimeDlg      = NULL;
    hSettingsDlg  = NULL;
  }

  HWND hFindDlg;
  HWND hTimeDlg;
  HWND hSettingsDlg;

  void SetDlg(HWND hWnd, Type t) {

    switch(t) {

      case FIND:
        hFindDlg = hWnd;
      break;
      case TIME:
        hTimeDlg = hWnd;
      break;
      case SETTINGS:
        hSettingsDlg = hWnd;
      break;

    }

  }

  bool IsDlgMsg(MSG *msg) {

    bool r = true;

    if (!IsDialogMessage(hFindDlg, msg)) {
      if (!IsDialogMessage(hTimeDlg, msg)) {
        if (!IsDialogMessage(hSettingsDlg, msg)) {

          r = false;

        }
      }
    }

    return r;
  }

  bool IsNull(void) { 
    
    if (hFindDlg == NULL && hTimeDlg == NULL && hSettingsDlg == NULL) {
      return true;
    } else {
      return false;
    }

  }

} DIALOGS, *PDIALOGS;

#endif /* _UTIL_H */