#ifndef __MODEL__
#define __MODEL__

#include "Browser.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

    struct GLFWwindow;
    struct nk_context;

    namespace cv {
        class VideoCapture;
    }

    class View;

    struct image_meta {
        int id;
        int w;
        int h;
        int n;
    };

    enum Showing {
        NOTHING = 0,
        IMAGE,
        VIDEO
    };

    class Model {
    public:

        Model(std::string config_file, GLFWwindow *window, nk_context *ctx, int content_width, int content_height);
        ~Model();

        void load(std::string filename);

        void add_browser(std::shared_ptr<Browser> browser);

        inline nk_context *get_context() { return ctx; }
        inline int get_width() const { return content_width; }
        inline int get_height() const { return content_height; }
        inline std::string get_path() const { return path; }
        inline std::string get_status() const { return status; }
        inline void set_status(std::string new_status) { status = new_status; }
        inline std::shared_ptr<Browser> get_browser()  { return browser; }
        inline Showing what_showing() const { return showing; }
        inline double get_video_fps() const { return video_fps; }
        inline double get_video_frames_n() const { return video_frames_n; }
        inline size_t *get_video_pos_ptr() { return &video_pos; }

        void set_size(int width, int height);

        void draw(); // throw std::runtime_error

        void up();
        void down();

        void toggle_view_mode();

        void reload_image();

    protected:
        void setup_most_sutable_browser();

        void adopt_config();

        void set_view_mode(std::string mode);

        std::string config_file;

        nlohmann::json config;

        GLFWwindow *window;

        nk_context *ctx;

        int content_width;
        int content_height;

        std::shared_ptr<cv::VideoCapture> vc;

        std::shared_ptr<View> view;
        std::unordered_map<std::string, std::shared_ptr<View>> views;

        std::shared_ptr<Browser> browser;
        std::vector<std::shared_ptr<Browser>> browsers;

        struct image_meta current_image_meta;

        Showing showing = NOTHING;
        double video_fps        = 0;
        double video_frames_n   = 0;
        size_t video_pos        = 0;
        size_t video_pos2       = 0; // This var is to compare it with video_pos. If they are not equal, it means that video_pos was changed outside this class,
                                     // i.e. through Model::get_video_pos_ptr() with a progress bar

        std::string path;
        std::string status;
    };

#endif /* __MODEL__ */

