/*
 * layered.h - NoteXpad2 Layered Window Attributes workaround
 * Roger Clark, drano@entendo.org
 */

#ifndef _NOTEXPAD2_LAYERED_H_
#define _NOTEXPAD2_LAYERED_H_
#include "Windows.h"

#ifndef WS_EX_LAYERED

#define WS_EX_LAYERED_WORKAROUND

BOOL SetLayeredWindowAttributes(
  HWND hwnd,           // handle to the layered window
  COLORREF crKey,      // specifies the color key
  BYTE bAlpha,         // value for the blend function
  DWORD dwFlags        // action
);

typedef BOOL (WINAPI *SETLAYEREDWINDOWATTRIBUTES)(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

#define WS_EX_LAYERED           0x00080000
#define LWA_COLORKEY            0x00000001
#define LWA_ALPHA               0x00000002

#endif /* ndef WS_EX_LAYERED */

#endif /* _NOTEXPAD2_LAYERED_H_ */