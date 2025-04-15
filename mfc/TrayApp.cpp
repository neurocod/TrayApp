// TrayApp.cpp : Defines the entry point for the application.
#include "framework.h"
#include "TrayApp.h"
#include <utility> // swap
#include <string>
#define MAX_LOADSTRING 100

HINSTANCE g_hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

struct SleepPreventer {
	//seconds are from 0 to 86400, it will work inside that interval
	// if start>end - it will work outside that interval
	static int _secondStart;
	static int _secondEnd;
	static bool shouldPreventNow() {
		SYSTEMTIME time;
		GetSystemTime(&time);
		const int secNow = time.wSecond + time.wMinute * 60 * time.wHour * 60 * 60;
		int start = _secondStart;
		int end = _secondEnd;
		if (start < end)
			return start <= secNow && secNow <= end;
		std::swap(start, end);
		return secNow<=start || secNow >= end;
	}

	static void preventSLeepByMouseMove() {
		INPUT i = {};
		i.type = INPUT_MOUSE; //INPUT_KEYBOARD
		MOUSEINPUT & mi = i.mi;
		mi.dwFlags = MOUSEEVENTF_MOVE;
		static bool b = true;
		b = !b;
		const int dx = 1;
		mi.dx = b ? dx : -dx;
		SendInput(1, &i, sizeof(i));

	}

	static void CALLBACK onTimer(HWND hwnd, UINT msg, UINT_PTR idTimer, DWORD dwTime) {
		if (shouldPreventNow())
			preventSLeepByMouseMove();
	}

	static void startTimer(HWND hWnd) {
		const UINT_PTR timerId = 1;
		SetTimer(hWnd, timerId, 100, &SleepPreventer::onTimer);
	}
	static void parseCommandLine(const std::wstring & s) {
		// parses "num1 num2" when num1 is _secondStart and num2 is _secondEnd
		if (s.empty())
			return;
		_secondStart = _wtoi(s.c_str());
		const size_t pos = s.find(' ');
		if (pos != s.npos) {
			std::wstring second = s.substr(pos);
			_secondEnd = _wtoi(second.c_str());	
		}
	}
};
int SleepPreventer::_secondStart = 0;
int SleepPreventer::_secondEnd = 86400;

struct SleepPreventerApp {
	static BOOL initInstance(HINSTANCE hInstance, int nCmdShow) {
		g_hInst = hInstance; // Store instance handle in our global variable

		HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
		if (!hWnd)
			return FALSE;
		SleepPreventer::startTimer(hWnd);
		//ShowWindow(hWnd, nCmdShow);
		//UpdateWindow(hWnd);

		return TRUE;
	}

	static ATOM registerClass(HINSTANCE hInstance) {
		WNDCLASSEXW wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAYAPP));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TRAYAPP);
		wcex.lpszClassName = szWindowClass;
		wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		return RegisterClassExW(&wcex);
	}

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		switch (message) {
		case WM_COMMAND: { // process the application menu
			const int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId) {
			case IDM_ABOUT:
				DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, onDialogBoxAbout);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
		case WM_PAINT: { // Paint the main window
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			EndPaint(hWnd, &ps);
		}
		break;
		case WM_DESTROY: // post a quit message and return
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	static INT_PTR CALLBACK onDialogBoxAbout(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
		UNREFERENCED_PARAMETER(lParam);
		switch (message)
		{
		case WM_INITDIALOG:
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
		}
		return (INT_PTR)FALSE;
	}

	static int main(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow) {
		SleepPreventer::parseCommandLine(lpCmdLine);

		LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		LoadStringW(hInstance, IDC_TRAYAPP, szWindowClass, MAX_LOADSTRING);
		registerClass(hInstance);

		if (!initInstance(hInstance, nCmdShow))
			return 0;

		HACCEL hAccelTable = LoadAcceleratorsW(hInstance, MAKEINTRESOURCE(IDC_TRAYAPP));
		MSG msg;
		while (GetMessageW(&msg, nullptr, 0, 0)) {
			if (!TranslateAcceleratorW(msg.hwnd, hAccelTable, &msg)) {
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}
		return (int)msg.wParam;
	}
}; // SleepPreventerApp

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	(void)hPrevInstance;
	(void)lpCmdLine;
	return SleepPreventerApp::main(hInstance, lpCmdLine, nCmdShow);
}
