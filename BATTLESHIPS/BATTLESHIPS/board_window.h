#pragma once
#include <windows.h>
#include <vector>

class BoardWindow {
public:
    BoardWindow(HWND parent, int gridSize);

private:
    static const int cellSize = 30;
    static const int margin = 5;
    static const int cellMargin = 3;

    HWND m_hWnd;
    int m_gridSize;
    int m_windowSize;
    std::vector<HWND> m_cells;

    void CreateCells(HWND parent);
    void ResizeCells();
    void CreateGrid(HWND parent);
};