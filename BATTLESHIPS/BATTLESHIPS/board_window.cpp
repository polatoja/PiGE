#include "board_window.h"

BoardWindow::BoardWindow(HWND parent, int gridSize)
    : m_hWnd(nullptr), m_gridSize(gridSize), m_windowSize(0)
{
    m_windowSize = (cellSize + cellMargin) * m_gridSize + margin * 2;

    m_hWnd = CreateWindowEx(
        0,
        L"STATIC",
        nullptr,
        WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_BLACKRECT,
        0, 0, m_windowSize, m_windowSize,
        parent,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr);

    CreateCells(m_hWnd);

    CreateGrid(m_hWnd);
}

void BoardWindow::CreateCells(HWND parent) {
    for (int i = 0; i < m_gridSize; ++i) {
        for (int j = 0; j < m_gridSize; ++j) {
            HWND hWnd = CreateWindowEx(
                0,
                L"STATIC",
                nullptr,
                WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_BLACKRECT,
                margin + j * (cellSize + cellMargin),
                margin + i * (cellSize + cellMargin),
                cellSize, cellSize,
                parent,
                nullptr,
                GetModuleHandle(nullptr),
                nullptr);

            m_cells.push_back(hWnd);
        }
    }
}

void BoardWindow::ResizeCells() {
    for (int i = 0; i < m_gridSize; ++i) {
        for (int j = 0; j < m_gridSize; ++j) {
            SetWindowPos(
                m_cells[i * m_gridSize + j],
                nullptr,
                margin + j * (cellSize + cellMargin),
                margin + i * (cellSize + cellMargin),
                cellSize, cellSize,
                SWP_NOZORDER);
        }
    }
}

void BoardWindow::CreateGrid(HWND parent) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HGDIOBJ oldPen = SelectObject(GetDC(parent), hPen);

    for (int i = 0; i <= m_gridSize; ++i) {
        // Draw horizontal lines
        MoveToEx(GetDC(parent), margin, margin + i * (cellSize + cellMargin), nullptr);
        LineTo(GetDC(parent), margin + m_gridSize * (cellSize + cellMargin), margin + i * (cellSize + cellMargin));

        // Draw vertical lines
        MoveToEx(GetDC(parent), margin + i * (cellSize + cellMargin), margin, nullptr);
        LineTo(GetDC(parent), margin + i * (cellSize + cellMargin), margin + m_gridSize * (cellSize + cellMargin));
    }

    SelectObject(GetDC(parent), oldPen);
    DeleteObject(hPen);
}
