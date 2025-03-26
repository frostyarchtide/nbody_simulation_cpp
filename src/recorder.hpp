#pragma once

#include <string>

#include "window.hpp"

class Recorder {
public:
    Recorder(const Window& window, unsigned int framerate, std::string path);
    ~Recorder();

    void update();
    void start_recording();
    void stop_recording();
    void save_frame(std::string filepath);
    void encode_frames(std::string filepath);

private:
    unsigned int framerate;
    glm::uvec2 resolution;
    std::string path;
    bool recording = false;
    unsigned int frame = 0;
};
