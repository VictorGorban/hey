#include <Windows.h>//подключение winAPI
#include <tchar.h>

// функция для определения координат окна относит. родительского. Если это главное окно, то относит. десктопа.
RECT GetLocalCoordinates(HWND hWnd);

HWND hChildWnd;
//HWND hButton1;
int border;


int child_x, child_y;

TCHAR WinName[] = _T("MainFrame");
TCHAR Child1[] = _T("MainFrame");
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc1(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE This,//дескриптор текущего приложения
	HINSTANCE Prev,//не используется в win32
	LPSTR cmd,//для запуска окна в командной строке
	int mode)//режим отображения окна
{
	WNDCLASS wc; //класс окна

	//определение класса главного окна
	wc.hInstance = This;
	wc.lpszClassName = WinName; // Имя класса окна
	wc.lpfnWndProc = (WNDPROC)WndProc; // Функция окна
	wc.style = CS_HREDRAW | CS_VREDRAW; // Стиль окна
	wc.hIcon = LoadIcon(NULL, IDI_ASTERISK); // Стандартная иконка
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // Стандартный курсор
	wc.lpszMenuName = NULL; // Нет меню
	wc.cbClsExtra = 0; // Нет дополнительных данных класса
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);// Заполнение окна белым цветом
	if (!RegisterClass(&wc))
		return 0; // Регистрация класса окна


	//создание окна
	HWND hWnd = CreateWindow(
		WinName, //имя класса окна
		_T("Отец"),//заголовок главного окна
		WS_POPUP,//стиль окна
		CW_USEDEFAULT,//x
		CW_USEDEFAULT,//y
		500,//Width
		400,//Height
		HWND_DESKTOP,//дескриптор родительского окна
		NULL,//нет меню
		This,//дескриптор приложения
		NULL);//дополнительная информация
	ShowWindow(hWnd, mode);//отображаем окно
	UpdateWindow(hWnd);

	// создаем и показываем сына
	child_x = 0;
	child_y = 0;
	hChildWnd = CreateWindow(
		WinName, // КЛАСС ТОТ ЖЕ, ЧТО И У РОДИТЕЛЯ
		_T("Сын"),
		WS_OVERLAPPEDWINDOW | WS_CHILDWINDOW,
		child_x,
		child_y,
		200,
		100,
		hWnd,
		NULL,
		This,
		NULL);
	ShowWindow(hChildWnd, SW_SHOW | SW_SHOWNORMAL);
	UpdateWindow(hChildWnd);

	border = 8; // ширина границы.


	//кнопка для 1го задания
	//hButton1 = CreateWindow(_T("button"), _T("Двигать сына"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON ,
	//	150,//x
	//	150,//y
	//	150,//width
	//	40,//height
	//	hWnd,
	//	(HMENU)100,//id кнопки
	//	This, NULL);

	//цикл обработки сообщений
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);//функция трансляции кодов нажатой клавиши
		DispatchMessage(&msg);//посылаем сообщения WndProc()
	}
	return 0;
}

void moveChild(HWND hWnd) {
	// rect отца
	RECT parent_rect;
	GetWindowRect(hWnd, &parent_rect);
	LONG pw = parent_rect.right - parent_rect.left;
	LONG ph = parent_rect.bottom - parent_rect.top;

	// rect сына
	RECT child_local_rect = GetLocalCoordinates(hChildWnd);
	RECT child_rect;
	GetWindowRect(hChildWnd, &child_rect);
	LONG cw = child_rect.right - child_rect.left;
	LONG ch = child_rect.bottom - child_rect.top;

	// поехали по часовой стрелке
	if (child_local_rect.left == 0 && child_local_rect.top == 0) {
		SetWindowPos(hChildWnd, hWnd, parent_rect.right - child_rect.right, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
	else if (child_local_rect.left > 0 && child_local_rect.top == 0) {
		// 38 потому что это высота границы 8 + верхней границы 30(панель с кнопочками).
		SetWindowPos(hChildWnd, hWnd, child_local_rect.left, child_local_rect.top + (ph - ch), cw, ch, SWP_NOSIZE | SWP_NOZORDER);
	}
	else if (child_local_rect.left > 0 && child_local_rect.top > 0) {
		SetWindowPos(hChildWnd, hWnd, 0, child_local_rect.top, cw, ch, SWP_NOSIZE | SWP_NOZORDER);
	}
	else {
		SetWindowPos(hChildWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	
}

//Оконная процедура
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Обработчик сообщений
	switch (message)
	{
		case WM_LBUTTONDOWN:
			moveChild(hWnd);
			break;
	//case WM_COMMAND:
	//	switch (wParam)
	//	{
	//	case 100:
	//		moveChild(hWnd);
	//		break;
	//	}
	//	break;
	case WM_DESTROY: PostQuitMessage(0);
		break; // Завершение программы
		// Обработка сообщения по умолчанию
	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WndProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Обработчик сообщений
	switch (message)
	{
	case WM_DESTROY: 
		PostQuitMessage(0);
		break; // Завершение программы
		// Обработка сообщения по умолчанию
	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// функция для определения координат окна относит. родительского. Если это главное окно, то относит. десктопа.
RECT GetLocalCoordinates(HWND hWnd)
{
	RECT Rect;
	GetWindowRect(hWnd, &Rect);
	MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT)& Rect, 2);
	return Rect;
}
