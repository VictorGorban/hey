#include <Windows.h>//подключение winAPI
#include <tchar.h>

// функция для определения координат окна относит. родительского. Если это главное окно, то относит. десктопа.
RECT GetLocalCoordinates(HWND hWnd);

HWND secondWindow;
HWND hButton1;
HWND hButton2;
HWND hButton3;
int border;
int caption;





COLORREF grey = RGB(133, 133, 133);
COLORREF red = RGB(255, 0, 0);
COLORREF yellow = RGB(255, 237, 0);
COLORREF green = RGB(0, 240, 0);

COLORREF current_color = yellow;




HDC drawhdc;

class MyClass
{
public:
	HBRUSH greyBrush = CreateSolidBrush(grey);
	HBRUSH redBrush = CreateSolidBrush(red);
	HBRUSH yellowBrush = CreateSolidBrush(yellow);
	HBRUSH greenBrush = CreateSolidBrush(green);

	HBRUSH current_brush = yellowBrush;

	int state = 0;
	bool working = false;

	MyClass() {
		// начальное состояние - желтый мигающий
		state = 0;
		current_brush = yellowBrush;
		RedrawStopLight();
	}

	void IncStateAndRedraw() {
		if (state == 0) {
			return;
		}
		if (state == 3)
			return;

		state++;

		RedrawStopLight();
	}

	void DecStateAndRedraw() {
		if (state == 0) {
			return;
		}
		if (state == 1)
			return;

		state--;

		RedrawStopLight();
	}

	void RedrawStopLight() {
		switch (state)
		{
		case 1: // red
			SelectObject(drawhdc, greenBrush);
			break;
		case 2: // yellow
			SelectObject(drawhdc, yellowBrush);
			break;
		case 3: // green
			SelectObject(drawhdc, redBrush);
			break;
		default:
			SelectObject(drawhdc, current_brush);
		}
		Ellipse(drawhdc, 0, 0, 300, 300);
	}

	void SwitchCurrentBrush() {
		if (current_brush == greyBrush) {
			current_brush = yellowBrush;
		}
		else
			current_brush = greyBrush;
	}

};


MyClass light;

TCHAR WinName[] = _T("MainFrame");
TCHAR Child1[] = _T("MainFrame");

// прототипы ф-ий
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
	if (!RegisterClass(&wc))// Регистрация класса окна
		return 0;

	//
	WNDCLASS wcChild1 = wc;

	wcChild1.lpfnWndProc = (WNDPROC)WndProc1;
	wcChild1.lpszClassName = Child1;
	wcChild1.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcChild1.hIcon = LoadIcon(NULL, IDI_ASTERISK);
	wcChild1.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcChild1.lpszMenuName = NULL;
	wc.cbClsExtra = 0;// Нет дополнительных данных класса
	wc.cbWndExtra = 0;

	RegisterClass(&wcChild1);

	//создание окна
	HWND hWnd = CreateWindow(
		WinName, //имя класса окна
		_T("Панель управления"),//заголовок главного окна
		WS_OVERLAPPEDWINDOW,//стиль окна
		0,//x
		0,//y
		200,//Width
		400,//Height
		HWND_DESKTOP,//дескриптор родительского окна
		NULL,//нет меню
		This,//дескриптор приложения
		NULL);//дополнительная информация
	ShowWindow(hWnd, mode);//отображаем окно
	UpdateWindow(hWnd);

	// создаем и показываем второе окно
	secondWindow = CreateWindow(
		Child1,
		_T("Светофор"),
		WS_OVERLAPPEDWINDOW,
		190,
		0,
		316,
		338,
		hWnd,
		NULL,
		This,
		NULL);
	ShowWindow(secondWindow, SW_SHOW | SW_SHOWNORMAL);
	UpdateWindow(secondWindow);

	drawhdc = GetDC(secondWindow);

	border = 8; // ширина границы.
	caption = 30;


	// создание кнопок
	hButton1 = CreateWindow(
		_T("button"),
		_T("Работа"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		40,//x
		10,//y
		100,//width
		40,//height
		hWnd,
		(HMENU)100,//id кнопки
		This, NULL);
	hButton2 = CreateWindow(
		_T("button"),
		_T("Вверх"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		40,//x
		60,//y
		100,//width
		40,//height
		hWnd,
		(HMENU)200,//id кнопки
		This, NULL);
	hButton3 = CreateWindow(
		_T("button"),
		_T("Вниз"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		40,//x
		110,//y
		100,//width
		40,//height
		hWnd,
		(HMENU)300,//id кнопки
		This, NULL);


	// создаем светофор
	//light = new MyClass(); // оно и так есть

	
	// window descriptor, timer id, time, callback
	SetTimer(secondWindow, 1, 1000, nullptr);

	//цикл обработки сообщений
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);//функция трансляции кодов нажатой клавиши
		DispatchMessage(&msg);//посылаем сообщения WndProc()
	}
	return 0;
}

//Оконная процедура
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Обработчик сообщений
	switch (message)
	{
	case WM_TIMER:
	{
		light.SwitchCurrentBrush();
		light.RedrawStopLight();

	}
	case WM_COMMAND:
		switch (wParam)
		{
		case 100: // работа
			if (light.state != 0)
			{
				light.state = 0;
				light.current_brush = light.yellowBrush;
				light.RedrawStopLight();
				// window descriptor, timer id, time, callback
				SetTimer(secondWindow, 1, 1000, nullptr);
			}
			else {
				// window descriptor, timer id
				KillTimer(secondWindow, 1);
				light.state = 2;
				light.RedrawStopLight();
			}
			break;
		case 200: { // вверх
			light.IncStateAndRedraw();
			break;
		}
		case 300: { // вниз
			light.DecStateAndRedraw();
			break;
		}
				  break;
		}
	
	case WM_DESTROY: 
		//return 0; // I don't know why after first "Redraw" I got a "WM_DESTROY signal. 200, too". Ok, I know now. That happens when the style of second window is "Visible". 
		// Microsoft please don't kill me

		//delete light;
		break; // Завершение программы
		// Обработка сообщения по умолчанию
	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// wanna delete this? Ok, give that another try
LRESULT CALLBACK WndProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return 0;
}


