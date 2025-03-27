#include "recorder.hpp"

#include <filesystem>
#include <fstream>
#include <thread>

#include "debug.hpp"

Recorder::Recorder(const Window& window, unsigned int framerate, std::string path)
    : framerate(framerate),
    path(path)
{
    resolution = window.get_resolution();
    std::filesystem::create_directory(path);
}

Recorder::~Recorder() {
    clean_up_images();
}

void Recorder::update() {
    if (!recording) return;
    save_frame(path + "/" + std::to_string(frame++) + ".ppm");
}

void Recorder::start_recording() {
    if (recording) return;
    recording = true;
    frame = 0;
    debug::print_info("Started recording", "RECORDER");
    clean_up_images();
}

void Recorder::stop_recording() {
    if (!recording) return;
    recording = false;
    debug::print_info("Stopped recording after " + std::to_string(frame + 1) + " frames", "RECORDER");
    
    std::thread encoding_thread(&Recorder::encode_frames, this, "output.mp4");
    encoding_thread.detach();
}

void Recorder::clean_up_images() {
    std::string command = "rm -f "
        + path
        + "/*.ppm";

    if (system(command.c_str()) != 0) {
        debug::print_error("Failed to clean up images in \"" + path +"\"", false, "RECORDER");
    }
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
        + " -y -i "
        + path
        + "/%d.ppm -c:v libx264 -pix_fmt yuv420p "
        + path
        + "/"
        + filepath
        + " > /dev/null 2> /dev/null";

    if (system(command.c_str()) != 0) {
        debug::print_error("Failed to save recording to \"" + filepath + "\"", false, "RECORDER");
    } else {
        debug::print_info("Saved recording to \"" + filepath + "\"", "RECORDER");
    }

    clean_up_images();
}
