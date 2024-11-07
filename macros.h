
//
// macros.h
// NoteXpad 2.0
// Ryan Lederman, ryan@winprog.org
//
#ifndef _MACROS_H
#define _MACROS_H

#define MSGBOX_ERROR(x)   (MessageBox( GetForegroundWindow(), x, T("NoteXpad v2.0 - Error"), MB_OK | MB_ICONSTOP ))
#define MSGBOX_NOTIFY(x)  (MessageBox( GetForegroundWindow(), x, T("NoteXpad v2.0 - Notice"), MB_OK | MB_ICONINFORMATION ))
#define MSGBOX_QUESTION(x)(MessageBox( GetForegroundWindow(), x, T("NoteXpad v2.0 - Question"), MB_YESNO | MB_ICONQUESTIONMARK))
#define T(x) TEXT(x)

#endif /* _MACROS_H */