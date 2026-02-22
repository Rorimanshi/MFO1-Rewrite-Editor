#include"MainWindow.hpp"
#include"LauncherInfo.hpp"

#include"assert.h"
#include"shellapi.h"

MainWindow::MainWindow(HINSTANCE hInst, wchar_t* pArgs, const wchar_t* wndClassName,
	int width, int height)
	: wndClassName(wndClassName)
{
	WNDCLASSEXW wcex = {
		sizeof(WNDCLASSEX),
		CS_HREDRAW | CS_VREDRAW,
		_MsgSetup,
		0, 0,
		hInst,
		nullptr, nullptr, nullptr, nullptr,
		wndClassName, nullptr
	};
	RegisterClassExW(&wcex);

	hWnd = CreateWindowW(wndClassName, L"MFO1 Launcher", WS_OVERLAPPEDWINDOW,
		500, 500, width, height, nullptr, nullptr, hInst, this);
	assert(hWnd != nullptr);

	UpdateWindow(hWnd);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}
MainWindow::~MainWindow() {
	UnregisterClassW(wndClassName, hInst);
}

void MainWindow::Kill() {
	PostQuitMessage(0);
}

bool MainWindow::ProcessMsg() {
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT) {
			return false;
		}
	}
	return true;
}

void MainWindow::CreateButton(HWND hParent, int x, int y, int w, int h, INT_PTR id, const wchar_t* name) const{
    HWND hButton = CreateWindowW(
        L"BUTTON",
        name,
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x, y, w, h,
        hParent,
        (HMENU)id,
        (HINSTANCE)GetWindowLongPtr(hWnd , GWLP_HINSTANCE),
        nullptr
    );
}

LRESULT CALLBACK MainWindow::_MsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_CREATE) {
		const CREATESTRUCT* const pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		MainWindow* const pWnd = reinterpret_cast<MainWindow*>(pCreate->lpCreateParams);
		assert(pWnd != nullptr);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&MainWindow::_MsgThunk));
		return pWnd->MsgHandle(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT CALLBACK MainWindow::_MsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	MainWindow* const pWnd = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->MsgHandle(hWnd, msg, wParam, lParam);
}
LRESULT MainWindow::MsgHandle(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
    case WM_CREATE:
        CreateButton(hWnd, 50, 50, 100, 50, LauncherInfo::ID_BNT_PLAY, L"Uruchom MFO1");
        CreateButton(hWnd, 50, 150, 100, 50, LauncherInfo::ID_BNT_EDITOR, L"Uruchom Edytor");
        return 0;
    case WM_COMMAND:
        switch(LOWORD(wParam)){
            case LauncherInfo::ID_BNT_PLAY:
                MessageBoxW(hWnd, L"Uruchamianie gry...", L"Launcher", MB_OK);
                
                break;
            case LauncherInfo::ID_BNT_EDITOR:
                MessageBoxW(hWnd, L"Uruchamianie edytora...", L"Launcher", MB_OK);
                ShellExecuteW(
					hWnd,
					L"open",
					L"localserver\\build\\main.exe",
					L"-Meditor",
					nullptr,
					SW_SHOW
				);
				this->Kill();
                break;
        }
        break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}