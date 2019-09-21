// TEST.cpp : Defines the entry point for the application.
//
#include "pch.h"
#include "dll2.h"


// Global Variables:
HINSTANCE dllInstance;

BOOLEAN WINAPI DllMain(IN HINSTANCE hDllHandle,
	IN DWORD     nReason,
	IN LPVOID    Reserved) {
	dllInstance = hDllHandle;

	return true;
}

// Functions:
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


				if (!(xMinToRet < xMaxToRet)) {
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
double* ShowXMinXMaxDialogDisableParent(HWND parent, double xMin, double xMax) {
	// Child

	double* arr = new double[2];
	arr[0] = xMin;
	arr[1] = xMax;


	INT_PTR result = NULL;

	// по-простому не работает. 1813. Окей, будет по-сложному

	do {
		result = DialogBoxParam(dllInstance, MAKEINTRESOURCE(IDD_DIALOG1), parent, DlgProc, (LPARAM)arr);
	}
	while (result <= 0);

	arr = (double*)result;

	return arr;
}

