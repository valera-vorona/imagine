#include "FileBrowser.h"
#include <filesystem>
#include <stdexcept>


    FileBrowser::FileBrowser(std::string path) : path(path) {
        update(path);
    }

    void FileBrowser::update(std::string path) {
        namespace fs = std::filesystem;

        this->path = fs::u8path(path);

        const auto status = fs::status(this->path);

        if (!fs::exists(status)) {
            throw std::runtime_error("Incorrect path");
        }

        if (!fs::is_directory(status)) {
            this->path.remove_filename();
        }
 
        files.clear();
        for (const auto &entry : fs::directory_iterator(this->path, fs::directory_options::follow_directory_symlink)) {
            files.push_back({
                fs::is_directory(entry),
                entry.path().filename().string()
            });
        }
    }


