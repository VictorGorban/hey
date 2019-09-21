#include <Windows.h>
#include <tchar.h>
#include <math.h>
#include <string>

#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

HWND hWnd;
HINSTANCE hInst;

HWND hEditX1;
HWND hEditY1;
HWND hEditX2;
HWND hEditY2;
HWND hEditXLength;

HWND hButton1;
HWND hButton2;

HMENU hMenu;

HANDLE hEventHandle;

HANDLE thread1;
HANDLE thread2;
bool thread1Running = false;

STARTUPINFO si = { sizeof(si) };
PROCESS_INFORMATION pi;

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

DWORD WINAPI ThreadFunc1(LPVOID lpParam)
{



	HDC dc = GetWindowDC(hWnd);
	RECT rect;
	GetClientRect(hWnd, &rect);
	rect.top += 50;
	rect.left += 10;
	rect.right -= 30;
	//rect.bottom -= 50;

	int steps = 100;

	double stepX = ((rect.right - rect.left) / steps * 0.98); // работает только для 100)
	double stepY = ((rect.bottom - rect.top) / steps * 0.98);

	rect.top += 30;
	rect.left += 10;
	rect.right += 0;
	rect.bottom += 0;


	int i = steps + 1;
	int iPrev = -1;
	bool direction = false;
	while (true) {
		// здесь жду события
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
		int b = 3;

		for (int i2 = 0; i2 < 7; i2++) {


			RECT r;
			r.top = rect.top + stepY * i;
			r.bottom = r.top + 16;
			r.left = rect.left + stepX * i;
			r.right = r.left + 50;
			DrawTextW(dc, L"Привет!", 7, &r, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			InvalidateRect(hWnd, &r, false);
			Sleep(200);
			// нарисовали, поспали 200 мс, потом стерли.

			iPrev = i;

			if (iPrev >= 0) {
				
				DrawTextW(dc, L"               ", 15, &r, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				//InvalidateRect(hWnd, &r, true);
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
		 //SetEvent, или PulseEvent
		SetEvent(hEventHandle);
		Sleep(50); // второй поток должен среагировать
		//PulseEvent(hEventHandle);
	}



	return 0;
}

void drawLine(HDC hdc, double x1, double y1, double x2, double y2) {
	HPEN hpen = CreatePen(PS_DASHDOT, 0, RGB(255, 0, 128));
	SelectObject(hdc, hpen);
	MoveToEx(hdc, x1, y1, 0); // перемещение в начальную точку
	LineTo(hdc, x2, y2);

	DeleteObject(hpen);
}

DWORD WINAPI ThreadFunc2(LPVOID lpParam)
{
	PMyStruct pms = (PMyStruct)lpParam;
	MyStruct ms = *pms;

	HDC hdc = GetWindowDC(ms.hwnd);
	RECT rect;
	GetClientRect(ms.hwnd, &rect);

	int x1, x2, y1, y2;

	x1 = ms.x1;
	x2 = ms.x2;
	y1 = ms.y1;
	y2 = ms.y2;
	drawLine(hdc, x1, y1, x2, y2);

	x1 = ms.x1;
	x2 = x1 + ms.xLength;
	y1 = ms.y1;
	y2 = y1;
	drawLine(hdc, x1, y1, x2, y2);

	x1 = ms.x1 + ms.xLength;
	x2 = ms.x2 + ms.xLength;
	y1 = ms.y1;
	y2 = ms.y2;
	drawLine(hdc, x1, y1, x2, y2);

	x1 = ms.x2;
	x2 = x1 + ms.xLength;
	y1 = ms.y2;
	y2 = y2;
	drawLine(hdc, x1, y1, x2, y2);

	// Все, конец, поток отработал
	return 0;
}

bool isDigit(wchar_t* str) {
	std::wstring ws = str;
	if (ws.find_first_not_of(L"1234567890") != wstring::npos) { // если ф-ия НАХОДИТ в строке НЕ цифру, то это false.
		return false;
	}

	return true;
}

void TryParseParamsForSecondThread(MyStruct& ms, bool& isOk) {
	isOk = true;
	TCHAR str[10];
	int val;

	GetWindowText(hEditX1, str, 10);
	if (!isDigit(str)) {
		isOk = false;
		return;
	}
	val = _wtoi(str);
	ms.x1 = val;

	GetWindowText(hEditX2, str, 10);
	if (!isDigit(str)) {
		isOk = false;
		return;
	}
	val = _wtoi(str);
	ms.x2 = val;

	GetWindowText(hEditY1, str, 10);
	if (!isDigit(str)) {
		isOk = false;
		return;
	}
	val = _wtoi(str);
	ms.y1 = val;

	GetWindowText(hEditY2, str, 10);
	if (!isDigit(str)) {
		isOk = false;
		return;
	}
	val = _wtoi(str);
	ms.y2 = val;

	GetWindowText(hEditXLength, str, 10);
	if (!isDigit(str)) {
		isOk = false;
		return;
	}
	val = _wtoi(str);
	ms.xLength = val;

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE: {
		// создаем событие для синхронизации
		hEventHandle = CreateEventW(
			NULL,               // default security attributes
			TRUE,               // manual-reset event
			true,              // initial state is signaled
			TEXT("SyncEvent")  // object name
		);

		SetHandleInformation(hEventHandle, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);

		// создаем поток
		thread1 = CreateThread(NULL, 0, ThreadFunc1, NULL, CREATE_SUSPENDED, NULL);
		thread1Running = false;
		break;
	}
	case WM_COMMAND: {
		switch (wParam) {
		case 100:
		{
			if (thread1Running) {
				SuspendThread(thread1);
			}
			else
			{
				ResumeThread(thread1);
			}
			thread1Running = !thread1Running;

			break;
		}
		case 200: {
			PMyStruct pms = new MyStruct();
			pms->hwnd = hWnd;
			bool isOk;
			TryParseParamsForSecondThread(*pms, isOk);
			if (!isOk) {
				showError(L"Введите числа без точек, запятых и других лишних символов.");
				return false;
			}

			thread2 = CreateThread(NULL, 0, ThreadFunc2, pms, 0, NULL);
			break;
		}
		case 300: {
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
		case 1100: {
			// запускаем приложение

			wstring str(L"5.2.exe");
			str += L" ";
			str += std::to_wstring((ULONG)hWnd).c_str();
			str += L" ";
			str += std::to_wstring((ULONG)hEventHandle).c_str();

			bool isCreated = CreateProcessW(0,   // Name of program to execute
				(LPWSTR)str.c_str(),   // Command line
				NULL,                      // Process handle not inheritable
				NULL,                      // Thread handle not inheritable
				true,                     // Set handle inheritance to true
				0,                         // No creation flags
				NULL,                      // Use parent's environment block
				NULL,                      // Use parent's starting directory 
				&si,                       // Pointer to STARTUPINFO structure
				&pi);  // pointer to procesInfo structure

			if (isCreated) {
				EnableMenuItem(hMenu, 1100, MF_GRAYED);
				wstring str2(L"");
				str2 += L"HWND: ";
				str2 += std::to_wstring((ULONG)hWnd);
				str2 += L"\r\n";
				str2 += L"Event: ";
				str2 += std::to_wstring((ULONG)hEventHandle);
				//MessageBox(hWnd, str2.c_str(), L"hello", 0); // и так все нормально
			}
			else {
				showError(L"Не удается создать второй процесс");
			}


			break;
		}
		}
		break;
	}
	case WM_DESTROY:// если этого не сделать, то все ваши жалкие попытки закрыть окно будут проигнорированы
		// Надо как-то убить дочерний процесс
		TerminateProcess(pi.hProcess, 0); // вроде работает
		PostQuitMessage(0);// отправляет приложению сообщение WM_QUIT. Принимает код ошибки, который заносится в wParam сообщения WM_QUIT
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);//обрабатываем все остальные сообщения обработчиком "по умолчанию"
}

int WINAPI WinMain(HINSTANCE hInstance,	//хендл на это приложение
	HINSTANCE hPrev,		//оставлен для совместимости с Win16, всегда = 0
	LPSTR szCmdLine,		//командная строка этого приложения
	int nShowCmd)			//параметры, указывающие, как надо запускать приложение
{
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

	AppendMenuW(hMenu, MF_STRING, 1100, L"Новое приложение");

	hWnd = CreateWindowW(L"My Class", L"Window title",
		WS_OVERLAPPEDWINDOW | WS_MAXIMIZE,
		00, 00, 1366, 768, NULL, hMenu,
		hInstance, NULL);

	HWND hLabel1 = CreateWindowW(L"static", NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		810, 10, 120, 16,
		hWnd, NULL,
		hInstance, NULL);
	SetWindowText(hLabel1, L"Доп. поток 1:");

	hButton1 = CreateWindow(
		L"Button",
		NULL,
		WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE,
		810, 30, 105, 20,
		hWnd,
		(HMENU)100,//id,
		hInstance,
		NULL);
	SetWindowText(hButton1, L"Переключить ");

	HWND hButton512 = CreateWindow(
		L"Button",
		NULL,
		WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE,
		610, 30, 150, 20,
		hWnd,
		(HMENU)300,//id,
		hInstance,
		NULL);
	SetWindowText(hButton512, L"Очистить экран     ");

	HWND hLabel2 = CreateWindowW(L"static", NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		1050, 10, 220, 16,
		hWnd, NULL,
		hInstance, NULL);
	SetWindowText(hLabel2, L"Доп. поток 2 - четырехугольник:");

	hButton2 = CreateWindow(
		L"Button",
		NULL,
		WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE,
		1300, 10, 90, 20,
		hWnd,
		(HMENU)200,//id,
		hInstance,
		NULL);
	SetWindowText(hButton2, L"Запустить  ");


	HWND hLabel3 = CreateWindowW(L"static", NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		1070, 40, 150, 16,
		hWnd, NULL,
		hInstance, NULL);
	SetWindowText(hLabel3, L"Левый нижний угол:");

	HWND hLabel4 = CreateWindowW(L"static", NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		1100, 60, 10, 16,
		hWnd, NULL,
		hInstance, NULL);
	SetWindowText(hLabel4, L"X:");

	HWND hLabel5 = CreateWindowW(L"static", NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		1170, 60, 10, 16,
		hWnd, NULL,
		hInstance, NULL);
	SetWindowText(hLabel5, L"Y:");

	// 2 edit

	hEditX1 = CreateWindow(
		L"Edit",
		L"",
		WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE,
		1080, 80, 50, 20,
		hWnd,
		NULL,
		hInstance,
		NULL);
	SetWindowText(hEditX1, L"0");

	hEditY1 = CreateWindow(
		L"Edit",
		L"",
		WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE,
		1150, 80, 50, 20,
		hWnd,
		NULL,
		hInstance,
		NULL);
	SetWindowText(hEditY1, L"500");


	HWND hLabel6 = CreateWindowW(L"static", NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		1070, 120, 150, 16,
		hWnd, NULL,
		hInstance, NULL);
	SetWindowText(hLabel6, L"Левый верхний угол:");

	HWND hLabel7 = CreateWindowW(L"static", NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		1100, 140, 10, 16,
		hWnd, NULL,
		hInstance, NULL);
	SetWindowText(hLabel7, L"X:");

	HWND hLabel8 = CreateWindowW(L"static", NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		1170, 140, 10, 16,
		hWnd, NULL,
		hInstance, NULL);
	SetWindowText(hLabel8, L"Y:");

	// 2 edit

	hEditX2 = CreateWindow(
		L"Edit",
		L"",
		WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE,
		1080, 160, 50, 20,
		hWnd,
		NULL,
		hInstance,
		NULL);
	SetWindowText(hEditX2, L"0");

	hEditY2 = CreateWindow(
		L"Edit",
		L"",
		WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE,
		1150, 160, 50, 20,
		hWnd,
		NULL,
		hInstance,
		NULL);
	SetWindowText(hEditY2, L"400");

	HWND hLabel9 = CreateWindowW(L"static", NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		1070, 190, 150, 16,
		hWnd, NULL,
		hInstance, NULL);
	SetWindowText(hLabel9, L"Длина по X:");

	hEditXLength = CreateWindow(
		L"Edit",
		L"",
		WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE,
		1070, 210, 100, 20,
		hWnd,
		NULL,
		hInstance,
		NULL);
	SetWindowText(hEditXLength, L"600");


	ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}