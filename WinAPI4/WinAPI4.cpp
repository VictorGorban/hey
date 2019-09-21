#include <Windows.h>//ïîäêëþ÷åíèå winAPI
#include <tchar.h>
#include <math.h>
#include <string>

#include <iostream>
#include <sstream>
#include <iomanip>

#include "WinAPI4.h"

using namespace std;

//typedef void (WINAPI* cfunc)(HDC hdc, RECT rectClient,
//	double xMin, double xMax);

typedef int(*TdrawGraph)(HDC hdc, RECT rectClient,
	double xMin, double xMax);

typedef double* (*TShowXMinXMaxDialogDisableParent)(HWND parent, double xMin, double xMax);

double dx = 0.1;
const double a = 10; // îñíîâàísfdvdfvèå ëîãàðèôìà // hello github
double xMin = 0.1;
double xMax = 20;

HMODULE dll1;

TdrawGraph DrawGraph;



HDC paintHdc = 0;
HWND hWnd;
HWND hChildWnd;
HINSTANCE hInst;

HWND hEdit1;
HWND hEdit2;
HMENU hMenu;

LONG WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI WndProc1(HWND, UINT, WPARAM, LPARAM);

std::wstring s2ws(const std::string& s) {
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

std::string doubleToStr(double x, int precision = 2) {
	std::stringstream ss;
	// Set Fixed -Point Notation, then presision
	ss << std::fixed << std::setprecision(precision) << x;

	std::string str = ss.str();
	return str;
}

void showError(LPCWSTR message) {
	HWND hWnd = GetForegroundWindow();
	MessageBox(hWnd, message, L"Error", MB_OK);
}

void showNotification(LPCWSTR message) {
	HWND hWnd = GetForegroundWindow();
	MessageBox(hWnd, message, L"Notification", MB_OK);
}

// main function
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	hInst = hInstance;

	dll1 = LoadLibraryW(L"Dll1.dll");
	if (dll1 == nullptr) {
		showError(L"Enable to load dll1");
		exit(-1);
	}
	DrawGraph = (TdrawGraph)GetProcAddress((HMODULE)dll1, "DrawGraph");
	if (DrawGraph == nullptr) {
		showError(L"Enable to load function from dll1");
		exit(-1);
	}



	MSG msg;

	// parent
	WNDCLASS wc;
	memset(&wc, 0, sizeof(WNDCLASS));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = CreateSolidBrush(0x00FFFFFF);
	wc.lpszClassName = L"My Class";
	RegisterClass(&wc);

	hMenu = CreateMenu();
	AppendMenuW(hMenu, MF_STRING, 100, L"Dialog");
	AppendMenuW(hMenu, MF_STRING, 101, L"Another item1");
	AppendMenuW(hMenu, MF_STRING, 101, L"Another item2");

	hWnd = CreateWindowW(L"My Class", L"loga(x) graph",
		WS_OVERLAPPEDWINDOW,
		00, 00, 1366, 768, NULL, hMenu,
		hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void RepaintGraph(HWND hwnd) {
	RECT rect;
	HDC hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);

	DrawGraph(hdc, rect, xMin, xMax); // ïîñòðîåíèå ãðàôèêà
}

int ShowDialog() {
	bool thatsBad = false;
	HMODULE dll2 = LoadLibraryW(L"Dll2.dll");
	if (dll2 == nullptr) {
		showError(L"Enable to load dll2");
		thatsBad = true;
	}
	TShowXMinXMaxDialogDisableParent ShowXMinXMaxDialogDisableParent = (TShowXMinXMaxDialogDisableParent)GetProcAddress((HMODULE)dll2, "ShowXMinXMaxDialogDisableParent");
	if (ShowXMinXMaxDialogDisableParent == nullptr) {
		showError(L"Enable to load function from dll2");
		thatsBad = true;
	}

	if (thatsBad) {

		EnableMenuItem(hMenu, 100, MF_GRAYED);
		return -1;
	}
	else {
		EnableMenuItem(hMenu, 100, MF_ENABLED);
	}

	/*
	// It's all right, I can't do it here: I'm getting 'undeclared identifier IDD_DIALOG1.'
	HRSRC hrsrc = FindResourceW(hInst,
		MAKEINTRESOURCE(IDD_DIALOG1),
		RT_DIALOG);*/

	double* arr = ShowXMinXMaxDialogDisableParent(hWnd, xMin, xMax);
	xMin = arr[0];
	xMax = arr[1];

	RepaintGraph(hWnd);
	InvalidateRect(hWnd, NULL, TRUE);

	return 0;
}

// Îêîííàÿ ôóíêöèÿ
LONG WINAPI WndProc(HWND hwnd, UINT Message,
	WPARAM wparam, LPARAM lparam) {
	// çàäàþòñÿ ïîëüçîâàòåëåì, ïî èäåå.


	switch (Message) {
		case WM_LBUTTONDOWN:
		{
			ShowDialog();
			return 0;
		}
		break;
		case WM_COMMAND:
			switch (wparam) {
				case 100:
					return ShowDialog();
					break;
				case 999:
					RepaintGraph(hwnd);
					InvalidateRect(hwnd, NULL, TRUE);
					break;

			}
			break;
		case WM_PAINT: // òî÷íî ðàíüøå âñåãî îñòàëüíîãî
			PAINTSTRUCT ps;
			paintHdc = BeginPaint(hwnd, &ps);
			RepaintGraph(hwnd);
			EndPaint(hwnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, Message, wparam, lparam);
	}
	return 0;
}
