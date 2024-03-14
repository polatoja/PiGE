#include "board.h"
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()

std::vector<std::vector<int>> board::grid;

int board::my_random(int min, int max) {
    static bool initialized = false;
    if (!initialized) {
        srand((unsigned int) time(NULL));
        initialized = true;
    }
    return min + rand() % (max - min + 1);
}

std::vector<std::vector<int>> board::place_ships(int boardSize, int randomVal)
{
    // Seed the random number generator
    srand(randomVal);

    // Define ship sizes
    std::vector<int> shipSizes = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };

    // Initialize the grid with -1 to mark unoccupied cells
    grid = std::vector<std::vector<int>>(boardSize, std::vector<int>(boardSize, -1));

    // Iterate through each ship size
    for (int shipSize : shipSizes)
    {
        bool placed = false;
        while (!placed)
        {
            // Randomly choose orientation (0 for horizontal, 1 for vertical)
            int orientation = rand() % 2;

            // Randomly choose starting position
            int startX = rand() % boardSize;
            int startY = rand() % boardSize;

            // Check if ship fits in chosen position
            if (orientation == 0 && startX + shipSize <= boardSize)
            {
                // Check if chosen positions are empty
                bool valid = true;
                for (int x = startX; x < startX + shipSize; ++x)
                {
                    if (grid[startY][x] != -1)
                    {
                        valid = false;
                        break;
                    }
                }

                // If positions are empty, place the ship
                if (valid)
                {
                    for (int x = startX; x < startX + shipSize; ++x)
                    {
                        grid[startY][x] = shipSize;
                    }

                    // Mark cells around the ship as unoccupied
                    for (int x = std::max(0, startX - 1); x < std::min(boardSize, startX + shipSize + 1); ++x)
                    {
                        for (int y = std::max(0, startY - 1); y < std::min(boardSize, startY + 2); ++y)
                        {
                            if (grid[y][x] == -1)
                            {
                                grid[y][x] = 0;
                            }
                        }
                    }
                    placed = true;
                }

            }
            else if (orientation == 1 && startY + shipSize <= boardSize)
            {
                // Check if chosen positions are empty
                bool valid = true;
                for (int y = startY; y < startY + shipSize; ++y)
                {
                    if (grid[y][startX] != -1)
                    {
                        valid = false;
                        break;
                    }
                }

                // If positions are empty, place the ship
                if (valid)
                {
                    for (int y = startY; y < startY + shipSize; ++y)
                    {
                        grid[y][startX] = shipSize;
                    }

                    // Mark cells around the ship as unoccupied
                    for (int y = std::max(0, startY - 1); y < std::min(boardSize, startY + shipSize + 1); ++y)
                    {
                        for (int x = std::max(0, startX - 1); x < std::min(boardSize, startX + 2); ++x)
                        {
                            if (grid[y][x] == -1)
                            {
                                grid[y][x] = 0;
                            }
                        }
                    }
                    placed = true;
                }

            }
        }
    }

    for (int i = 0; i < boardSize; ++i)
    {
        for (int j = 0; j < boardSize; ++j)
        {
            if (grid[i][j] == -1)
            {
                grid[i][j] = 0;
            }
        }
    }

    return grid; // Return the grid after placing the ships
}
