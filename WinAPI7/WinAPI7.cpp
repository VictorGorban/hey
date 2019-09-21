#include <Windows.h>//подключение winAPI
#include <tchar.h>
#include <math.h>
#include <string>

#include <iostream>
#include <sstream>
#include <iomanip>

#include "WinAPI7.h"

using namespace std;

// globals
HINSTANCE hInstance;
HWND mainWindow;

/*task #1*/
HWND list1; // local drives
HWND button1;  // GO free clusters
HWND edit1; // free clusters

HWND button2;  // open file

/*task #2*/
HWND button3;  // open path and save file (with memory info)

// functions
// Оконная функция
LONG WINAPI WndProc(HWND hwnd, UINT Message,
	WPARAM wparam, LPARAM lparam);


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

int WINAPI WinMain(
	HINSTANCE hInst,//дескриптор текущего приложения
	HINSTANCE Prev,//не используется в win32
	LPSTR cmd,//для запуска окна в командной строке
	int mode)//режим отображения окна
{
	hInstance = hInst;
	MSG msg;

	// parent
	WNDCLASS wc;
	memset(&wc, 0, sizeof(WNDCLASS));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInst;
	wc.hbrBackground = CreateSolidBrush(0x00FFFFFF);
	wc.lpszClassName = L"My Class";
	RegisterClass(&wc);


	mainWindow = CreateWindowW(L"My Class", L"График функции loga(x)",
		WS_OVERLAPPEDWINDOW,
		00, 00, 1366, 768, NULL, NULL,
		hInstance, NULL);

	list1 = CreateWindowW(
		L"listbox", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
		30, 10, 200, 100,
		mainWindow,
		NULL, hInstance, NULL);

	// Добавляем в список несколько строк
	wchar_t disks[256];
	wchar_t* disk;
	DWORD sizebuf = 256;
	GetLogicalDriveStringsW(sizebuf, disks);
	disk = disks;
	while (*disk) {
		SendMessageW(list1, LB_ADDSTRING, 0,
			(LPARAM)disk);
		disk = disk + wcslen(disk) + 1;
	}

	delete disk;
	delete[] disks;

	edit1 = CreateWindowW(
		L"Edit",
		L"edit1",
		WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE,
		10, 40, 80, 30,
		mainWindow,
		NULL,
		hInstance,
		NULL);

	ShowWindow(mainWindow, mode);
	UpdateWindow(mainWindow);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

// Оконная функция
LONG WINAPI WndProc(HWND hwnd, UINT Message,
	WPARAM wparam, LPARAM lparam) {
	// задаются пользователем, по идее.


	switch (Message) {
		case WM_LBUTTONDOWN:
		{
			return 0;
		}
		break;
		case WM_COMMAND:
			switch (wparam) {
				case 100:
					break;

			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0); // завершаем весь message loop, как следствие весь процесс
			break;
		default:
			return DefWindowProc(hwnd, Message, wparam, lparam);
	}
	return 0;
}

