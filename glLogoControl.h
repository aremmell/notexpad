
#ifndef _GLHEADER_H
#define _GLHEADER_H

#include "stdafx.h"
#include "macros.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

// chunk of info we alias with the gl view to make it nice and koshser
struct gldata_t {
	HWND hWnd; // owner of this structure
	HGLRC hRC; // opengl rendering context handle
	HDC hDC; // window handle
	HANDLE hEvent; // handle to signal when to stop the thread
	DWORD dwThread;
	HANDLE hThread; // thread handle
};

int glLogoCreate(int width, int height); // used to setup the viewport and the perspective when the view is created
void glLogoDraw(); // draw routine, called in a loop in its own thread
void glLogoDestroy(); // called when the context is destroyed
void glLogoControlInit(); // initializes the window class and stuff for use

#endif /* _GLHEADER_H */