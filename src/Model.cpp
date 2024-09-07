#include "Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <stdexcept>

#include "MainView.h"

#include "DummyBrowser.h"

/* config constants */
const std::string CFG_LATEST_SEEN = "latest_seen";

int load_image(const char *filename, struct image_meta *image_meta) // throw std::runtine_error
{
    int x,y,n;
    GLuint tex;
    GLint ifmt;
    GLenum fmt;

    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);

    if (!data) {
        throw std::runtime_error(std::string("Can't open image file: '") + filename + "'");
    }

    switch (n) {
        case 2: ifmt = GL_RG8; fmt = GL_RG; break;
        case 3: ifmt = GL_RGB8; fmt = GL_RGB; break;
        case 4: ifmt = GL_RGBA8; fmt = GL_RGBA; break;
        default: ifmt = GL_R8; fmt = GL_R; break;
    }

    // OpenGL default alignment is 4, correct it if necessary
    glPixelStorei(GL_UNPACK_ALIGNMENT, (x * n) % 4 ? 1 : 4);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, ifmt, x, y, 0, fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    if (image_meta) {
        image_meta->id = (int)tex;
        image_meta->w = x;
        image_meta->h = y;
        image_meta->n = n;
    }

    return tex;
}

void free_image(int tex) {
    glDeleteTextures(1, (const GLuint*)&tex);
}

    Model::Model(std::string config_file, nk_context *ctx, int content_width, int content_height) :
        config_file(config_file),
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

        strcpy(path_buffer, config[CFG_LATEST_SEEN].get<std::string>().c_str());

        //browser->update_path(path_buffer);

        drawer = std::make_shared<MainView>(this, path_buffer);

        try {
            load_image(path_buffer, &current_image_meta);
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
        drawer->draw(content_width, content_height, &current_image_meta);
    }

    void Model::reload_image() {
        try {
            free_image(current_image_meta.id);
            load_image(browser->get_full_path().c_str(), &current_image_meta);
            strcpy(path_buffer, browser->get_full_path().c_str());
            config[CFG_LATEST_SEEN] = path_buffer;
        } catch (std::runtime_error &e) {
            status = e.what();
        }
    }

    void Model::setup_most_sutable_browser() {
        int max = 0;

        for (auto b: browsers) {
            if (b->can_do(path_buffer) > max) {
                browser = b;
            }
        }

        browser->update_path(path_buffer);
    }

    void Model::up() {
        if (browser->prev() && !browser->is_dir()) {
            reload_image();
        }
    }

    void Model::down() {
        if (browser->next() && !browser->is_dir()) {
            reload_image();
        }
    }

    void Model::adopt_config() {
        if (!config[CFG_LATEST_SEEN].is_string()) {
             config[CFG_LATEST_SEEN] = "/"; //TODO: I should use a more smart way here to find the home dir like it is done in the main function
        }
    }

