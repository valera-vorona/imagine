#include "Model.h"
#include "NormalView.h"
#include "FullScreenView.h"
#include "DummyBrowser.h"
#include "Cache.h"
#include "Loader.h"
#include "misc.h"

#include <GLFW/glfw3.h>
#include <fstream>
#include <stdexcept>

/* config constants */
const std::string CFG_LATEST_SEEN           = "latest_seen";
const std::string CFG_VIEW_MODE             = "view_mode";
const std::string CFG_VIEW_MODE_NORMAL      = "normal";
const std::string CFG_VIEW_MODE_FULLSCREEN  = "fullscreen";
const std::string CFG_VIDEO                 = "video";
const std::string CFG_PAUSED                = "paused";

int mat_to_tex(const cv::Mat &im, media_data *media) {
    using namespace cv;

    int x,y,n;
    GLuint tex;
    GLint ifmt;
    GLenum fmt;

    x = im.size().width;
    y = im.size().height;
    n = im.elemSize();

    switch (n) {
        case 2: ifmt = GL_RG8; fmt = GL_RG; break;
        case 3: ifmt = GL_RGB8; fmt = GL_BGR; break;
        case 4: ifmt = GL_RGBA8; fmt = GL_BGRA; break;
        default: ifmt = GL_R8; fmt = GL_R; break;
    }

    // OpenGL default alignment is 4, correct it if necessery
    glPixelStorei(GL_UNPACK_ALIGNMENT, (x * n) % 4 ? 1 : 4);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST is faster
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //don't use mipmaps, I wonder if it saves some time
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, ifmt, x, y, 0, fmt, GL_UNSIGNED_BYTE, im.ptr());
    //glGenerateMipmap(GL_TEXTURE_2D);

    if (media) {
        media->id = tex;
        media->w = x;
        media->h = y;
        media->n = n;
    }

    return tex;
}

int load_image(std::string filename, struct media_data *media_data) // throw std::runtime_error
{
    using namespace cv;

    auto im = imread(filename, IMREAD_UNCHANGED);
    if (im.empty()) {
        throw std::runtime_error(std::string("Can't open image file: '") + filename + "'");
    }

    return mat_to_tex(im, media_data);
}

void free_image(int tex) {
    glDeleteTextures(1, (const GLuint*)&tex);
}

    Model::Model(std::string config_file, GLFWwindow *window, nk_context *ctx, int content_width, int content_height) :
        config_file(config_file),
        window(window),
        ctx(ctx),
        content_width(content_width),
        content_height(content_height) {

        std::ifstream config_stream(config_file); 
        config = nlohmann::json::parse(config_stream, nullptr, false, true);
        config_stream.close();

        if (config.is_discarded()) {
            config = nlohmann::json::parse("{}", nullptr, false, true);
        }

        adopt_config();

        // Adding the dummy browser. Its function can_do() should always return not 0 showing with that that it can do a file when
        // all the other browsers can't
        add_browser(std::make_shared<DummyBrowser>());

        path            = config[CFG_LATEST_SEEN].get<std::string>();
        video_paused    = config[CFG_VIDEO][CFG_PAUSED].get<bool>();

        views[CFG_VIEW_MODE_NORMAL]     = std::make_shared<NormalView>(this, path.c_str());
        views[CFG_VIEW_MODE_FULLSCREEN] = std::make_shared<FullScreenView>(this, path.c_str());

        set_view_mode(config[CFG_VIEW_MODE].get<std::string>());

        load(path);
    }

    Model::~Model() {
        free_image(current_media.id);

        std::ofstream config_stream(config_file, std::ofstream::out | std::ofstream::trunc);
        config_stream << config;
    }

    void Model::load(std::string filename) {
        try {
            load_image(filename, &current_media);
            showing = IMAGE;

            status = std::string("w: ") + std::to_string(current_media.w) +
                ", h: " + std::to_string(current_media.h) +
                ", n: " + std::to_string(current_media.n);
        } catch (std::runtime_error) {
            try {

                loader = std::make_shared<Loader>(filename, &current_media, 4);
                cache = std::make_shared<Cache>(Period{0, 0}, Period{10, 20}, loader);
                showing = VIDEO;

                if (current_media.fps < 1. || current_media.fps > 200.) {
                    current_media.fps = 20;
                }

                status = std::string("fps: ") + std::to_string((int)current_media.fps) +
                    ", sec: " + std::to_string((int)(current_media.pos / current_media.fps)) +
                    ", length: " + std::to_string((int)(current_media.frames_n / current_media.fps)) +
                    ", w: " + std::to_string(current_media.w) +
                    ", h: " + std::to_string(current_media.h);
            } catch (std::runtime_error &e) {
                showing = NOTHING;
                status.clear();
            }
        }
    }

    void Model::add_browser(std::shared_ptr<Browser> browser) {
        browsers.push_back(browser);
        setup_most_sutable_browser();
    }

    void Model::set_size(int width, int height) {
        content_width = width;
        content_height = height;
    }

    void Model::draw() {
        if (showing == VIDEO && !video_paused) {
            if (current_media.pos != current_media.pos2) {
                //TODO: imapelent video pos change
            }

            cv::Mat mat;
            current_media.pos = current_media.pos2 = cache->next(mat);
            free_image(current_media.id);
            mat_to_tex(mat, &current_media);

            status = std::string("fps: ") + std::to_string((int)current_media.fps) +
                ", sec: " + std::to_string((int)(current_media.pos / current_media.fps)) +
                ", length: " + std::to_string((int)(current_media.frames_n / current_media.fps)) +
                ", w: " + std::to_string(current_media.w) +
                ", h: " + std::to_string(current_media.h) +
                ", n: " + std::to_string(current_media.n);
        }

        view->draw(content_width, content_height, &current_media, showing == VIDEO);
    }

    void Model::reload_image() {
        try {
            free_image(current_media.id);
            path = browser->get_full_path();
            view->set_full_path(path.c_str());
            config[CFG_LATEST_SEEN] = path;
            load(path);
        } catch (std::runtime_error &e) {
            status = e.what();
        }
    }

    void Model::setup_most_sutable_browser() {
        int max = 0;

        for (auto b: browsers) {
            if (b->can_do(path) > max) {
                browser = b;
            }
        }

        browser->update_path(path);
    }

    void Model::up() {
        if (browser->prev()) {
            reload_image();
        }
    }

    void Model::down() {
        if (browser->next()) {
            reload_image();
        }
    }

    void Model::enter() {
        try {
            if (browser->enter()) {
                reload_image();
            }
        } catch (std::runtime_error &e) {
            set_status(e.what());
        }
    }

    void Model::toggle_video_play() {
        video_paused = !video_paused;
        config[CFG_VIDEO][CFG_PAUSED] = video_paused;
    }

    void Model::toggle_view_mode() {
        if (config[CFG_VIEW_MODE] == CFG_VIEW_MODE_FULLSCREEN) {
            set_view_mode(CFG_VIEW_MODE_NORMAL);
        } else {
            set_view_mode(CFG_VIEW_MODE_FULLSCREEN);
        }
    }

    void Model::set_view_mode(std::string mode) {
        auto monitor = glfwGetPrimaryMonitor();
        auto vmode = glfwGetVideoMode(monitor);

        if (mode == CFG_VIEW_MODE_FULLSCREEN) {
            if (glfwGetWindowMonitor(window) == nullptr) {
                glfwSetWindowMonitor(window, monitor, 0, 0, vmode->width, vmode->height, vmode->refreshRate);
            }
            view = views[CFG_VIEW_MODE_FULLSCREEN];
            config[CFG_VIEW_MODE] = CFG_VIEW_MODE_FULLSCREEN;
        } else {
            if (glfwGetWindowMonitor(window)) {
                glfwSetWindowMonitor(window, nullptr, 0, 0, content_width - 20, content_height - 20, vmode->refreshRate);
                //TODO: case 1: deducting the group's border size which is found experementally, it is better to find out how to find the size of the current group in the nuklear code
            }
            view = views[CFG_VIEW_MODE_NORMAL];
            config[CFG_VIEW_MODE] = CFG_VIEW_MODE_NORMAL;
        }
    }

    void Model::adopt_config() {
        if (!config[CFG_LATEST_SEEN].is_string()) {
             config[CFG_LATEST_SEEN] = get_home_dir();
        }

        if (!config[CFG_VIEW_MODE].is_string()) {
             config[CFG_VIEW_MODE] = CFG_VIEW_MODE_NORMAL;
        }

        if (!config[CFG_VIDEO][CFG_PAUSED].is_boolean()) {
             config[CFG_VIDEO][CFG_PAUSED] = false;
        }
    }

