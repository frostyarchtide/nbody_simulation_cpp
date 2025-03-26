#pragma once

#include <string>

#include "window.hpp"

class Recorder {
public:
    Recorder(const Window& window, std::string path);

    void save_frame();

private:
    glm::uvec2 resolution;
    std::string path;
    unsigned int frame = 0;
};
