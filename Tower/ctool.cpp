#include "ctool.h"


std::mutex ctool::mtx; 

void ctool::ShowConsoleCursor(bool showFlag) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // Set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

void ctool::GotoXY(int x, int y) {
    COORD crd = { (SHORT)x, (SHORT)y }; 
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), crd);
}

void ctool::Draw(const char* str, int i, const cpoint p[], cpoint& drawn_point_out) {
    mtx.lock();
    drawn_point_out = p[i]; 
    ctool::GotoXY(static_cast<int>(drawn_point_out.getX()), static_cast<int>(drawn_point_out.getY()));
    std::cout << str;
    mtx.unlock();
}
