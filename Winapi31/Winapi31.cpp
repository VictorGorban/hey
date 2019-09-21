#include <Windows.h>//подключение winAPI
#include <tchar.h>
#include <math.h>
#include <string>

#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

double dx = 0.1;
const double a = 10; // основание логарифма
LONG WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
double** points; // массив данных
	  // Задание исходных данных для графика
	  // (двумерный массив, может содержать несколько рядов данных)

bool isBetween(double x, double from, double to) {
	if (x > from && x < to) {
		return true;
	}

	return false;
}

bool isNearerToUpper(double what, double x1, double x2) {
	double x = what;

	if (x1 > x2) {
		std::swap(x1, x2);
	}

	if (x < x1) {
		return false;
	}
	if (x > x2) {
		return true;
	}
	// x between x1 and x2

	double diffToLower = x - x1;
	double diffToUpper = x2 - x;

	if (diffToUpper < diffToLower) {
		return true;
	}

	return false;
}

void showError(LPCWSTR message) {
	HWND hWnd = GetForegroundWindow();
	MessageBox(hWnd, message, L"Error", MB_OK);
}

void drawLine(HDC hdc, double prevX, double prevY, double thisX, double thisY) {
	MoveToEx(hdc, prevX, prevY, 0); // перемещение в начальную точку
	LineTo(hdc, thisX, thisY);
}

void drawText(HDC hdc, double x, double y, LPCWSTR str, int length) {
	MoveToEx(hdc, x, y, 0); // перемещение в начальную точку
	TextOut(hdc, x, y, str, length);
}

double invertY(double Y, double height) {
	return height - Y;
}

double fitGapToStandardGap(double gap, double* standardGaps, int length) {
	double* gaps = standardGaps;
	double minGap = gaps[0];
	double maxGap = gaps[length - 1];

	if (gap < minGap) {
		return minGap;
	}

	if (gap > maxGap) {
		return maxGap;
	}
	// if between minGap and maxGap

	for (int i = 0; i < length - 1; i++) {
		double curr = gaps[i];
		double next = gaps[i + 1];

		if (isBetween(gap, curr, next)) {
			// я не могу возвращать нижнее значение, т.к. это ведет к налезанию букв.
			return next;
		}

	}
	// если мы дошли сюда, значит какое-то из условий не сработало.
	return -1;
}

double* generateGaps(double* first3Gaps, int repeatAnd10xTimes) {
	double gap0 = first3Gaps[0];
	double gap1 = first3Gaps[1];
	double gap2 = first3Gaps[2];

	double* res = new double[3 * repeatAnd10xTimes];

	for (int i = 0; i < repeatAnd10xTimes; i++) {
		int pos = 3 * i;

		res[pos + 0] = gap0;
		res[pos + 1] = gap1;
		res[pos + 2] = gap2;

		gap0 *= 10;
		gap1 *= 10;
		gap2 *= 10;
	}

	return res;
}

// for testing purpose, it just return log(x)
double logax(double a, double x) {
	// переход к новому основанию. Юзаю 2 потому что привычно.
	double res = log2(x) / log2(a);

	return res;
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

LPCWSTR strToWinapiString(std::string str) {
	std::wstring wstr = s2ws(str);
	LPCWSTR res = wstr.c_str();
	// здесь есть, а после выхода из функции уже \0. Интересно.
	return res;
}

std::string doubleToStr(double x, int precision) {
	std::stringstream ss;
	// Set Fixed -Point Notation, then presision
	ss << std::fixed << std::setprecision(precision) << x;

	std::string str = ss.str();
	return str;
}

std::string pointToString(double* p) {
	std::string str = std::to_string(p[0]) + " " + std::to_string(p[1]) + ",  ";

	return str;
}

std::string arrayOfPointsToString(double** arr, int length) {
	std::string str = "";

	for (int i = 0; i < length; i++) {
		double* point = arr[i];

		str += pointToString(point);
	}


	return str;
}

std::string array1ToString(double* arr, int length) {
	std::string str = "";

	for (int i = 0; i < length; i++) {
		double item = arr[i];

		str += std::to_string(item) + ", ";
	}


	return str;
}


bool checkParams(double xMin, double xMax) {
	// у меня логарифм. Меньше нуля он не существует, в нуле тоже. a>=0;
	if (a < 0) {
		return false;
	}

	if (xMin <= 0) {
		return false;
	}

	if (xMin > xMax) {
		return false;
	}

	return true;
}

int findNumberOfPointsAndSetDX(double xMin, double xMax) {
	int n;
	double diff = xMax - xMin;

	if (diff > 5) {
		dx = 0.2;
	}
	else if (diff > 2) {
		dx = 0.05;
	}
	else {
		dx = 0.01;
	}

	n = (int)(diff / dx) + 1; // иначе last точка не будет считаться.

	return n;
}

double** getDataNoCheck(double xMin, double xMax) // todo: параметр a менять здесь
{											// parameter A  of loga(x)
	// number of points
	int n = findNumberOfPointsAndSetDX(xMin, xMax);

	// это массив точек, т.е. array[n] of points {x,y}. Point is array[2];
	// this is sin(x), but I need log a x.
	double** f;
	f = new double* [n];

	for (int i = 0; i < n; i++) {
		f[i] = new double[2];

		double x = (double)(xMin + i * dx);
		f[i][0] = x;
		f[i][1] = logax(a, x);
		//f[n] = [x,y]
	}

	return f;
}

// Функция рисования графика
void DrawGraph(HDC hdc, RECT rectClient,
	double xMin, double xMax) {
	// маленький костыль: это условие проверяется и в checkParams, но там нет set.
	if (xMin <= 0) {
		xMin = 0.01;
	}


	bool isParamsOk = checkParams(xMin, xMax);
	if (!isParamsOk) {
		showError(L"Рисование графика не удалось, неверные данные.");
		return;
	}

#pragma region set vars


	// no checking!!!
	double** points = getDataNoCheck(xMin, xMax);



	double yMin, yMax;
	double diffX, diffY;
	int height, width;
	double scaleX, scaleY;
	HPEN hpen;


	height = rectClient.bottom - rectClient.top;
	width = rectClient.right - rectClient.left;


#pragma region определяем minY, maxY.
	yMin = yMax = points[0][1];

	// number of points
	int n = findNumberOfPointsAndSetDX(xMin, xMax);

	for (int i = 0; i < n; i++) {
		if (points[i][1] < yMin) yMin = points[i][1];
		if (points[i][1] > yMax) yMax = points[i][1];
	}
#pragma endregion

	// на основании xMin и xMax -> xDiff и width and height, находим масштаб по points и по y. А также offsetX и Y.
	diffX = xMax - xMin;
	diffY = yMax - yMin;
	scaleX = (double)width / diffX; // масштабный коэффициент X
	scaleY = (double)height / diffY; // масштабный коэффициент X

#pragma endregion

	double yOfOx, xOfOy = 0;
	bool isOxTextUpper = true;
	bool isOyTextRighter = true;

#pragma region Отрисовка осей координат

#pragma region set position of axes
	/// Ox always on left side

	if (isBetween(0, yMin, yMax)) {
		double all = -yMin + yMax;
		double mult = -yMin / all; // >0
		//mult = 1-mult;
		if (mult > 1 || mult <= 0) {
			showError(L"Это кошмар");
		}
		yOfOx = height * mult; // высота как максимальная y координата

		isOxTextUpper = false;
		if (yOfOx < 12) { // высота шрифта + 4px запаса
			isOxTextUpper = true;
		}
	}
	else if (yMin >= 0) {
		yOfOx = 0 + 1; // +1 просто потому что иначе не рисуется. Oy рисуется, а Ox нет.
		isOxTextUpper = true;
	}
	else /*(yMax < 0)*/ {
		yOfOx = height;
		isOxTextUpper = false;
	}
#pragma endregion

#pragma region draw serifs and line numbers on the axes
	SIZE size;
	GetTextExtentPoint32(hdc, L"5", 1, &size);
	int charWidth = size.cx;
	int charHeight = size.cy;
	int numberWidth = charWidth * 8;
	int numberHeight = charHeight;

	int maxNumbersX = width / numberWidth;
	int maxNumbersY = height / (numberHeight * 4);

	double numbersGapX = (double)((xMax - xMin) / maxNumbersX);
	double numbersGapY = (double)((yMax - yMin) / maxNumbersY);
	int gapCount = 30;
	double* first3Gaps = new double[3];
	first3Gaps[0] = 0.1;
	first3Gaps[1] = 0.25;
	first3Gaps[2] = 0.5;
	double* standardGaps = generateGaps(first3Gaps, gapCount / 3);
	numbersGapX = fitGapToStandardGap(numbersGapX, standardGaps, gapCount);
	if (numbersGapX < 0) {
		showError(L"Что-то не так в ф-ии numbersGap");
		return;
	}

	{
		hpen = CreatePen(PS_DASHDOT, 0, RGB(128, 128, 128));
		SelectObject(hdc, hpen);
		double y = yOfOx; // устанавливаю в типа 0
		double x = xOfOy;
		{ // отрисовка всего на Oy от 0 в +
			int i = 0;
			if (yMax < 0) {
				goto endBlock1;
			}

			while (y < height) {
				drawLine(hdc, x, invertY(y, height), width, invertY(y, height));
				// а потом еще циферки
				double number = i * numbersGapY;
				if (yMin > 0) {
					number += yMin;
				}
				std::string str = doubleToStr(number, 2);
				if (str == "0.00") {
					str = "0";
				}
				int strLen = str.length();
				wstring wstr = s2ws(str);
				LPCWSTR strNumber = wstr.c_str();
				// finding pixel length of str
				GetTextExtentPoint32(hdc, strNumber, strLen, &size);
				int strWidth = size.cx;
				int strHeight = size.cy;
				int startTextX;
				int startTextY = y + strHeight;
				if (isOyTextRighter) {
					startTextX = x;
				}
				else {
					startTextX = x - strWidth;
				}

				if (true) {
					drawText(hdc, startTextX, invertY(startTextY, height), strNumber, strLen);
				}


				y += numbersGapY * scaleY;
				i++;
			}
		endBlock1:;
		}

		y = yOfOx; // устанавливаю в типа 0
		x = xOfOy;
		{ // отрисовка всего на Oy от 0 в -
			int i = 0;
			if (yMin >= 0) {
				goto endBlock2;
			}
			while (y > 0) {
				drawLine(hdc, x, invertY(y, height), width, invertY(y, height));
				// а потом еще циферки
				double number = i * numbersGapY;
				if (yMax < 0) {
					number += yMax;
				}
				std::string str = doubleToStr(number, 2);

				int strLen = str.length();
				wstring wstr = s2ws(str);
				LPCWSTR strNumber = wstr.c_str();
				// finding pixel length of str
				GetTextExtentPoint32(hdc, strNumber, strLen, &size);
				int strWidth = size.cx;
				int strHeight = size.cy;
				int startTextX = x; // меньше 0 у меня x не может быть.
				int startTextY = y + strHeight;
				if (isOyTextRighter) {
					startTextX = x;
				}
				else {
					startTextX = x - strWidth;
				}

				if (number != 0) {
					drawText(hdc, startTextX, invertY(startTextY, height), strNumber, strLen);
				}


				y -= numbersGapY * scaleY;
				i--;
			}
		endBlock2:;
		}


		y = yOfOx; // устанавливаю в типа 0
		x = xOfOy;

		{ // отрисовка всего на Ox от 0 в +
			int i = 0;
			while (x <= width) {
				drawLine(hdc, x, invertY(0, height), x, invertY(height, height));
				// а потом еще циферки
				double number = i * numbersGapX + xMin;
				std::string str = doubleToStr(number, 2);
				int strLen = str.length();
				wstring wstr = s2ws(str);
				LPCWSTR strNumber = wstr.c_str();
				// finding pixel length of str
				GetTextExtentPoint32(hdc, strNumber, strLen, &size);
				int strWidth = size.cx;
				int strHeight = size.cy;
				int startTextX = x;
				int startTextY;
				if (isOxTextUpper) {
					startTextY = y + strHeight;
				}
				else {
					startTextY = y - strHeight / 2;
				}

				if (number != 0) {
					drawText(hdc, startTextX, invertY(startTextY, height), strNumber, strLen);
				}


				x += numbersGapX * scaleX;
				i++;
			}
		}


		hpen = CreatePen(PS_SOLID, 0, RGB(0, 255, 0));
		SelectObject(hdc, hpen);
		MoveToEx(hdc, 0, invertY(yOfOx, height), 0); // перемещение в точку (0;OffsetY)
		LineTo(hdc, width, invertY(yOfOx, height)); // рисование oX
		MoveToEx(hdc, xOfOy, 0, 0); // перемещение в точку (OffsetX;0)
		LineTo(hdc, xOfOy, height); // рисование oY
	}


	delete[] first3Gaps;
	delete[] standardGaps;


	// Ок, похоже все работает. Теперь надо нарисовать цифры по OY внизу и отрефакторить. И сделать отдельный gap для Oy, а то путает.

#pragma endregion


	DeleteObject(hpen); // удаление черного пера


#pragma endregion

#pragma region Отрисовка графика функции

	int color = RGB(255, 0, 0); // красное перо
	hpen = CreatePen(PS_SOLID, 2, color); // формирование пера 2px
	SelectObject(hdc, hpen);
	for (int i = 1; i < n; i++) {
		double* prevPoint = points[i - 1];
		double* thisPoint = points[i];

		double prevX = (prevPoint[0] - xMin) * scaleX;
		double prevY = (prevPoint[1] - yMin) * scaleY;

		double thisX = (thisPoint[0] - xMin) * scaleX;
		double thisY = (thisPoint[1] - yMin) * scaleY;

		drawLine(hdc, prevX, invertY(prevY, height), thisX, invertY(thisY, height));
	}


	DeleteObject(hpen); // удаление текущего пера
#pragma endregion

}
// Главная функция
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	HWND hwnd;
	MSG msg;
	WNDCLASS w;
	memset(&w, 0, sizeof(WNDCLASS));
	w.style = CS_HREDRAW | CS_VREDRAW;
	w.lpfnWndProc = WndProc;
	w.hInstance = hInstance;
	w.hbrBackground = CreateSolidBrush(0x00FFFFFF);
	w.lpszClassName = L"My Class";
	RegisterClass(&w);
	hwnd = CreateWindowW(L"My Class", L"График функции loga(x)",
		WS_OVERLAPPEDWINDOW,
		00, 00, 1366, 768, NULL, NULL,
		hInstance, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

// Оконная функция
LONG WINAPI WndProc(HWND hwnd, UINT Message,
	WPARAM wparam, LPARAM lparam) {
	// задаются пользователем, по идее.
	double xMin = 0.1;
	double xMax = 10;


	HDC hdc;
	PAINTSTRUCT ps;
	switch (Message) {
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			DrawGraph(hdc, ps.rcPaint, xMin, xMax); // построение графика

			EndPaint(hwnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, Message, wparam, lparam);
	}
	return 0;
}