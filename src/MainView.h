#ifndef __MAINVIEW__
#define __MAINVIEW__

#include "Browser.h"
#include <memory>
#include <nlohmann/json.hpp>

    struct nk_context;
    struct nk_image;

    struct image_meta {
        int w;
        int h;
        int n;
    };

    class MainView {
    public:

        MainView(std::string config_file, nk_context *ctx, int content_width, int content_height);
        virtual ~MainView();

        inline nk_context *get_context() { return ctx; }
        inline int get_width() const { return content_width; }
        inline int get_height() const { return content_height; }

        void set_size(int width, int height);

        void draw(); // throw std::runtime_error

        void up();
        void down();

    protected:
        void reload_image();

        void adopt_config();

        static const int MAX_PATH_LEN = 4096;

        std::string config_file;

        nlohmann::json config;

        nk_context *ctx;

        int content_width;
        int content_height;

        std::unique_ptr<Browser> browser;

        nk_image *current_image;
        struct image_meta current_image_meta;

        char path_buffer[MAX_PATH_LEN];
        std::string status;
    };

#endif /* __MAINVIEW__ */

