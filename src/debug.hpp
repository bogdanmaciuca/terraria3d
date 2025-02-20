#pragma once
#include <iostream>
#include <GLFW/glfw3.h>

class Timer {
public:
    Timer(const std::string& name) : _name(name) {
        std::cout << "Timer " << _name << " started\n";
        _start = glfwGetTime();
    }
    ~Timer() {
        std::cout << "Timer " << _name << ": " << (glfwGetTime() - _start) * 1000.0 << "ms\n";
    }
private:
    double _start = 0;
    std::string _name;
};
