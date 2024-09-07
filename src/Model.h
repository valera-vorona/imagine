#ifndef __MODEL__
#define __MODEL__

#include "Browser.h"
#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

    struct nk_context;
    class MainView;

    struct image_meta {
        int id;
        int w;
        int h;
        int n;
    };

    class Model {
    public:

        Model(std::string config_file, nk_context *ctx, int content_width, int content_height);
        ~Model();

        void add_browser(std::shared_ptr<Browser> browser);

        inline nk_context *get_context() { return ctx; }
        inline int get_width() const { return content_width; }
        inline int get_height() const { return content_height; }
        inline std::string get_status() const { return status; }
        inline void set_status(std::string new_status) { status = new_status; }
        inline std::shared_ptr<Browser> get_browser()  { return browser; }

        void set_size(int width, int height);

        void draw(); // throw std::runtime_error

        void up();
        void down();

        void toggle_view();

        void reload_image();

    protected:
        void setup_most_sutable_browser();

        void adopt_config();

        std::string config_file;

        nlohmann::json config;

        nk_context *ctx;

        int content_width;
        int content_height;

        std::shared_ptr<MainView> drawer;

        std::shared_ptr<Browser> browser;
        std::vector<std::shared_ptr<Browser>> browsers;

        struct image_meta current_image_meta;

        std::string path;
        std::string status;
    };

#endif /* __MODEL__ */

