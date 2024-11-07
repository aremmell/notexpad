
// Hyperlink.cpp -- Implementation of CHyperlink class
// Ryan Lederman -- ryan@winprog.org

#include "stdafx.h"
#include "Hyperlink.h"
#include <tchar.h>
#include <stdlib.h>

CHyperlink::CHyperlink()
{
  
}
CHyperlink::~CHyperlink()
{
  Destroy();
}

bool CHyperlink::Create( PHYPERLINKSTRUCT phs )
{
	int iFontBold = 0;
	int iFontUnderline = 0;
	int iHighlightFontUnderline = 0;

	if( !IsWindow( phs->hWndParent ) )
		return false;

	if( phs->width <= 0 || phs->height <= 0 )
		return false;

	if( !phs->szFontName )
		return false;

  m_bMouseOver = false;
  m_iLastState = S_MOUSENOTOVER;
  m_bFwdMouseMsgs = phs->bFwdMouseMsgs;

	m_hWnd = CreateWindow( _T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE, phs->coordX, phs->coordY, phs->width, phs->height,
						   phs->hWndParent, reinterpret_cast<HMENU>(phs->ctlID), GetModuleHandle( NULL ), NULL );

	if( m_hWnd == NULL )
		return false;

	if( SetProp( m_hWnd, _T("CLASSPOINTER"), (HANDLE)this ) == 0 )
		return false;

	SetWindowLongPtr( m_hWnd, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(HyperlinkProc) );

	phs->bBold ? iFontBold = FW_BOLD : iFontBold = FW_NORMAL;

	m_hFont = CreateFont( phs->fontsize ,0,0,0,iFontBold,0, phs->bUnderline,0,0,0,0,0,0, phs->szFontName );

	if( m_hFont == NULL )
		return false;

	if( phs->bUseBg )
	{
		m_hBrushBackground = CreateSolidBrush( phs->clrBack );
		m_bUseBg = true;
	}
	else
	{	
		m_bUseBg = false;
	}
	
	m_bUseCursor = phs->bUseCursor; 
	
	if( m_bUseCursor )
		m_hCursor = phs->hCursor;

	m_rgbForeground = phs->clrText;
	m_rgbHighlight  = phs->clrHilite;
	m_rgbBackground = phs->clrBack;

	if( phs->pFn )
		m_pFn = phs->pFn;

	m_szBuffer = (TCHAR*)calloc( (_tcslen( phs->szCaption )*sizeof(TCHAR)) + sizeof(TCHAR), sizeof(TCHAR) );

	if( m_szBuffer == NULL )
		return false;

	_tcscpy( m_szBuffer, phs->szCaption );

  m_pUserData = phs->pUserData;

	return true;
}

LRESULT CALLBACK CHyperlink::HyperlinkProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	void *pThisClass = (void *)GetProp( hWnd, _T("CLASSPOINTER") );

	switch( msg )
	{
		case WM_PAINT: 
			((CHyperlink*)pThisClass)->OnDraw(); 
			break;
		case WM_MOUSEMOVE: ((CHyperlink*)pThisClass)->OnMouseMove(); break;
    case WM_MOUSELEAVE: ((CHyperlink*)pThisClass)->OnMouseLeave(); break;
		case WM_LBUTTONDOWN: 
			if( ((CHyperlink*)pThisClass)->m_pFn )
				((CHyperlink*)pThisClass)->m_pFn( ((CHyperlink*)pThisClass)->GetUserData() );
			break;
		case WM_CAPTURECHANGED:
			((CHyperlink*)pThisClass)->OnMouseMove(); 
			break;
    case WM_KILLFOCUS:
      ((CHyperlink*)pThisClass)->OnMouseMove(); 
			break;
		case WM_SETCURSOR:
			if( ((CHyperlink*)pThisClass)->m_bUseCursor )
			{
				((CHyperlink*)pThisClass)->m_hStockCursor = SetCursor( ((CHyperlink*)pThisClass)->m_hCursor );
				return TRUE;
      }
			return FALSE;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

void CHyperlink::OnDraw( void )
{
	PAINTSTRUCT ps = {0};
	HDC			dc;
	RECT		winRect = {0};

	dc = BeginPaint( m_hWnd, &ps );
	GetClientRect( m_hWnd, &winRect );

	if( m_bUseBg )
	{
		FillRect( dc, &winRect, m_hBrushBackground );
		SetBkColor( dc, m_rgbBackground );
	}
	else
  {
		SetBkMode( dc, TRANSPARENT );
	}

	SelectObject( dc, m_hFont );

	if( m_bMouseOver )
	{
		SetTextColor( dc, m_rgbHighlight );
	}
	else
	{
		SetTextColor( dc, m_rgbForeground );
	}

	DrawTextEx( dc, m_szBuffer, static_cast<int>(_tcslen( m_szBuffer )), &winRect, DT_EDITCONTROL | DT_WORDBREAK
              | DT_CENTER | DT_VCENTER, NULL );

	EndPaint( m_hWnd, &ps );
}

void CHyperlink::OnMouseMove()
{
	RECT  winRect = {0};
	POINT pt	  = {0};
  HWND hWndParent = GetParent(m_hWnd);

	GetCursorPos( &pt );

	GetWindowRect( m_hWnd, &winRect );

	if( PtInRect( &winRect, pt ) )
  {
    _RegisterMouseEvent();
    SetCapture(m_hWnd);
		m_bMouseOver = true;

		if( m_iLastState != S_MOUSEOVER )
		{

			InvalidateRect( m_hWnd, NULL, TRUE );
			UpdateWindow( m_hWnd );

      // If we're transparent, we need to invalidate
      // the rectangle of this control on the parent window
      // so that no ghosting occurs.
      if (!m_bUseBg) {

        RECT rcParent = {0};

        POINT ptUpperLeft   = {0};
        POINT ptBottomRight = {0};

        ptBottomRight.x = winRect.right;
        ptBottomRight.y = winRect.bottom;

        ClientToScreen(m_hWnd, &ptUpperLeft);
        ScreenToClient(hWndParent, &ptUpperLeft);

        rcParent.left = ptUpperLeft.x;
        rcParent.top = ptUpperLeft.y;
        rcParent.right = ptBottomRight.x;
        rcParent.bottom = ptBottomRight.y;

        InvalidateRect(hWndParent, &rcParent, FALSE);
        UpdateWindow(hWndParent);

      }

		}
		m_iLastState = S_MOUSEOVER;
	}
	else
	{

    ReleaseCapture();
		m_bMouseOver = false;

		if( m_iLastState != S_MOUSENOTOVER )
		{
			InvalidateRect( m_hWnd, NULL, TRUE );
			UpdateWindow( m_hWnd );

      // If we're transparent, we need to invalidate
      // the rectangle of this control on the parent window
      // so that no ghosting occurs.
      if (!m_bUseBg) {

        RECT rcParent = {0};

        POINT ptUpperLeft   = {0};
        POINT ptBottomRight = {0};

        ptBottomRight.x = winRect.right;
        ptBottomRight.y = winRect.bottom;

        ClientToScreen(m_hWnd, &ptUpperLeft);
        ScreenToClient(hWndParent, &ptUpperLeft);

        rcParent.left = ptUpperLeft.x;
        rcParent.top = ptUpperLeft.y;
        rcParent.right = ptBottomRight.x;
        rcParent.bottom = ptBottomRight.y;

        InvalidateRect(hWndParent, &rcParent, FALSE);
        UpdateWindow(hWndParent);

      }

		}
		m_iLastState = S_MOUSENOTOVER;
  }

  if (m_bFwdMouseMsgs) {

    ScreenToClient(hWndParent, &pt);
    PostMessage(hWndParent, WM_MOUSEMOVE, 0, MAKELONG(pt.x, pt.y));

  }

}

void CHyperlink::OnMouseLeave() {

  if (m_bFwdMouseMsgs) {

    PostMessage(GetParent(m_hWnd), WM_MOUSELEAVE, reinterpret_cast<WPARAM>(this), 0);

  }

}

void CHyperlink::_RegisterMouseEvent(bool bRegister) {

  TRACKMOUSEEVENT tme = {0};

  tme.cbSize = sizeof(tme);
  tme.dwFlags = TME_LEAVE;

  if (!bRegister) { tme.dwFlags |= TME_CANCEL; }

  TrackMouseEvent(&tme);

}

void CHyperlink::Destroy( void )
{
	DeleteObject( m_hFont );
	DeleteObject( m_hBrushBackground );
	DestroyWindow( m_hWnd );

	if( m_bUseCursor )
		DeleteObject( m_hCursor );

  if( m_szBuffer ) {

		free( m_szBuffer );
    m_szBuffer = NULL;

  }

  _RegisterMouseEvent(false);

}

