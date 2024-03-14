#include "app_actions.h"
#include <vector>
#include <algorithm>
#include <ctime>
#include <random>

vector<vector<int>> app_actions::GenerateShips()
{
    // Define the number of ships of each size
    std::vector<int> shipSizes = { 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1 };

    // Shuffle the ship sizes randomly
    //std::random_shuffle(shipSizes.begin(), shipSizes.end());

    // Initialize the board with zeros (indicating empty cells)
    std::vector<std::vector<int>> board(10, std::vector<int>(10, 0));

    // Seed the random number generator
    srand(static_cast<unsigned int>(time(nullptr)));

    // Loop through each ship size
    for (int size : shipSizes)
    {
        // Find a valid position for the ship
        bool validPosition = false;
        while (!validPosition)
        {
            // Generate random position, orientation, and direction
            int row = rand() % 10;
            int col = rand() % 10;
            bool horizontal = rand() % 2 == 0;
            bool direction = rand() % 2 == 0; // true: right or down, false: left or up

            // Check if the ship fits in the chosen position
            if (horizontal)
            {
                if (direction)
                {
                    if (col + size <= 10)
                    {
                        bool overlap = false;
                        for (int i = col; i < col + size; ++i)
                        {
                            if (board[row][i] != 0 || (row > 0 && board[row - 1][i] != 0) || (row < 9 && board[row + 1][i] != 0))
                            {
                                overlap = true;
                                break;
                            }
                        }
                        if (!overlap)
                        {
                            validPosition = true;
                            for (int i = col; i < col + size; ++i)
                            {
                                board[row][i] = size;
                            }
                        }
                    }
                }
                else
                {
                    if (col - size + 1 >= 0)
                    {
                        bool overlap = false;
                        for (int i = col; i > col - size; --i)
                        {
                            if (board[row][i] != 0 || (row > 0 && board[row - 1][i] != 0) || (row < 9 && board[row + 1][i] != 0))
                            {
                                overlap = true;
                                break;
                            }
                        }
                        if (!overlap)
                        {
                            validPosition = true;
                            for (int i = col; i > col - size; --i)
                            {
                                board[row][i] = size;
                            }
                        }
                    }
                }
            }
            else
            {
                if (direction)
                {
                    if (row + size <= 10)
                    {
                        bool overlap = false;
                        for (int i = row; i < row + size; ++i)
                        {
                            if (board[i][col] != 0 || (col > 0 && board[i][col - 1] != 0) || (col < 9 && board[i][col + 1] != 0))
                            {
                                overlap = true;
                                break;
                            }
                        }
                        if (!overlap)
                        {
                            validPosition = true;
                            for (int i = row; i < row + size; ++i)
                            {
                                board[i][col] = size;
                            }
                        }
                    }
                }
                else
                {
                    if (row - size + 1 >= 0)
                    {
                        bool overlap = false;
                        for (int i = row; i > row - size; --i)
                        {
                            if (board[i][col] != 0 || (col > 0 && board[i][col - 1] != 0) || (col < 9 && board[i][col + 1] != 0))
                            {
                                overlap = true;
                                break;
                            }
                        }
                        if (!overlap)
                        {
                            validPosition = true;
                            for (int i = row; i > row - size; --i)
                            {
                                board[i][col] = size;
                            }
                        }
                    }
                }
            }
        }
    }

    return board;
}
