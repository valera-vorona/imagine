#ifndef __VIEW__
#define __VIEW__

    struct media_data;
    class Model;

    class View {
    public:

        View(Model *model, const char *path); // throw std::runtime_error
        virtual ~View() = default;

        virtual void draw(int content_width, int content_height, media_data *media, bool show_progress = false) = 0;
        static void set_full_path(const char *path); // throw std::runtime_error

    protected:
        static const int MAX_PATH_LEN = 4096;

        Model *model;
        static char path_buffer[MAX_PATH_LEN]; // All the views share the same path_buffer
    };

#endif /* __VIEW__ */

