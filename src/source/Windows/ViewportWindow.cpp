#include "ViewportWindow.h"
#include <cstdlib>
using namespace Microsoft::WRL;

/**
 * Fills the main window class with any default details. This function is isolated to eventually support loading and other features.
 */
void ViewportWindow::PopulateWindowClass(WNDCLASS* wndClass, HINSTANCE hInstance) {
	wndClass->style = CS_DBLCLKS;
	wndClass->lpszMenuName = NULL;
	wndClass->lpszClassName = "Viewport_window_class";
	wndClass->hInstance = hInstance;
	wndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass->hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
}

LRESULT ViewportWindow::Awake(const HWND hWnd) {
	render_context->CreateWindowSwapchain(hWnd);
	return 0;
}

