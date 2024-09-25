#include "View.h"

#include <cstring>
#include <stdexcept>

    char View::path_buffer[MAX_PATH_LEN]; // static path_buffer is shared among all the views

    View::View(Model *model, const char *path) : model(model) {
        if (!model) {
          throw std::runtime_error("Error creating View, Model class pointer not provided");
        }

        set_full_path(path);
    }

    void View::set_full_path(const char *path) {
        if (strlen(path) > MAX_PATH_LEN - 1) {
            memcpy(path_buffer, path, MAX_PATH_LEN - 1);
            path_buffer[MAX_PATH_LEN - 1] = '\0';
            throw std::runtime_error("Path too long");
        } else {
            strcpy(path_buffer, path);
        }
    }

