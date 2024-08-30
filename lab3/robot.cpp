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
    print();
}

int Robot::top()
{
    if (maze[cur_pos.first - 1][cur_pos.second] != '#')
    {
        maze[cur_pos.first][cur_pos.second] = '.';
        cur_pos.first--;
        maze[cur_pos.first][cur_pos.second] = 'R';
        print();
        if (cur_pos == maze_exit)
        {
            std::cout << "Success!\n";
            exit(0);
        }
        return 1;
    }
    else return 0;
}

int Robot::bottom()
{
    if (maze[cur_pos.first + 1][cur_pos.second] != '#')
    {
        maze[cur_pos.first][cur_pos.second] = '.';
        cur_pos.first++;
        maze[cur_pos.first][cur_pos.second] = 'R';
        print();
        if (cur_pos == maze_exit)
        {
            std::cout << "Success!\n";
            exit(0);
        } return 1;
    }
    else return 0;
}

int Robot::right()
{
    if (maze[cur_pos.first][cur_pos.second + 1] != '#')
    {
        maze[cur_pos.first][cur_pos.second] = '.';
        cur_pos.second++;
        maze[cur_pos.first][cur_pos.second] = 'R';
        print();
        if (cur_pos == maze_exit)
        {
            std::cout << "Success!\n";
            exit(0);
        }
        return 1;
    }
    else return 0;
}

int Robot::left()
{
    if (maze[cur_pos.first][cur_pos.second - 1] != '#')
    {
        maze[cur_pos.first][cur_pos.second] = '.';
        cur_pos.second--;
        maze[cur_pos.first][cur_pos.second] = 'R';
        print();
        if (cur_pos == maze_exit)
        {
            std::cout << "Success!\n";
            exit(0);
        }
        return 1;
    }
    else return 0;
}

void Robot::teleport()
{
    if (portal_stack.empty()) throw std::invalid_argument("You have teleported into the abyss!");
    maze[cur_pos.first][cur_pos.second] = '.';
    cur_pos = portal_stack.top();
    maze[cur_pos.first][cur_pos.second] = 'R';
    print();
    portal_stack.pop();
}

void Robot::print()
{
    for (size_t i = 0; i < maze.size(); i++)
    {
        for (size_t j = 0; j < maze[i].size(); j++)
            std::cout << maze[i][j];
        std::cout << std::endl;
    }
}
