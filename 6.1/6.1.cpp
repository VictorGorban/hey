#include "math.h"
#include <windows.h>
#include <tlhelp32.h> 

#include <string>

#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

HWND hWnd;
HWND buttonGO;
HWND resultArea;
HWND resultArea2;

/*

��������!!!
��������!!!
��������!!!
��������!!!


������ ����� ����� �� � $PROJECT/DEBUG, � � $SOLUTION.


����� ��������� �������, ��� ���� ������.

�







*/

const int N = 8;
int matrix[N*N];

// ���������. ��� ����� ���������.
typedef struct MyStruct {
	HWND hwnd;
	int* matrix;
} MyStruct;

STARTUPINFO si = { sizeof(si) };
PROCESS_INFORMATION pi;

int GetRandomInt(int min, int max) {
	int result = min + (rand() % static_cast<int>(max - min + 1));
	return result;
}

void fillRandomMatrix8x8(int* arr) { // �� �����, ��������� ��� ��� ���������� ������. � 2xx ���������.

	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			arr[i*N+j] = GetRandomInt(-9, 9);
		}
	}
}


void showError(LPCWSTR message) {
	HWND hWnd = GetForegroundWindow();
	MessageBox(hWnd, message, L"Error", MB_OK);
}

void showNotification(LPCWSTR message) {
	HWND hWnd = GetForegroundWindow();
	MessageBox(hWnd, message, L"Notification", MB_OK);
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

std::wstring pieceOfArrayToWstring(int* arr, int startIndex, int len) {
	wstring result = L"";
	for (int i = startIndex; i < startIndex+len; i++) {
		result += s2ws(std::to_string(arr[i]));
		result += L"  ";
	}

	return result;
}

std::wstring matrixToWstring(int* matrix, int N) {
	wstring newLine = L"\r\n";
	wstring result = L"";
	for (int i = 0; i < N; i++) {
		result += pieceOfArrayToWstring(matrix, i*N, N);
		result += newLine;
	}

	return result;
}

// display them into resultArea.
void ShowBeforeAfterMatrices(int* matrix, int* matrix2) {
	wstring newLine = L"\r\n";
	wstring result = L"";
	result += L"Before: "+newLine;
	result += matrixToWstring(matrix, N);
	result += newLine;
	result += L"After: " + newLine;
	result += matrixToWstring(matrix2, N);
	result += newLine;

	SetWindowText(resultArea, result.c_str());
}

LPCWSTR GetOpenExeFilePath() {
	OPENFILENAME ofn;
	// a another memory buffer to contain the file name
	TCHAR szFile[255];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"exe\0*.exe\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrDefExt = L".exe";
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn)) {
		LPCWSTR path = ofn.lpstrFile;
		return path;
	}
	else {
		return nullptr;
	}
}

// ������� ������� ��������� ���������
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE:
		{ 
		}
		break;
		case WM_COMMAND:
		{
			switch (wParam) {
				case 100:
				{
					EnableWindow(buttonGO, false);

					LPCWSTR path = GetOpenExeFilePath();
					HANDLE Mailslot = NULL;

					bool isCreated = CreateProcessW(0,   // Name of program to execute
						(LPWSTR)path,                  // Command line
						NULL,                      // Process handle not inheritable
						NULL,                      // Thread handle not inheritable
						FALSE,                     // Set handle inheritance to FALSE
						0,                         // No creation flags
						NULL,                      // Use parent's environment block
						NULL,                      // Use parent's starting directory 
						&si,                       // Pointer to STARTUPINFO structure
						&pi);  // pointer to procesInfo structure

					if (!isCreated) {
						showError(L"Can't create the server process");
						goto end;
					}
					Sleep(150); // ���� ���� ������� �����������.

					fillRandomMatrix8x8(matrix);

					DWORD BytesWritten;
					if ((Mailslot = CreateFileW(L"\\\\.\\Mailslot\\Slot1", GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
						showError(L"Error while opening the mail slot");
						goto end;
					}


					if (WriteFile(Mailslot, (LPVOID) matrix, sizeof(int) * N * N, &BytesWritten, NULL) == 0) {
						showError(L"Error while sending the matrix %d\n");
						goto end;
					}
					

					
					end:

					CloseHandle(Mailslot);
					return true;
				}
				break;

			}
		}
		break;
		case WM_COPYDATA:
		{
			PCOPYDATASTRUCT cds = (PCOPYDATASTRUCT)lParam;

			int* matrix2 = (int*)cds->lpData; // ����, �� � ��� � ��� ������� �����, ��� �����...

			// ���������� �������1 � �������2 � ������ � ������� � textArea.
			ShowBeforeAfterMatrices(matrix, matrix2);

		

			// ����� ������
					// Open the clipboard, and empty it. 

			if (!OpenClipboard(hWnd))
				return FALSE;
			EmptyClipboard();
			wstring text = L""; // �� ������� ���� �������� ����� �� 4 �����
			wstring newLine = L"\r\n";
			text += L"BOOL WINAPI OpenClipboard(HWND hwnd);";
			text += newLine;
			text += L"� �������� ��������� ���� ������� ";
			text += newLine;
			text += L"���������� ������������� ����,";
			text += newLine;
			text += L"������� ����� \"�������\" Clipboard. ";

			SetWindowText(resultArea2, text.c_str());

			HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE,
				(text.length()) * sizeof(TCHAR));
			
			// Lock the handle and copy the text to the buffer. 
			
			LPWSTR lptstrCopy = (LPWSTR)GlobalLock(hglbCopy);
			memcpy(lptstrCopy, text.c_str(),
				text.length() * sizeof(TCHAR));
			GlobalUnlock(hglbCopy);

			// Place the handle on the clipboard. 

			SetClipboardData(CF_TEXT, hglbCopy);

			CloseClipboard();
			return true;
			// ������� �� ����� ������� �� � �����. ����� ���������� ������ �� ������� � ����� ������.
		}
		break;
		case WM_DESTROY:// ���� ����� �� �������, �� ��� ���� ������ ������� ������� ���� ����� ���������������
			//WaitForSingleObject(pi.hProcess, INFINITE); // not waiting. Just killing.

			// Close process and thread handles. 
			/*CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);*/ // �� ����, �������� �������� ������ ��������� ����
			PostQuitMessage(0);// ���������� ���������� ��������� WM_QUIT. ��������� ��� ������, ������� ��������� � wParam ��������� WM_QUIT
			break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);//������������ ��� ��������� ��������� ������������ "�� ���������"
}



int WINAPI WinMain(HINSTANCE hInst,	//����� �� ��� ����������
	HINSTANCE hPrev,				//�������� ��� ������������� � Win16, ������ = 0
	LPSTR szCmdLine,				//��������� ������ ����� ����������
	int nShowCmd)					//���������, �����������, ��� ���� ��������� ����������
{
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


	hWnd = CreateWindowW(L"My Class", L"Win32 IPC. Client",
		WS_OVERLAPPEDWINDOW,
		00, 00, 1366, 768, NULL, NULL,
		hInst, NULL);
	

	buttonGO = CreateWindowW(
		L"Button",
		L"GO",
		WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE,
		100, 10, 120, 20,
		hWnd,
		(HMENU)100,//id,
		hInst,
		NULL);

	CreateWindowW(L"static", L"Result of sending-getting the matrix:",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		50, 50, 180, 16,
		hWnd, NULL,
		hInst, NULL);

	resultArea = CreateWindowW(L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		10, 70, 300, 300,
		hWnd,
		NULL,  // id
		hInst,
		NULL);

	CreateWindowW(L"static", L"Sended to clipboard:",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		400, 50, 180, 16,
		hWnd, NULL,
		hInst, NULL);

	resultArea2 = CreateWindowW(L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		350, 70, 500, 300,
		hWnd,
		NULL,  // id
		hInst,
		NULL);


	ShowWindow(hWnd, nShowCmd);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

