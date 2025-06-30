#include "ctool.h"

// ctool.cpp
// Implementation for ctool class.

std::mutex ctool::mtx; // Definition of the static mutex

void ctool::ShowConsoleCursor(bool showFlag) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // Set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

void ctool::GotoXY(int x, int y) {
    COORD crd = { (SHORT)x, (SHORT)y }; // COORD uses SHORT
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), crd);
}

// The original PDF has `char* str`. Using `const char*` is safer if str is not modified.
// The original PDF has `cpoint& _p` as the last parameter. Renamed to `drawn_point_out` for clarity.
void ctool::Draw(const char* str, int i, const cpoint p[], cpoint& drawn_point_out) {
    mtx.lock();
    drawn_point_out = p[i]; // Store the point whose coordinates are being used
    ctool::GotoXY(static_cast<int>(drawn_point_out.getX()), static_cast<int>(drawn_point_out.getY()));
    std::cout << str;
    mtx.unlock();
}
