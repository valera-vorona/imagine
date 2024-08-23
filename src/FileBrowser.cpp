#include "FileBrowser.h"
#include <filesystem>
#include <algorithm>
#include <stdexcept>

    FileBrowser::FileBrowser(std::string path) : path(path) {
        update(path);
    }

    void FileBrowser::update(std::string path) {
        namespace fs = std::filesystem;

        this->full_path = this->path = fs::u8path(path);

        const auto status = fs::status(this->path);

        if (!fs::exists(status)) {
            throw std::runtime_error("Incorrect path");
        }

        std::string filename;

        if (!fs::is_directory(status)) {
            filename = this->path.filename();
            this->path.remove_filename();
        }
 
        files.clear();
        for (const auto &entry : fs::directory_iterator(this->path, fs::directory_options::follow_directory_symlink)) {
            auto current = entry.path().filename().string();
            files.push_back({
                fs::is_directory(entry),
                filename == current ? true : false,
                current
            });
        }

        std::ranges::sort(files, [](const FileBrowser::FileEntry &a, const FileBrowser::FileEntry &b) {
             return a.name.compare(b.name) < 0;
        });

    }

    bool FileBrowser::prev() {
        auto entry = std::ranges::find_if(files, [](const FileEntry &e) {
            return e.is_active == true;
        });

        if (entry != files.end() && entry != files.begin() && entry - 1 != files.begin()) {
            entry->is_active = false;
            (entry - 1)->is_active = true;
            full_path = path / (entry - 1)->name;
            return true;
        } else {
            return false;
        }
    }

    bool FileBrowser::next() {
        auto entry = std::ranges::find_if(files, [](const FileEntry &e) {
            return e.is_active == true;
        });

        if (entry != files.end() && entry + 1 != files.end()) {
            entry->is_active = false;
            (entry + 1)->is_active = true;
            full_path = path / (entry + 1)->name;
            return true;
        } else {
            return false;
        }
    }

