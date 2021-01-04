#pragma once
#include <windows.h>
#include <stdio.h>
#include <WinUser.h>
#include <iostream>
#include "window.h"

class Window {
public:
	LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void PopulateWindowClass(WNDCLASS *wndClass, HINSTANCE hInstance) = 0;

protected:
	/**
	* Abstract method--the routine for initial creation will be different depending on the type of window in question.
	* Event handlers may need to be created, 
	*/
	virtual LRESULT Awake(const HWND hWnd) throw(HRESULT) = 0;

	LRESULT Terminate(HWND window, HRESULT error);
};
