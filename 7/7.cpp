#include <Windows.h>//ïîäêëþ÷åíèå winAPI
#include <tchar.h>
#include <math.h>
#include <string>

#include <iostream>
#include <sstream>
#include <iomanip>

#include "7.h"

using namespace std;



HWND hWnd;
HINSTANCE hInst;

HWND hLabel1;
HWND hLabel2;

HWND hEdit1;
HWND hEdit2;
HWND hList;
HWND hButton1;

HWND hButton2;
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

std::string ulongToStr(unsigned long x) {
	std::stringstream ss;
	// Set Fixed -Point Notation, then presision
	ss << x;

	std::string str = ss.str();
	return str;
}

std::string ulonglongToStr(unsigned long long x) {
	std::stringstream ss;
	// Set Fixed -Point Notation, then presision
	ss << x;

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

	hLabel1 = CreateWindowW(L"static", NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		10, 10, 120, 16,
		hWnd, NULL,
		hInstance, NULL);
	SetWindowText(hLabel1, L"Select drive:");

	hList = CreateWindow(
		L"listbox",
		NULL,
		WS_CHILD | WS_VISIBLE | LBS_STANDARD | LBS_NOTIFY,
		10, 30, 120, 30,
		hWnd,
		(HMENU)300, //id
		hInstance,
		NULL);
	{ // to hide
		wchar_t disks[256];
		wchar_t* disk;
		DWORD sizebuf = 256;
		GetLogicalDriveStringsW(sizebuf, disks);
		disk = disks;
		while (*disk) {
			SendMessage(hList, LB_ADDSTRING, 0,
				(LPARAM)disk);
			disk = disk + wcslen(disk) + 1;
		}

		/*delete[] disk;
		delete[] disks;*/
	}

	hLabel2 = CreateWindowW(L"static", NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		10, 60, 120, 16,
		hWnd, NULL,
		hInstance, NULL);
	SetWindowText(hLabel2, L"Clusters number:");

	hEdit1 = CreateWindow(
		L"Edit",
		L"",
		WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE,
		10, 80, 120, 20,
		hWnd,
		NULL,
		hInstance,
		NULL);
	SetWindowText(hEdit1, L"");

	hButton1 = CreateWindow(
		L"Button",
		NULL,
		WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE,
		250, 10, 120, 20,
		hWnd,
		(HMENU)100,//id,
		hInstance,
		NULL);
	SetWindowText(hButton1, L"Get file attributes");

	hButton2 = CreateWindow(
		L"Button",
		NULL,
		WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE,
		450, 10, 225, 20,
		hWnd,
		(HMENU)200,//id,
		hInstance,
		NULL);
	SetWindowText(hButton2, L"Select file to save memory info in");



	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

// thanks to social.msdn. Much better than msdn itself.
TCHAR* GetSelection(HWND list) {

	// Get current selection index in listbox
	int itemIndex = (int)SendMessage(list, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	if (itemIndex == LB_ERR) {
		// No selection
		return 0;
	}

	// Get length of text in listbox
	int textLen = (int)SendMessage(list, LB_GETTEXTLEN, (WPARAM)itemIndex, 0);

	// Allocate buffer to store text (consider +1 for end of string)
	TCHAR* textBuffer = new TCHAR[textLen + 1];

	// Get actual text in buffer
	SendMessage(list, LB_GETTEXT, (WPARAM)itemIndex, (LPARAM)textBuffer);

	// Show it
	//MessageBox(NULL, textBuffer, L"Selected Text:", MB_OK);

	return textBuffer;
}

LONG WINAPI WndProc(HWND hwnd, UINT Message,
	WPARAM wparam, LPARAM lparam) {


	switch (Message) {
		case WM_LBUTTONDOWN:
		{
			return 0;
		}
		break;
		case WM_COMMAND:
		{
			int wmId = LOWORD(wparam);
			int wmEvent = HIWORD(wparam);

			switch (wmId) {
				case 300:
				{
					if (wmEvent == LBN_SELCHANGE) {
						//showNotification(L"Selection changed event fired");
						TCHAR* text = GetSelection(hList);
						if (text != NULL) {

							DWORD bytes_per_cluster = 0;

							DWORD sectors_per_cluster = 0;
							DWORD bytes_per_sector = 0;
							DWORD free_clusters = 0;
							DWORD dummy = 0;

							GetDiskFreeSpaceW(text, &sectors_per_cluster, &bytes_per_sector, &free_clusters, &dummy);

							wstring str = s2ws(ulongToStr(free_clusters));

							//showNotification(str.c_str());
							SetWindowTextW(hEdit1, str.c_str());
						}
						else {
							showError(L"Can't get selection");
						}
					}
				}
				break;
				case 100:
				{
					OPENFILENAME ofn;
					// a another memory buffer to contain the file name
					TCHAR szFile[255];
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = NULL;
					ofn.lpstrFile = szFile;
					ofn.lpstrFile[0] = '\0';
					ofn.nMaxFile = sizeof(szFile);
					ofn.lpstrFilter = L"All\0*.*\0";
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = NULL;
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
					if (GetOpenFileName(&ofn)) {
						LPCWSTR path = ofn.lpstrFile;
						//showNotification(path);
						std::wstring result = path;
						result += L": \n";

						ULONG attrs = GetFileAttributes(path);
						if (attrs & FILE_ATTRIBUTE_ARCHIVE) {
							result += L"archived; ";
						}
						else {
							result += L"not archived; ";
						}
						if (attrs & FILE_ATTRIBUTE_COMPRESSED) {
							result += L"compressed; ";

						}
						else {
							result += L"not compressed; ";
						}
						if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
							// can't get in here.
							result += L"directory; ";
						}
						else {
							result += L"not directory; ";
						}
						if (attrs & FILE_ATTRIBUTE_HIDDEN) {
							result += L"hidden; ";
						}
						else {
							result += L"not hidden; ";
						}
						if (attrs & FILE_ATTRIBUTE_READONLY) {
							result += L"readonly; ";
						}
						else {
							result += L"not readonly; ";
						}

						showNotification(result.c_str());
					}
					else {
						//showError(L"Can't get file attributes");
						return true;
					}
				}
				break;
				case 200:
				{
					OPENFILENAME ofn;
					// a another memory buffer to contain the file name
					TCHAR szFile[255];
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = NULL;
					ofn.lpstrFile = szFile;
					ofn.lpstrFile[0] = '\0';
					ofn.nMaxFile = sizeof(szFile);
					ofn.lpstrFilter = L"Text\0*.txt\0";
					ofn.nFilterIndex = 1;
					ofn.lpstrDefExt = L".txt";
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = NULL;
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
					if (GetSaveFileName(&ofn)) {
						LPCWSTR path = ofn.lpstrFile;
						
						//showNotification(path);
						HANDLE hOut;
						hOut = CreateFile(path, GENERIC_WRITE, NULL, NULL,
							CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

						if (hOut == INVALID_HANDLE_VALUE) {
							showError(L"can't create the file");
							return -2;
						}

						std::wstring result = L"";
						result += L"Total page file (MB) is ";

						MEMORYSTATUSEX statex;
						statex.dwLength = sizeof(statex);
						GlobalMemoryStatusEx(&statex);

						result += s2ws(ulonglongToStr(statex.ullTotalPageFile / 1024 / 1024));
						result += L" and ";


						result += L"Usage of RAM (%) is ";
						result += s2ws(ulonglongToStr(statex.dwMemoryLoad));
						result += L"%";

						//showNotification(result.c_str());

						if (WriteFile(hOut, result.c_str(), result.length() * 2, NULL, NULL)) { // *2 because of UNICODE
							CloseHandle(hOut);
							STARTUPINFO si = { sizeof(si) };
							PROCESS_INFORMATION pi;
							wstring str(L"notepad");
							str += L" ";
							str += path;

							bool isCreated = CreateProcessW(0,   // Name of program to execute
								(LPWSTR)str.c_str(),                  // Command line
								NULL,                      // Process handle not inheritable
								NULL,                      // Thread handle not inheritable
								FALSE,                     // Set handle inheritance to FALSE
								0,                         // No creation flags
								NULL,                      // Use parent's environment block
								NULL,                      // Use parent's starting directory 
								&si,                       // Pointer to STARTUPINFO structure
								&pi);  // pointer to procesInfo structure

							if (isCreated) {
								// Wait until child process exits.
								WaitForSingleObject(pi.hProcess, INFINITE);

								// Close process and thread handles. 
								CloseHandle(pi.hProcess);
								CloseHandle(pi.hThread);
							}
							else {
								showError(s2ws(ulongToStr(GetLastError())).c_str());
							}
						}
						else {
							showError(L"Can't write the file");
						}

					}
					else {
						//showError(L"Can't get file attributes");
						return true;
					}
				}
				break;
			}
		}
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, Message, wparam, lparam);
	}
	return 0;
}
