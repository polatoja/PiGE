#pragma once

#include <vector>

class board
{
private:
    static std::vector<std::vector<int>> grid;

public:
    static int my_random(int min, int max);
    static std::vector<std::vector<int>> place_ships(int boardSize, int randomVal); // Function to place ships on the board
};
