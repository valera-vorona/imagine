#include "Model.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>

#include <fstream>
#include <stdexcept>

#include "MainView.h"
#include "FullScreenView.h"

#include "DummyBrowser.h"

/* config constants */
const std::string CFG_LATEST_SEEN           = "latest_seen";
const std::string CFG_VIEW_MODE             = "view_mode";
const std::string CFG_VIEW_MODE_NORMAL      = "normal";
const std::string CFG_VIEW_MODE_FULLSCREEN  = "fullscreen";

int load_image(std::string filename, struct image_meta *image_meta) // throw std::runtine_error
{
    using namespace cv;

    int x,y,n;
    GLuint tex;
    GLint ifmt;
    GLenum fmt;

    Mat im = imread(filename, IMREAD_UNCHANGED);

    if (im.empty()) {
        throw std::runtime_error(std::string("Can't open image file: '") + filename + "'");
    }

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
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, ifmt, x, y, 0, fmt, GL_UNSIGNED_BYTE, im.ptr());
    glGenerateMipmap(GL_TEXTURE_2D);

    if (image_meta) {
        image_meta->id = tex;
        image_meta->w = x;
        image_meta->h = y;
        image_meta->n = n;
    }

    return tex;
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
        config = nlohmann::json::parse(config_stream);
        config_stream.close();

        adopt_config();

        // Adding the dummy browser. Its function can_do() should always return not 0 showing with that that it can do a file when
        // all the other browsers can't
        add_browser(std::make_shared<DummyBrowser>());

        path = config[CFG_LATEST_SEEN].get<std::string>();

        views[CFG_VIEW_MODE_NORMAL]     = std::make_shared<MainView>(this, path.c_str());
        views[CFG_VIEW_MODE_FULLSCREEN] = std::make_shared<FullScreenView>(this, path.c_str());

        set_view_mode(config[CFG_VIEW_MODE].get<std::string>());

        try {
            load_image(path, &current_image_meta);
        } catch (std::runtime_error &e) {
            status = e.what();
        }
    }

    Model::~Model() {
        free_image(current_image_meta.id);

        std::ofstream config_stream(config_file, std::ofstream::out | std::ofstream::trunc);
        config_stream << config;
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
        view->draw(content_width, content_height, &current_image_meta);
    }

    void Model::reload_image() {
        try {
            free_image(current_image_meta.id);
            path = browser->get_full_path();
            view->set_full_path(path.c_str());
            config[CFG_LATEST_SEEN] = path;
            load_image(path, &current_image_meta);
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
             config[CFG_LATEST_SEEN] = "/"; //TODO: I should use a more smart way here to find the home dir like it is done in the main function
        }

        if (!config[CFG_VIEW_MODE].is_string()) {
             config[CFG_VIEW_MODE] = CFG_VIEW_MODE_NORMAL;
        }
    }

