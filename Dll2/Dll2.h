// dll2.h - Contains declarations of first function
#pragma once

#include "pch.h"
#include "resource.h"

#include <Windows.h>//подключение winAPI
#include <tchar.h>
#include <math.h>
#include <string>

#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

#ifdef DLL2_EXPORTS
#define DLL2_API __declspec(dllexport)
#else
#define DLL2_API __declspec(dllimport)
#endif

// returns double[xMin, xMax];
extern "C" DLL2_API double* ShowXMinXMaxDialogDisableParent(HWND parent, double xMin, double xMax);
