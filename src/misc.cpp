#include "misc.h"
#include <memory>
#include <vector>
#include <filesystem>
#include <stdexcept>

#ifndef _WIN32
#include <pwd.h>
#endif

#include "Model.h"

std::string get_home_dir() {
#ifdef _WIN32
    std::string rv = getenv("USERPROFILE");
    if (rv.empty()) {
        rv = getenv("HOMEDRIVE") + genenv("HOMEPATH");
    }
#else
    std::string rv = getenv("HOME");
    if (rv.empty()) {
        struct passwd *pw = getpwuid(getuid());
        if (!pw) {
            throw std::runtime_error("getpwuid() failed");
        }

        rv = pw->pw_dir;
    }
#endif

    return rv;
}

//TODO: I know this function is implemented bad. I should find out how to reffer to CMAKR_INSTALL_FREFIX
// or something like that to get the project's install dir

std::string get_install_dir() {
    namespace fs = std::filesystem;

    const std::vector<std::string> paths = {
#ifdef _WIN32
        "C:/Program Files",
        "D:/Program Files",
        "C:/Program Files"
#else
        "/usr",
        "/usr/local"
#endif
    };

    for (auto p : paths) {
        auto f =  fs::path(p) / IMAGINE_INSTALL_DIR;
        if (fs::exists(f)) {
            return f;
        }
    }

    return std::string();
}

