#include "math.h"
#include <windows.h>
#include <tlhelp32.h> 

#include <string>

#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;


HANDLE Mailslot;
HWND clientHwnd;
HWND resultArea;
const int N = 8;
int matrix[N * N];

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

void showError(LPCWSTR message) {
	HWND hWnd = GetForegroundWindow();
	MessageBox(hWnd, message, L"Error", MB_OK);
}

void showNotification(LPCWSTR message) {
	HWND hWnd = GetForegroundWindow();
	MessageBox(hWnd, message, L"Notification", MB_OK);
}

void swapArrayPieces(int* a, int firstIndex, int secondIndex) {
	int temp1[N];
	for (int i = firstIndex; i < firstIndex + N; i++) {
		temp1[i - firstIndex] = a[i];
	}
	int temp2[N];
	for (int i = secondIndex; i < secondIndex + N; i++) {
		temp2[i - secondIndex] = a[i];
	}

	for (int i = firstIndex; i < firstIndex + N; i++) {
		a[i] = temp2[i - firstIndex];
	}
	for (int i = secondIndex; i < secondIndex + N; i++) {
		a[i] = temp1[i - secondIndex];
	}


}

bool isEven(int x) {
	return x % 2 == 0;
}

bool arrayFindTwoEvenNumbersNotEqual(int* a, int& first, int& last, int& iMin, int& iMax, int len = N * N) {
	bool exists1 = false, exists2 = false;
	for (int i = 0; i < len; i++) {
		if (isEven(a[i])) {
			if (!exists1) {
				exists1 = true;
				first = a[i];
				iMin = i / N;
				continue;
			}


			// ������ �� �������� ������ �� ������
			if (!exists2) {
				exists2 = true;
				last = a[i];
				iMax = i / N;
			}
			else if (first == last) {
				last = a[i];
				iMax = i / N;
			}

			if (exists2 && first != last) {
				return true;
			}
		}
	}
	return false;
}

int findPos(int* a, int x) {
	for (int i = 0; i < N * N; i++) {
		if (a[i] == x) {
			return i;
		}
	}
	return -1;
}

// ��������� ������ �������, ����������� ��, � ���������� �� ��, ��������������.
void transformMatrix(int* a) {
	int posMin = findPos(a, -8);
	int imn = posMin / N;
	int posMax = findPos(a, +8);
	int imx = posMax / N;

	// ��� ������ ���� ���������, ��� � ������� ���� 2 ������ �����, � ��� �� �����.
	int min, max;
	int iMin, iMax;
	if (!arrayFindTwoEvenNumbersNotEqual(a, min, max, iMin, iMax))
		return;



	// ������� ������ � min � max ����������.
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			int e = a[i * N + j];
			if (!isEven(e)) { // �����
				continue;
			}

			if (e > max) {
				max = e;
				iMax = i;
			}
			if (e < min) {
				min = e;
				iMin = i;
			}
		}
	}
	swapArrayPieces(a, iMin * N, iMax * N);
}

// ������� ������� ��������� ���������
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:// ���� ����� �� �������, �� ��� ���� ������ ������� ������� ���� ����� ���������������
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
	// 1� ����
	// �������������� �����
	WNDCLASSEX wcx = { 0 };//�������� ����� ��� ���� ���������, ����� ������ �� ������
	wcx.cbSize = sizeof(WNDCLASSEX); //�� ������� ��������� Windows ���������, ����� ������ API ���� ������������
	wcx.style = CS_HREDRAW | CS_VREDRAW;// ������� ���� ���������������� ��� ��������� �������� ����
	wcx.lpfnWndProc = WndProc;// ��������� ������� ��������� ���������
	wcx.hInstance = hInst;	// ����� �� ���� ����������
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // GetStockObject ���������� ����� �� ����� ��������, ��� ���� ����
	wcx.lpszClassName = TEXT("Win32.");// ��� ������� ������. ������ ���� ����������, �����, ���� ����� � ����� ������ ��� ���������������, �� � ����������� ����� ��������

	if (!RegisterClassEx(&wcx))
		return 1;// ������������ ( �� ���������� - ������ �� ��������� ) � ����� ������ (1)



	// 2� ����
	// ��������� ����
	HWND hWnd = CreateWindowEx(0,
		TEXT("Win32."),//��� ������
		TEXT("Server."),//��������� ����
		WS_OVERLAPPEDWINDOW, //��� ������ (�������� ����������� ���������� ����, ������ � ������� ������ ���� � �.�.)
		CW_USEDEFAULT, 0,//����� ��������� ���� (���������� � � y). ����� ������� ����� ��� ����������, ������� ������ �������� ������������
		CW_USEDEFAULT, 0,//������ ���� (������������ ���������� ����� ���������)
		0, //������ �� ������������ ����
		0,//����� ����
		hInst,
		0);//���, ������������ � ���������� WM_CREATE 

	resultArea = CreateWindowW(L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		10, 70, 500, 500,
		hWnd, // parent
		NULL,  // id
		hInst,
		NULL);

	CreateWindowW(L"static", L"Received text from buffer:",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		10, 20, 180, 16,
		hWnd, NULL,
		hInst, NULL);

	// ������ ���� ��� ��� ���� � create

	{
		int intBuffer;
		DWORD NumberOfBytesRead;
		if ((Mailslot = CreateMailslot(L"\\\\.\\Mailslot\\slot1", 0, MAILSLOT_WAIT_FOREVER, NULL)) == INVALID_HANDLE_VALUE) {
			showError(L"Error while creating mail slot ");
			goto end;
		}
		if (ReadFile(Mailslot, (LPVOID)matrix, sizeof(int) * N * N, &NumberOfBytesRead, NULL) != 0) {
			// I'm cool
		}
		else {
			showError(L"Error while receiving the matrix");
			goto end;
		}

		transformMatrix(matrix);


		clientHwnd = FindWindowW(L"My Class", L"Win32 IPC. Client");
		if (clientHwnd != nullptr) {
			COPYDATASTRUCT cds;
			cds.dwData = 0;
			cds.cbData = N * N * sizeof(int);
			cds.lpData = (void*)matrix;

			SendMessage(clientHwnd, WM_COPYDATA, 0, (LPARAM)(LPVOID)& cds);
		}
		else {
			showError(L"Cannot find client window by its class and name");
		}



	end:
		CloseHandle(Mailslot);

		/*������ �������� ����� �� ������ ������ � ������� ��� �� �����.*/
		HGLOBAL   hglb;
		LPCWSTR    str;
		if (!IsClipboardFormatAvailable(CF_TEXT))
		{
			showError(L"Clipboard format CF_TEXT not available");
			return false;
		}
		if (!OpenClipboard(hWnd))
		{
			showError(L"Can't open clipboard for this window");
			return true;
		}

		hglb = GetClipboardData(CF_TEXT);
		if (hglb != NULL)
		{
			str = (LPWSTR)GlobalLock(hglb);
			if (str != NULL)
			{
				// Call the application-defined ReplaceSelection 
				// function to insert the text and repaint the 
				// window. 

				SetWindowText(resultArea, str);
				GlobalUnlock(hglb);
			}
		}
		CloseClipboard();
	}

	// ������ ���������� ������ ( nShowCmd - ��� ��� ��������? ����������������, ������� ��� ... )
	ShowWindow(hWnd, nShowCmd);
	// ������� ���� ����������
	UpdateWindow(hWnd);

	// 3� ����
	// ������ �������� ����� ��������� ���������
	MSG msg = { 0 };// ������� ��������� ���������, ������� ����� ������������
	while (GetMessage(&msg,
		0,//������� �������� ��������� �� ���� ����
		0, 0))//�������� �������� ���������� ��������� (������ �������� ���)
	{	// ���� ���������
		TranslateMessage(&msg);	// ����������� ����������� ������� � ASCII-��� � �������� ��������� WM_CHAR (��� �� �����.����������, ���� ���� �������� � �������, �������� � ����������)
		DispatchMessage(&msg);	// �������� ��������� ��� ��������� � "������� ������� ��������� ���������"
	}

	return((int)msg.wParam);	// �.�. ��� �������, �� ������ �������� WM_QUIT ��������� (��. PostQuitMessage)
}



