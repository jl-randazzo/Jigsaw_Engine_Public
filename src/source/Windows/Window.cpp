#include "window.h"

LRESULT Window::Terminate(HWND hWnd, HRESULT error) {
	char class_name[256];
	GetClassName(hWnd, class_name, 255);
	HINSTANCE main_programInstance = (HINSTANCE)GetModuleHandle(NULL);

	DestroyWindow(hWnd);
	WNDCLASS wndClass;

	GetClassInfo(main_programInstance, class_name, &wndClass);
	UnregisterClass(wndClass.lpszClassName, main_programInstance);
	return error;
}

/**
* Signal handler for generic Window class 
*/
LRESULT Window::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_SHOWWINDOW:
		try {
			return Awake(hWnd);
		} catch (HRESULT e) {
			return Terminate(hWnd, e);
		}
	case WM_CLOSE:
		return Terminate(hWnd, 0);
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
