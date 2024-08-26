#ifndef __MAINVIEW__
#define __MAINVIEW__

#include "FileBrowser.h"

    struct nk_context;
    struct nk_image;

    struct image_meta {
        int w;
        int h;
        int n;
    };

    class MainView {
    public:

        MainView(nk_context *ctx, int content_width, int content_height);
        virtual ~MainView();

        void set_size(int width, int height);

        void draw(); // throw std::runtime_error

        void up();
        void down();

    protected:
        void reload_image();

        static const int MAX_PATH_LEN = 4096;

        nk_context *ctx;

        int content_width;
        int content_height;

        FileBrowser file_browser;

        nk_image *current_image;
        struct image_meta current_image_meta;

        char path_buffer[MAX_PATH_LEN];
        std::string status;
    };



#endif /* __MAINVIEW__ */

