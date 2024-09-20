#ifndef __MODEL__
#define __MODEL__

#include "Browser.h"
#include "Cache.h"
#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

    struct GLFWwindow;
    struct nk_context;

    namespace cv {
        class VideoCapture;
    }

    class View;
    class Cache;
    class Loader;

    struct media_data {
        int id;             // OpenGL image name
        int w;              // Width
        int h;              // Height
        int n;              // Image element size
        double fps;         // Frames per second in video file
        double frames_n;    // Number of frames in video file
        size_t pos;         // Current position in video file
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
        inline double get_video_fps() const { return current_media.fps; }
        inline double get_video_frames_n() const { return current_media.frames_n; }
        inline size_t *get_video_pos_ptr() { return &current_media.pos; }

        void set_size(int width, int height);

        void draw(); // throw std::runtime_error

        void up();
        void down();
        void enter();
        void toggle_video_play();

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

        std::shared_ptr<View> view;
        std::unordered_map<std::string, std::shared_ptr<View>> views;

        std::shared_ptr<Browser> browser;
        std::vector<std::shared_ptr<Browser>> browsers;

        media_data current_media;

        Showing showing = NOTHING;

        bool video_paused;

        std::shared_ptr<Cache> cache;
        Cache::iterator cache_i;
        std::shared_ptr<Loader> loader;

        std::string path;
        std::string status;
    };

#endif /* __MODEL__ */

