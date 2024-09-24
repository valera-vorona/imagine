#include "misc.h"
#include <memory>
#include <vector>
#include <stdexcept>

#ifndef CMAKE_INSTALL_PREFIX
    #include <filesystem>
#endif

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

#ifdef CMAKE_INSTALL_PREFIX

std::string get_install_dir() {
    return CMAKE_INSTALL_PREFIX;
}

#else

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
        auto f = fs::path(p) / IMAGINE_INSTALL_DIR;
        if (fs::exists(f)) {
            return f;
        }
    }

    return std::string();
}

#endif

