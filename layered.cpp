
/*
 * layered.cpp - NoteXpad2 Layered Window Attributes workaround
 * Roger Clark, drano@entendo.org
 */

#include "layered.h"

#ifdef WS_EX_LAYERED_WORKAROUND

static SETLAYEREDWINDOWATTRIBUTES g_MySLWA = NULL;
static BOOL g_bChecked = FALSE;

BOOL SetLayeredWindowAttributes(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags)
{
	if (!g_bChecked) {
		HMODULE hUser32 = LoadLibrary("USER32.DLL");

		g_MySLWA = GetProcAddress(hUser32, "SetLayeredWindowAttributes");

		g_bChecked = TRUE;
	}

	if (g_MySLWA != NULL)
		return g_MySLWA(hwnd, crKey, bAlpha, dwFlags);

	return FALSE;
}

#endif
