// TEST.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "TEST.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInstance;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_TEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEST));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEST));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TEST);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInstance = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) {
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}


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

BOOL CALLBACK DlgProc(HWND hwndDlg, UINT message,
	WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG:
		{
			SetWindowText(hwndDlg, L"Enter numbers>0, 1st<2nd.");

			double* arr = (double*)lParam;
			double xMinToSet = arr[0];
			double xMaxToSet = arr[1];

			std::wstring temp1(s2ws(doubleToStr(xMinToSet)));
			std::wstring temp2(s2ws(doubleToStr(xMaxToSet)));


			LPCWSTR str1 = temp1.c_str();
			LPCWSTR str2 = temp2.c_str();

			HWND edit1 = GetDlgItem(hwndDlg, IDC_EDIT1);
			HWND edit2 = GetDlgItem(hwndDlg, IDC_EDIT2);

			SetWindowText(edit1, str1);
			SetWindowText(edit2, str2);

			return true;
		}
		break;
		case WM_COMMAND:
		{
			if (wParam == IDOK) {
				HWND edit1 = GetDlgItem(hwndDlg, IDC_EDIT1);
				HWND edit2 = GetDlgItem(hwndDlg, IDC_EDIT2);

				LPWSTR str1 = new TCHAR[10];
				LPWSTR str2 = new TCHAR[10];

				GetWindowText(edit1, str1, 10);
				GetWindowText(edit2, str2, 10);

				double xMinToRet = _wtof(str1); // if failed then we have returned 0
				double xMaxToRet = _wtof(str2);

				if (xMinToRet <= 0) {
					showError(L"Please input correct xMin number like 12.345. More than 0");
					return false;
				}

				if (xMaxToRet <= 0) {
					showError(L"Please input correct xMax number like 12.345. More than 0");
					return false;
				}


				if (!xMinToRet < xMaxToRet) {
					showError(L"Please input xMin<xMax");
					return false;
				}




				double* arr = new double[2];
				arr[0] = xMinToRet;
				arr[1] = xMaxToRet;
				EndDialog(hwndDlg, (INT_PTR)arr);
				return true;
			}
			else return FALSE;
		}
		break;

		default: return FALSE;
	}
}


// returns double[2]
double* ShowXMinXMaxDialogDisableParent(HINSTANCE hInstance, HWND parent, double xMin, double xMax) {
	// Child

	double* arr = new double[2];
	arr[0] = xMin;
	arr[1] = xMax;

	// Pointer here, all fine
	HRSRC hrsrc = FindResourceW(hInstance,
		MAKEINTRESOURCE(IDD_DIALOG1),
		RT_DIALOG);
	// Pointer here.
	HGLOBAL hg = LoadResource(hInstance, hrsrc);
	LPDLGTEMPLATE pdt;
	pdt = (LPDLGTEMPLATE)LockResource(hg);


	INT_PTR result = NULL;

	do {
		result = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), parent, DlgProc, (LPARAM)arr);
	}
	while (result == NULL);

	arr = (double*)result;

	return arr;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	double xMin = 0.1;
	double xMax = 1;
	switch (message) {
		case WM_LBUTTONDOWN:
		{


			double* arr = ShowXMinXMaxDialogDisableParent(hInstance, hWnd, xMin, xMax);
			xMin = arr[0];
			xMax = arr[1];
		}
		break;
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId) {
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			EndPaint(hWnd, &ps);
		}
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

