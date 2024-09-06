#ifndef __MAINVIEW__
#define __MAINVIEW__

#include "Browser.h"
#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

    class MainView {
    public:

        MainView(nk_context *ctx, const char *path);
        virtual ~MainView() = default;

        void draw(int contect_width, int content_height); // throw std::runtime_error

    protected:

        nk_context *ctx;

        nk_image *current_image;
        struct image_meta current_image_meta;

        char path_buffer[MAX_PATH_LEN];
        std::string status;
    };

#endif /* __MAINVIEW__ */

