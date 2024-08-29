#ifndef REGEX_ROBOT_H
#define REGEX_ROBOT_H

#include <iostream>
#include <string>
#include <vector>
#include <stack>

class Robot {
    private:
        std::vector<std::vector<char>> maze;
        std::pair<int, int> cur_pos;
        std::pair<int, int> maze_exit;
        std::stack<std::pair<int, int>> portal_stack;
    public:
        Robot() = default;
        Robot(const std::string&);
        int top()       {if (maze[cur_pos.first - 1][cur_pos.second] != '#') {cur_pos.first--; if (cur_pos == maze_exit) {std::cout << "Success!\n"; exit(0);} return 1;} else return 0;}
        int bottom()    {if (maze[cur_pos.first + 1][cur_pos.second] != '#') {cur_pos.first++; if (cur_pos == maze_exit) {std::cout << "Success!\n"; exit(0);} return 1;} else return 0;}
        int right()     {if (maze[cur_pos.first][cur_pos.second + 1] != '#') {cur_pos.second++; if (cur_pos == maze_exit) {std::cout << "Success!\n"; exit(0);} return 1;} else return 0;}
        int left()      {if (maze[cur_pos.first][cur_pos.second - 1] != '#') {cur_pos.second--; if (cur_pos == maze_exit) {std::cout << "Success!\n"; exit(0);} return 1;} else return 0;}
        void portal()   { portal_stack.push(cur_pos); };
        void teleport() {if (portal_stack.empty()) throw std::invalid_argument("You have teleported into the abyss!");
                        cur_pos = portal_stack.top(); portal_stack.pop();};
        ~Robot() = default;
};

#endif //REGEX_ROBOT_H
