// dll1.h - Contains declarations of first function
#pragma once

#include "pch.h"

#include <Windows.h>//подключение winAPI
#include <tchar.h>
#include <math.h>
#include <string>

#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

#ifdef DLL1_EXPORTS
#define DLL1_API __declspec(dllexport)
#else
#define DLL1_API __declspec(dllimport)
#endif

// if error, return <0
extern "C" DLL1_API int DrawGraph(HDC hdc, RECT rectClient,
	double xMin, double xMax);
