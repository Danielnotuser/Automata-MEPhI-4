#include "robot.h"
#include <fstream>

Robot::Robot(const std::string &filename)
{
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open the file: " << filename << std::endl;
        return;
    }

    maze.clear();

    int rows, cols;

    if (!(file >> rows >> cols)) {
        std::cerr << "Error reading matrix dimensions." << std::endl;
    }

    if (rows != cols) std::cerr << "The matrix must be square." << std::endl;

    maze.resize(rows, std::vector<char>(cols));


    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            file >> maze[i][j];
            if (maze[i][j] == 'R') cur_pos = std::make_pair(i, j);
            else if (maze[i][j] == 'E') maze_exit = std::make_pair(i, j);
        }
    }

    file.close();
}

