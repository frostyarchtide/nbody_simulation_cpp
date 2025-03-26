#include "recorder.hpp"

#include <filesystem>
#include <fstream>

#include "debug.hpp"

Recorder::Recorder(const Window& window, unsigned int framerate, std::string path)
    : framerate(framerate),
    path(path)
{
    resolution = window.get_resolution();
    std::filesystem::create_directory(path);
}

Recorder::~Recorder() {
    if (recording) stop_recording();
}

void Recorder::update() {
    if (!recording) return;
    save_frame(path + "/" + std::to_string(frame++) + ".ppm");
}

void Recorder::start_recording() {
    if (recording) return;
    recording = true;
    debug::print_info("Started recording", "RECORDER");
}

void Recorder::stop_recording() {
    if (!recording) return;
    recording = false;
    debug::print_info("Stopped recording after " + std::to_string(frame) + " frames", "RECORDER");
    encode_frames("output.mp4");
}

void Recorder::save_frame(std::string filepath) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        debug::print_error("Failed to save file to \"" + filepath + "\"", false, "RECORDER");
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

void Recorder::encode_frames(std::string filepath) {
    std::string command = "ffmpeg -framerate "
        + std::to_string(framerate)
        + " -i "
        + path
        + "/%d.ppm -c:v libx264 -pix_fmt yuv420p "
        + path
        + "/"
        + filepath;

    if (system(command.c_str()) != 0) {
        debug::print_error("Failed to save recording to \"" + filepath + "\"", false, "RECORDER");
    } else {
        debug::print_info("Saved recording to \"" + filepath + "\"", "RECORDER");
    }

    command = "rm -r "
        + path
        + "/*.ppm";

    if (system(command.c_str()) != 0) {
        debug::print_error("Failed to clean up images in \"" + filepath +"\"", false, "RECORDER");
    }
}
