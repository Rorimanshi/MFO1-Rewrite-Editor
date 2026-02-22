#ifndef _MAIN_WINDOW
#define _MAIN_WINDOW

#include<iostream>
#include<windows.h>

class MainWindow {
public:
	MainWindow(
        HINSTANCE hInst,
        wchar_t* pArgs,
        const wchar_t* wndClassName,
		int width, int height
    );
	~MainWindow();

	void Kill();
	bool ProcessMsg();
private:
    void CreateButton(HWND hParent, int x, int y, int w, int h, INT_PTR id, const wchar_t* name) const;

	static LRESULT CALLBACK _MsgSetup(HWND hWNd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK _MsgThunk(HWND hWNd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT MsgHandle(HWND hWNd, UINT msg, WPARAM wParam, LPARAM lParam);

	HINSTANCE hInst = nullptr;
    HWND hWnd = nullptr;
	std::wstring args;
	const wchar_t* wndClassName;
};

#endif