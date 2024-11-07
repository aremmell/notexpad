/* This is the studly chunk of code Wyatt asked me to hack up for NoteXpad2
	... O_6
*/

#include "glLogoControl.h"
#include "resource.h"
#include "stdlib.h"
#include "time.h"

// module private functions
LRESULT CALLBACK glLogoControlWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int glLogoCreateGLView(HWND hwnd);
void glLogoDestroyGLView(HWND hwnd);

LRESULT CALLBACK glLogoControlWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch(uMsg){
		case WM_CREATE:
			glLogoCreateGLView(hwnd);
			return 0;

		case WM_DESTROY:
			glLogoDestroyGLView(hwnd);
			return 0;

/*		case WM_MOUSEMOVE: // this is lame :(
			SetCursor(NULL);
			return 0;
*/
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

DWORD WINAPI glLogoDrawThread(LPVOID lpParameter){
	struct gldata_t *glData = (struct gldata_t *)lpParameter;
	RECT rc;

	OutputDebugString(T("Starting Thread\n"));

	// you have to create context's in the threads they are used
	glData->hRC = wglCreateContext(glData->hDC); // so we do that here
	wglMakeCurrent(glData->hDC, glData->hRC); // and more in-thread context bs

	GetWindowRect(glData->hWnd, &rc); // used for setting up the viewport

	glLogoCreate(rc.right - rc.left, rc.bottom - rc.top); // pass it to the user routine

	while(WaitForSingleObject(glData->hEvent, 0) != WAIT_OBJECT_0){ // Wait for signal
		Sleep(1); // give up the slice so we dont choke the cpu with our simple effect
		glLogoDraw(); // our drawing routine
		SwapBuffers(glData->hDC); // flip the backbuffer into view
	}

	wglMakeCurrent(NULL, NULL); // unselects any current opengl context
	wglDeleteContext(glData->hRC); // deletes it

	glLogoDestroy(); // call user routine

	OutputDebugString(T("Ending Thread\n"));

	return 1;
}

int glLogoCreateGLView(HWND hwnd){
	PIXELFORMATDESCRIPTOR pfd;
	UINT pixelformat;
	struct gldata_t *glData = (struct gldata_t *)LocalAlloc(LPTR, sizeof(struct gldata_t));

	glData->hWnd = hwnd;
	glData->hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // create an event we signal the thread with

	ZeroMemory(&pfd, sizeof(pfd)); // clear out the pixelformatdescriptor

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // fill in the required fields
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; // opengl :D~
	pfd.iPixelType = PFD_TYPE_RGBA; // rgba context
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16; // 16 bit depth buffer, after all, this is a small window :|

//	pfd.cAccumBits = 32;
//	pfd.cStencilBits = 8;

	glData->hDC = GetDC(hwnd); // get the dc so we can play with it

	pixelformat = ChoosePixelFormat(glData->hDC, &pfd); // chose.. 

	SetPixelFormat(glData->hDC, pixelformat, &pfd); // .. and set pixel format

	SetWindowLong(hwnd, GWL_USERDATA, (LONG)glData); // put our custom chunk of data in the windows userdata space

	glData->hThread = CreateThread(NULL, 0, glLogoDrawThread, glData, 0, &glData->dwThread); // create the rendering thread

	return 1;
}

void glLogoDestroyGLView(HWND hwnd){
	struct gldata_t *glData = (struct gldata_t *)GetWindowLong(hwnd, GWL_USERDATA);

	SetEvent(glData->hEvent); // tell the rendering thread to quit
	WaitForSingleObject(glData->hThread, INFINITE); // wait for it to do so

	CloseHandle(glData->hEvent);  // close the event handle we signalled the thread with
	CloseHandle(glData->hThread); // close the thread handle

	ReleaseDC(hwnd, glData->hDC); // release the windows dc we GetDC'd in glLogoCreateGLView
	
	LocalFree(glData);
}

void glLogoControlInit(){
	WNDCLASSEX wcx;

	ZeroMemory(&wcx, sizeof(wcx));
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcx.cbWndExtra = 0;
	wcx.cbClsExtra = 0;
	wcx.lpfnWndProc = glLogoControlWindowProc;
	wcx.hInstance = GetModuleHandle(NULL);
	wcx.lpszClassName = T("glLogoControl");
	wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	RegisterClassEx(&wcx);
}

GLuint texture;

int glLogoCreate(int width, int height){ // called when the window is created
	OutputDebugString(T("glLogoCreate()\n"));

	HRSRC hrTex;
	HGLOBAL hmTex;
	LPVOID texmem;

	hrTex = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_TEXTURE1), T("TEXTURE")); // find our texture map
	hmTex = LoadResource(GetModuleHandle(NULL), hrTex); // load it
	texmem = LockResource(hmTex); // get a pointer to the data

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_TEXTURE_2D);
	
	glGenTextures(1, &texture); // build a texture id for our bitmap
	glBindTexture(GL_TEXTURE_2D, texture); // bind it

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, texmem); // set texture map

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // set filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE); // texture environment
	
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0, 0, 0, 0);
	glClearDepth(1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,1 );
	return 1;
}

void glLogoDraw(){ // called whenever it needs drawing
//	OutputDebugString("glLogoDraw()\n");

	static GLfloat rotation = 0.0f;
	static GLfloat bgrot    = 0.0f;
	static GLfloat rotx, roty, rotz = 0.0f;
	static bool    clockwise = true;

	glClear(/*GL_COLOR_BUFFER_BIT | */GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -10.0f);
	glRotatef(bgrot, 0.0f, 0.0f, -1.0f);
	glScalef(9, 9, 9);

	glDisable(GL_TEXTURE_2D);
	glDepthMask(1);

	glBegin(GL_QUADS);
		//glColor4f(1, 0, 0, 0.08f);
		//glColor3ub(0,92,173);
		glColor3ub(0,105,179);
		glTexCoord2f(0, 0);
		glVertex3f(-1.0f, 1.0f, 0.0f);

		//glColor3ub(0,92,173);
		glColor3ub(0,105,179);
		glTexCoord2f(1, 0);
		glVertex3f(1.0f,1.0f, 0.0f);

		glColor4f(1, 1, 1, 0.08f);
		glTexCoord2f(1, 1);
		glVertex3f(1.0f, -1.0f, 0.0f);

		glColor4f(1, 1, 1, 0.08f);
		glTexCoord2f(0, 1);
		glVertex3f(-1.0f, -1.0f, 0.0f);
	glEnd();

	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -6.0f);

	rotx += 0.2f;
	roty += 0.9f;
	rotz += 0.3f;


	glRotatef(rotation, rotx, roty, rotz);
	glScalef(1.3f,1.3f, 1.3f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glEnable(GL_DEPTH_TEST);
	//glDepthMask(0);

	glColor4f(1, 1, 1, 1.0f);

	glBegin(GL_QUADS);
		// Front
		glTexCoord2f(0, 0);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glTexCoord2f(1, 0);
		glVertex3f(1.0f,1.0f, 1.0f);
		glTexCoord2f(1, 1);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glTexCoord2f(0, 1);
		glVertex3f(-1.0f, -1.0f, 1.0f);

		// Bottom
		glTexCoord2f(0, 0);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glTexCoord2f(1, 0);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glTexCoord2f(1, 1);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f(0, 1);
		glVertex3f(1.0f, 1.0f, 1.0f);

		// Top
		glTexCoord2f(0, 0);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glTexCoord2f(1, 0);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1, 1);
		glVertex3f(1.0f, -1.0f, -1.0f);	
		glTexCoord2f(0, 1);
		glVertex3f(1.0f, -1.0f, 1.0f);

		// Back
		glTexCoord2f(0, 0);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glTexCoord2f(1, 0);
		glVertex3f(1.0f,1.0f, -1.0f);
		glTexCoord2f(1, 1);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glTexCoord2f(0, 1);
		glVertex3f(-1.0f, -1.0f, -1.0f);

		// Right
		glTexCoord2f(0, 0);
		glVertex3f(1.0f,-1.0f,1.0f);
		glTexCoord2f(1, 0);
		glVertex3f(1.0f,-1.0f,-1.0f);
		glTexCoord2f(1, 1);
		glVertex3f(1.0f,1.0f,-1.0f);
		glTexCoord2f(0, 1);
		glVertex3f(1.0f,1.0f,1.0f);

		// Left
		glTexCoord2f(0, 0);
		glVertex3f(-1.0f,-1.0f,1.0f);
		glTexCoord2f(1, 0);
		glVertex3f(-1.0f,-1.0f,-1.0f);
		glTexCoord2f(1, 1);
		glVertex3f(-1.0f,1.0f,-1.0f);
		glTexCoord2f(0, 1);
		glVertex3f(-1.0f,1.0f,1.0f);

	glEnd();

	if( clockwise )
	{
		bgrot += 0.5f;
		if(bgrot > 360) {
			bgrot = 360;
			clockwise = false;
		}
	}
	else
	{
		bgrot -= 0.5f;
		if(bgrot < 0) {
			bgrot = 0;
			clockwise = true;
		}
	}

	rotation += 1.5f;
	if(rotation > 360)
		rotation -= 360;
}

void glLogoDestroy(){ // when destroyed, AFTER thread has been freed and context destroyed
	OutputDebugString(T("glLogoDestroy()\n"));
}
