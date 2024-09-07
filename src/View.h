#ifndef __VIEW__
#define __VIEW__

    struct image_meta;
    class Model;

    class View {
    public:

        View(Model *model, const char *path); // throw std::runtime_error
        virtual ~View() = default;

        virtual void draw(int content_width, int content_height, struct image_meta *image) = 0;
        void set_full_path(const char *path); // throw std::runtime_error

    protected:
        static const int MAX_PATH_LEN = 4096;

        Model *model;
        char path_buffer[MAX_PATH_LEN];
    };

#endif /* __VIEW__ */

