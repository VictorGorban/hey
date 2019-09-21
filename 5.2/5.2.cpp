#include <Windows.h>
#include <tchar.h>
#include <math.h>
#include <string>

#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

HWND hWnd;
HWND hWnd2;
HINSTANCE hInst;

HANDLE hEventHandle;

HANDLE thread1;

typedef struct MyStruct
{
	HWND   hwnd = NULL;
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	int xLength = 0;
} MyStruct, * PMyStruct;


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

void drawLine(HDC hdc, double x1, double y1, double x2, double y2) {

	MoveToEx(hdc, x1, y1, 0); // ����������� � ��������� �����
	LineTo(hdc, x2, y2);

}

void drawRoundedRectangle(HDC hdc, RECT& r) {
	int round = 10;
#pragma region drawLines. � ���� ������� �� ����� ������� ������ �������, ��������� � Arc.
	drawLine(hdc, r.left + round - round * 4 / 10, r.top, r.right - round + round * 4 / 10, r.top);
	drawLine(hdc, r.right, r.top + round - round * 4 / 10, r.right, r.bottom - round + round * 4 / 10);
	drawLine(hdc, r.right - round + round * 4 / 10, r.bottom, r.left + round - round * 4 / 10, r.bottom);
	drawLine(hdc, r.left, r.bottom - round + round * 4 / 10, r.left, r.top + round - round * 4 / 10);
#pragma endregion

#pragma region Arc
	// arc ��� ����� �������. ��� ���� ���� � debug ����� ����, �� ����� ��� ���������. ms u r the best. ... ������ ������������ � 86 �� 64, ������ - ������ ��� ����. �����...
	// �������� ��� ��� - ������. ������ ������������ �� 64, �� ������������ - ��� ��������...
	// ������ �������� ��� ��� - ������. ��� ��� - ��������. �����������������������
	// �����
	Arc(hdc, r.right - round, r.top, r.right, r.top + round,
		r.right + 4, r.top + round - 3, r.right - round + 3, r.top); // �������������� �������������, ����� ������ ���� � �����
	Arc(hdc, r.right - round, r.bottom, r.right, r.bottom - round,
		r.right - round + 4, r.bottom, r.right, r.bottom - round + 3); // �������������� �������������, ����� ������ ���� � �����
	Arc(hdc, r.left, r.bottom - round, r.left + round, r.bottom,
		r.left - 4, r.bottom - round, r.left + round - 3, r.bottom); // �������������� �������������, ����� ������ ���� � �����
	Arc(hdc, r.left, r.top + round, r.left + round, r.top,
		r.left + round - 3, r.top, r.left - 3, r.top + round); // �������������� �������������, ����� ������ ���� � �����
#pragma endregion

}

DWORD WINAPI ThreadFunc1(LPVOID lpParam)
{
	HDC hdc = GetWindowDC(hWnd2);

	RECT rect;
	GetClientRect(hWnd2, &rect);
	rect.top += 50;
	rect.left += 10;
	rect.right -= 30;
	//rect.bottom -= 50;

	int steps = 100;

	double stepX = ((rect.right - rect.left) / steps * 0.98); // �������� ������ ��� 100)
	double stepY = ((rect.bottom - rect.top) / steps * 0.98);

	rect.top += 30;
	rect.left += 10;
	rect.right += 0;
	rect.bottom += 0;


	int i = steps + 1;
	int iPrev = -1;
	bool direction = false;
	while (true) {
		// ����� ��� �������
		//showNotification(L"Now It'll be waiting for event. Only 3 sec, c'mon!!!");
		int errorCode = WaitForSingleObject(hEventHandle, 3000);
		if (errorCode != WAIT_OBJECT_0) {
			wstring errStr = L"Process 2: Wait failed";
			errStr += L" Reason: ";

			switch (errorCode)
			{
			case WAIT_TIMEOUT: {
				errStr += L"timeout";
				break;
			}
			case WAIT_ABANDONED: {
				errStr += L"abandoned";
				break;
			}
			case WAIT_FAILED: {
				errStr += L"failed with errCode: ";
				errStr += std::to_wstring(GetLastError());
				break;
			}

			default:
				break;
			}

			showError(errStr.c_str());
			return 2;
		}
		ResetEvent(hEventHandle);
		//int b = 3;

		for (int i2 = 0; i2 < 7; i2++) {

			HPEN hpen = CreatePen(PS_SOLID, 1, RGB(255, 0, 128)); // ������� pen, ������� � ����� ������ ��������.
			SelectObject(hdc, hpen);

			RECT r;
			r.top = rect.top + stepY * i;
			r.bottom = r.top + 16 * 3.5;
			r.left = rect.left + stepX * i;
			r.right = r.left + 50 * 3.5;
			//DrawTextW(hdc, L"������!", 7, &r, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			drawRoundedRectangle(hdc, r);
			InvalidateRect(hWnd, &r, false);
			Sleep(200);

			// ����������, ������� 200��, ����� ������

			iPrev = i;

			if (iPrev >= 0) {
				DeleteObject(hpen);
				hpen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255)); // ����� �����
				SelectObject(hdc, hpen);

				// ������� ������ ������ ������������...
				drawRoundedRectangle(hdc, r); // ��������� ����� � ������ ����� rectangle ����� ������.
				//DrawTextW(hdc, L"               ", 15, &rPrev, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				// ����� ��������� ��� ����, �.�. ���������� ������������. ��� ���� ����������� ���-�� ������, ������� ����� ���!
				//InvalidateRect(hWnd, &r, true);

				DeleteObject(hpen); // � ������ ������� hpen, ������ ��� ��� continue �����, �� ������ ���������.
			}

			if (direction) {
				i++;
			}
			else {
				i--;
			}

			if (i > steps || i == 0) {
				direction = !direction;
			}
		}
		//SetEvent, ��� PulseEvent
	   SetEvent(hEventHandle);
	   Sleep(50); // ������ ����� ������ ������������
	   //PulseEvent(hEventHandle);
	}



	return 0;
}




LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE: {
		// ������� �����
		thread1 = CreateThread(NULL, 0, ThreadFunc1, NULL, 0, NULL);
		break;
	}
	case WM_COMMAND: {
		switch (wParam) {
		case 300: {
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
		}
	}
	case WM_DESTROY:// ���� ����� �� �������, �� ��� ���� ������ ������� ������� ���� ����� ���������������
		// ���� ���-�� ����� �������� �������
		CloseHandle(thread1);
		SetEvent(hEventHandle);
		PostQuitMessage(0);// ���������� ���������� ��������� WM_QUIT. ��������� ��� ������, ������� ��������� � wParam ��������� WM_QUIT
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);//������������ ��� ��������� ��������� ������������ "�� ���������"
}

int WINAPI WinMain(HINSTANCE hInstance,	//����� �� ��� ����������
	HINSTANCE hPrev,		//�������� ��� ������������� � Win16, ������ = 0
	LPSTR cmdLine,		//��������� ������ ����� ����������
	int nShowCmd)			//���������, �����������, ��� ���� ��������� ����������
{
	hInst = hInstance;

	MSG msg;

	string s = cmdLine;
	wstring ws = s2ws(s);


#pragma region ������� ���������� ��������� ������ � ���������� handle-��
	int count = 2;
	LPWSTR* args = CommandLineToArgvW(ws.c_str(), &count);
	hEventHandle = (HANDLE)_wtol(args[1]);
	hWnd2 = (HWND)_wtol(args[0]);

	//showNotification(std::to_wstring((long)hEventHandle).c_str());
#pragma endregion



	// parent
	WNDCLASS wc;
	memset(&wc, 0, sizeof(WNDCLASS));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = CreateSolidBrush(0x00FFFFFF);
	wc.lpszClassName = L"My Class";
	RegisterClass(&wc);


	hWnd = CreateWindowW(L"My Class", L"Window title",
		WS_OVERLAPPEDWINDOW,
		00, 00, 1366, 768, NULL, 0,
		hInstance, NULL);


	/*ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);*/

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}