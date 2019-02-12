/*
* 3D Vision CM Unleashed
* Copyright (c) Helifax 2019
*/

//Originally created 2006-06-08 by Kirill V. Lyadvinsky
// http://www.opensource.org/licenses/cddl1.php

#ifndef __SPLASHWND_H_
#define __SPLASHWND_H_

#include <gdiplus.h>

class CSplashWnd
{
public:
    CSplashWnd(HWND hParent = NULL);
    ~CSplashWnd();

    void Show();
    void Hide();

    HWND GetWindowHwnd() const
    {
        return m_hSplashWnd;
    };

private:
    CSplashWnd(const CSplashWnd&);
    CSplashWnd& operator=(const CSplashWnd&);

    static LRESULT CALLBACK SplashWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static unsigned int __stdcall SplashThreadProc(void* lpParameter);

    HANDLE m_hThread;
    unsigned int m_ThreadId;
    HANDLE m_hEvent;
    Gdiplus::Image* m_pImage;
    HWND m_hSplashWnd;
    HWND m_hProgressWnd;
    HWND m_hParentWnd;
    std::wstring m_StringToPrint;
    UINT_PTR m_TimerId;
};

#endif  //__SPLASHWND_H_
