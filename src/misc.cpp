#include "misc.h"
#include <memory>
#include <stdexcept>

#ifndef _WIN32
#include <pwd.h>
#endif

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

