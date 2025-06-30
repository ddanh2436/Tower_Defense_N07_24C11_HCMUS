#ifndef CTOOL_H
#define CTOOL_H

#pragma once 

#include "cpoint.h"
#include <windows.h> 
#include <iostream>
#include <mutex>

class ctool {
public:
    static std::mutex mtx; 
    static void ShowConsoleCursor(bool showFlag);
    static void GotoXY(int x, int y);
    static void Draw(const char* str, int i, const cpoint p[], cpoint& drawn_point_out);
};

#endif // CTOOL_H
