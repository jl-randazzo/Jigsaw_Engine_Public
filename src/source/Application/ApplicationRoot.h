#pragma once
#include "MainApplicationOrchestrator.h"
#include <map>

HRESULT Run();
void MainLoop(Jigsaw::Orchestration::MainApplicationOrchestrator* application);
LRESULT TerminateWindow(HWND window, HRESULT error);
LRESULT MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

