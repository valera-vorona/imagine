#ifndef __MAINVIEW__
#define __MAINVIEW__

    struct nk_context;
    struct nk_image;

    class MainView {
    public:

        MainView(nk_context *ctx, int content_width, int content_height);
        virtual ~MainView();

        void set_size(int width, int height);

        void draw();

    protected:
        static const int MAX_PATH_LEN = 4096;

        nk_context *ctx;

        int content_width;
        int content_height;

        bool refresh = true;
        nk_image *current_image;

        char path_buffer[MAX_PATH_LEN];
    };



#endif /* __MAINVIEW__ */

