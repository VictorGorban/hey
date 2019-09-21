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


			// теперь мы работаем только со вторым
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

// принимает данную матрицу, преобразует ее, и возвращает ее же, преобразованую.
void transformMatrix(int* a) {
	int posMin = findPos(a, -8);
	int imn = posMin / N;
	int posMax = findPos(a, +8);
	int imx = posMax / N;

	// для начала надо убедиться, что в матрице есть 2 четных числа, и они не равны.
	int min, max;
	int iMin, iMax;
	if (!arrayFindTwoEvenNumbersNotEqual(a, min, max, iMin, iMax))
		return;



	// находим строки с min и max элементами.
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			int e = a[i * N + j];
			if (!isEven(e)) { // нечет
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

// главная функция обработки сообщений
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:// если этого не сделать, то все ваши жалкие попытки закрыть окно будут проигнорированы
		PostQuitMessage(0);// отправляет приложению сообщение WM_QUIT. Принимает код ошибки, который заносится в wParam сообщения WM_QUIT
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);//обрабатываем все остальные сообщения обработчиком "по умолчанию"
}

int WINAPI WinMain(HINSTANCE hInst,	//хендл на это приложение
	HINSTANCE hPrev,				//оставлен для совместимости с Win16, всегда = 0
	LPSTR szCmdLine,				//командная строка этого приложения
	int nShowCmd)					//параметры, указывающие, как надо запускать приложение
{
	// 1й этап
	// регистрируется класс
	WNDCLASSEX wcx = { 0 };//обнуляем сразу все поля структуры, чтобы ничего не забыть
	wcx.cbSize = sizeof(WNDCLASSEX); //по размеру структуры Windows определит, какая версия API была использована
	wcx.style = CS_HREDRAW | CS_VREDRAW;// говорим окну перерисовываться при изменении размеров окна
	wcx.lpfnWndProc = WndProc;// указываем функцию обработки сообщений
	wcx.hInstance = hInst;	// хендл на наше приложение
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // GetStockObject возвращает хендл на белую кисточку, для фона окна
	wcx.lpszClassName = TEXT("Win32.");// имя данного класса. Должно быть уникальным, иначе, если класс с таким именем уже зарегестрирован, то в регистрации будет отказано

	if (!RegisterClassEx(&wcx))
		return 1;// регистрируем ( не получилось - уходим по английски ) с кодом ошибки (1)



	// 2й этап
	// создается окно
	HWND hWnd = CreateWindowEx(0,
		TEXT("Win32."),//имя класса
		TEXT("Server."),//заголовок окна
		WS_OVERLAPPEDWINDOW, //тип окошка (включает отображение системного меню, кнопок в верхнем правом углу и т.п.)
		CW_USEDEFAULT, 0,//место появления окна (координаты х и y). Здесь указано место “по умолчанию”, поэтому второй параметр игнорируется
		CW_USEDEFAULT, 0,//ширина окна (определяется аналогично месту появления)
		0, //ссылка на родительское окно
		0,//хендл меню
		hInst,
		0);//код, передаваемый с сообщением WM_CREATE 

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

	// помещу сюда все что было в create

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

		/*Теперь получить текст из буфера обмена и вывести его на экран.*/
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

	// теперь показываем окошко ( nShowCmd - как его показать? минимизированным, обычным или ... )
	ShowWindow(hWnd, nShowCmd);
	// говорим окну обновиться
	UpdateWindow(hWnd);

	// 3й этап
	// запуск главного цикла обработки сообщений
	MSG msg = { 0 };// создаем структуру сообщения, которую будем обрабатывать
	while (GetMessage(&msg,
		0,//говорим получать сообщения от всех окон
		0, 0))//диапазон значений получаемых сообщений (сейчас получаем все)
	{	// ждем сообщение
		TranslateMessage(&msg);	// преобразуем виртуальную клавишу в ASCII-код и посылаем сообщение WM_CHAR (тут не нужно.Необходимо, если надо работать с текстом, вводимым с клавиатуры)
		DispatchMessage(&msg);	// передаем сообщения для обработки в "главную функцию обработки сообщений"
	}

	return((int)msg.wParam);	// т.к. это функция, то вернем параметр WM_QUIT сообщения (см. PostQuitMessage)
}



