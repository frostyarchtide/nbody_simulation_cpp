#include "recorder.hpp"

#include <filesystem>
#include <fstream>

#include "debug.hpp"

Recorder::Recorder(const Window& window, std::string path) : path(path) {
    resolution = window.get_resolution();
    std::filesystem::create_directory(path);
}

void Recorder::save_frame() {
    std::string filepath = path + std::string("/") + std::to_string(frame++) + ".ppm";
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        debug::print_error("Error opening file \"" + filepath + "\"");
    }

    unsigned char* pixels = new unsigned char[3 * resolution.x * resolution.y];
    glReadPixels(0, 0, resolution.x, resolution.y, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    file << "P6\n" << resolution.x << " " << resolution.y << "\n255\n";

    for (int y = resolution.y - 1; y >= 0; y--) {
        file.write(reinterpret_cast<char*>(pixels + (y * resolution.x * 3)), resolution.x * 3);
    }

    delete[] pixels;
    file.close();
}
