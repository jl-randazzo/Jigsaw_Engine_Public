#include "Windows/window.h"
#include "Windows/ViewportWindow.h"
#include "ApplicationRoot.h"
#include "ApplicationRootProperties.h"
#include "Marshalling/MarshallingUtil.h"
#include "Ref.h"
#include "Graphics/RenderContext.h"

typedef std::pair<const HWND, Window*> WINPAIR;

// Global variables
std::map<HWND, Window*> window_map;
HINSTANCE main_programInstance;

HWND main_hWnd;
WNDCLASS main_wndClass;

/**
 * Boots the main window.
 */
HRESULT Run() {
	main_programInstance = (HINSTANCE)GetModuleHandle(NULL);

	Jigsaw::File_IO::FILE_DATA f_data = Jigsaw::File_IO::GetResourceFile("application_root_properties.json");
	const_cast<ApplicationRootProperties&>(ApplicationRootProperties::Get()) = *Jigsaw::Marshalling::MarshallingUtil::Unmarshal<ApplicationRootProperties>(f_data).get();
	std::fclose(f_data.file);

	// Initialize values for window creation
	int xPos = 0;
	int yPos = 0;
	int width = 1280;
	int height = 720;
	
	Jigsaw::Ref<Jigsaw::Graphics::RenderContext> render_context = Jigsaw::Graphics::RenderContext::Init();
	ViewportWindow* viewport_window = new ViewportWindow(static_cast<double>(width) / static_cast<double>(height), render_context);
	viewport_window->PopulateWindowClass(&main_wndClass, main_programInstance);
	main_wndClass.lpfnWndProc = (WNDPROC)MessageHandler;

	ATOM class_registry = RegisterClass(&main_wndClass);
	if (!class_registry) {
		return HRESULT_FROM_WIN32(GetLastError());
	}

	HWND parent = NULL;
	HMENU hMenu = NULL;

	main_hWnd = CreateWindow(main_wndClass.lpszClassName, "Main_Window", WS_DLGFRAME | WS_SYSMENU | WS_OVERLAPPEDWINDOW ,
		xPos, yPos, width, height, parent, hMenu, main_programInstance, NULL);

	// if failed to create window, thrown an error
	if (main_hWnd == 0) {
		return HRESULT_FROM_WIN32(GetLastError());
	}

	// insert viewport into application window_map and finalize/show the window 
	window_map.insert(WINPAIR(main_hWnd, static_cast<Window*>(viewport_window)));
	ShowWindow(main_hWnd, SW_SHOWNORMAL);
	std::cout << "Window created with handle: " << main_hWnd << std::endl;

	MainLoop(Jigsaw::Orchestration::MainApplicationOrchestrator::GetOrchestrator(render_context, Jigsaw::Ref<ViewportWindow>(viewport_window)));

	return 0;
}

/**
 * Main loop for translating and dispatching messages
 */
void MainLoop(Jigsaw::Orchestration::MainApplicationOrchestrator* const application) {
	MSG message;
	PeekMessage(&message, NULL, 0, 0, PM_NOREMOVE);
	while(message.message != WM_QUIT) {
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		} else {
		}

		application->Orchestrate();
	}
}

LRESULT TerminateWindow(HWND hWnd, HRESULT error) {
	DestroyWindow(hWnd);
	UnregisterClass(main_wndClass.lpszClassName, main_programInstance);
	return error;
}

LRESULT MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (window_map.find(hWnd) == window_map.end()) {
		std::cout << "Unhandled window message: " << uMsg << std::endl;
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	} else {
		return window_map.at(hWnd)->HandleMessage(hWnd, uMsg, wParam, lParam);
	}
}
