#ifndef __MAINVIEW__
#define __MAINVIEW__

    struct image_meta;
    class Model;

    class MainView {
    public:

        MainView(Model *model, const char *path); // throw std::runtime_error
        virtual ~MainView() = default;

        void draw(int content_width, int content_height, struct image_meta *image); // throw std::runtime_error
        void set_full_path(const char *path);

    protected:
        static const int MAX_PATH_LEN = 4096;

        Model *model;
        char path_buffer[MAX_PATH_LEN];
    };

#endif /* __MAINVIEW__ */

