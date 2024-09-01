#include "FileBrowser.h"
#include <algorithm>
#include <stdexcept>

    void FileBrowser::update_path(std::string path, bool force_update) {
        namespace fs = std::filesystem;

        fs::path in_full_path = fs::u8path(path);

        if (!force_update && this->full_path == in_full_path) return;

        const auto status = fs::status(in_full_path);

        if (!fs::exists(status)) {
            throw std::runtime_error("Incorrect path");
        }

        fs::path in_path = in_full_path;

        if (!fs::is_directory(status)) {
            in_path.remove_filename();
        }

        auto filename = in_full_path.filename();

        if (force_update || this->path != in_path) {
            this->full_path = in_full_path;
            this->path = in_path;

            files.clear();

            files.push_back({true, false, ".."});

            for (const auto &entry : fs::directory_iterator(this->path, fs::directory_options::follow_directory_symlink)) {
                auto current = entry.path().filename().string();
                files.push_back({
                    fs::is_directory(entry),
                    filename == current ? true : false,
                    current
                });
            }

            std::ranges::sort(files, [](const FileBrowser::FileEntry &a, const FileBrowser::FileEntry &b) {
                if (a.name == "..") return true;
                if (b.name == "..") return false;
                if (a.is_directory < b.is_directory) return false;
                if (a.is_directory > b.is_directory) return true;
                return a.name.compare(b.name) < 0;
            });
        } else {
            this->full_path = in_full_path;

            for (auto &entry : files) {
                entry.is_active = (filename == entry.name) ? true : false;
            }
        }
    }

    void FileBrowser::update_file(std::string file, bool force_update) {
        update_path(this->path / file, force_update);
    }

    bool FileBrowser::prev() {
        auto entry = std::ranges::find_if(files, [](const FileEntry &e) {
            return e.is_active == true;
        });

        if (entry != files.end() && entry != files.begin()) {
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

    std::string FileBrowser::get_path() const {
        return path;
    }

    std::string FileBrowser::get_full_path() const {
        return full_path;
    }

    bool FileBrowser::is_dir() const {
        namespace fs = std::filesystem;

        return fs::is_directory(fs::status(full_path));
    }

