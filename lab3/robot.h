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
        int top();
        int bottom();
        int right();
        int left();
        void portal()   { portal_stack.push(cur_pos); };
        void teleport();
        void print();
        ~Robot() = default;
};

#endif //REGEX_ROBOT_H
