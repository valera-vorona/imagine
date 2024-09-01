#include "MainView.h"

//#define NK_IMPLEMENTATION
#include "nuklear.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <stdexcept>


#include "FileBrowser.h"

/* config constants */
const std::string CFG_LATEST_SEEN = "latest_seen";

struct nk_image load_image(const char *filename, struct image_meta *image_meta) // throw std::runtine_error
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
        image_meta->w = x;
        image_meta->h = y;
        image_meta->n = n;
    }
    return nk_image_id((int)tex);
}

    MainView::MainView(std::string config_file, nk_context *ctx, int content_width, int content_height) :
        config_file(config_file),
        ctx(ctx),
        content_width(content_width),
        content_height(content_height),
        current_image(new struct nk_image) {

        std::ifstream config_stream(config_file); 
        config = nlohmann::json::parse(config_stream);
        config_stream.close();

        adopt_config();

        add_browser(std::move(std::make_unique<FileBrowser>()));
        browser = browsers.back().get();

        browser->update_path(config[CFG_LATEST_SEEN].get<std::string>());

        strcpy(path_buffer, browser->get_full_path().c_str());

        try {
            *current_image = load_image(path_buffer, &current_image_meta);
        } catch (std::runtime_error &e) {
            status = e.what();
        }
    }

    MainView::~MainView() {
        glDeleteTextures(1, (const GLuint*)&current_image->handle.id);
        delete current_image;

        std::ofstream config_stream(config_file, std::ofstream::out | std::ofstream::trunc);
        config_stream << config;
    }

    void MainView::add_browser(std::unique_ptr<Browser> browser) {
        browsers.push_back(std::move(browser));
    }

    void MainView::set_size(int width, int height) {
        content_width = width;
        content_height = height;
    }

    /* layout
     * -----------------------
     * Menu             (h=24)
     * Path edit line   (h=48)
     * Image menu line  (h=48)
     * FIle list w=300 | Image w=* (h=*)
     * Status bar       (h=24)
    */
    void MainView::draw() {
        static const int LINE_HEIGHT = 24;

        status.clear();

        if (!nk_begin(ctx, "MainView", nk_rect(0, 0, content_width, content_height), NK_WINDOW_BORDER)) {
          nk_end(ctx);
          throw std::runtime_error("Error creating MainView");
        }

        // Menu
        nk_layout_row_static(ctx, LINE_HEIGHT, 100, 3);
          nk_button_label(ctx, "File");
          nk_button_label(ctx, "Edit");
          nk_button_label(ctx, "Help");

        // Path edit line
        nk_layout_row_template_begin(ctx, LINE_HEIGHT * 2);
            nk_layout_row_template_push_dynamic(ctx);
            nk_layout_row_template_push_static(ctx, LINE_HEIGHT * 2);
        nk_layout_row_template_end(ctx);

        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, path_buffer, MAX_PATH_LEN, nk_filter_default);

        if (nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE)) {
            try {
                browser->update_path(path_buffer);
                reload_image();
            } catch (std::runtime_error &e) {
                status = e.what();
            }
        }

        // Image menu line
        nk_layout_row_dynamic(ctx, LINE_HEIGHT * 2, 1);
          nk_label(ctx, "Image menu under construction", NK_TEXT_LEFT);

        // Main part
        // Width is the whole window width - width of file list
        int width = nk_window_get_content_region_size(ctx).x - 400;
        // Height is calculated from the window content region height - sum of all the others' widget sizes
        int height = nk_window_get_content_region_size(ctx).y - LINE_HEIGHT * 7;
        int y_offset = 0; // offset in pixels of the active file
        nk_layout_row_template_begin(ctx, height);
            nk_layout_row_template_push_static(ctx, 400);
            nk_layout_row_template_push_static(ctx, width);
        nk_layout_row_template_end(ctx);

        //  File list
        if (nk_group_begin(ctx, "File list", NK_WINDOW_BORDER)) {
            bool active_gone_through = false;

            nk_layout_row_dynamic(ctx, LINE_HEIGHT, 1);

            for (const auto &e : browser->get_dir()) {
                nk_symbol_type symbol = e.is_directory ? NK_SYMBOL_TRIANGLE_RIGHT : NK_SYMBOL_NONE;
                bool pushed = false;
                if (e.is_active) {
                    nk_style_push_style_item(ctx, &ctx->style.button.normal, nk_style_item_color(nk_rgb(255, 0, 0)));
                    active_gone_through = true;
                    pushed = true;
                }

                if (nk_button_symbol_label(ctx, symbol, e.name.c_str(), NK_TEXT_RIGHT)) {
                    try {
                        browser->update_file(e.name);
                        if (!browser->is_dir()) {
                            reload_image();
                        }
                    } catch (std::runtime_error &e) {
                        status = e.what();
                    }
                }

                if (!active_gone_through) {
                    y_offset += LINE_HEIGHT;;
                }

                if (pushed) {
                    nk_style_pop_style_item(ctx);
                }
            }

            nk_group_end(ctx);
        }

        //nk_group_set_scroll(ctx, "File list", 0, y_offset);

        // Image
        if (nk_group_begin(ctx, "Image", NK_WINDOW_BORDER)) {
            width -= 20;
            height -= 20; // deducting the group's border size which is found experementally, it is better to find out how to find the size of the current group in the nuklear code

            // calculating aspect ratios of the image and the view
            const float ar_image = (float)current_image_meta.w / (float)current_image_meta.h;
            const float ar_view  = (float)width / (float)height;

            if (ar_image > ar_view) {
                nk_layout_row_static(ctx, (float)width / ar_image , width, 1);
                nk_image(ctx, *current_image);
            } else {
                nk_layout_row_static(ctx, height, (float)height * ar_image, 1);
                nk_image(ctx, *current_image);
            }

            status = std::string("w: ") + std::to_string(current_image_meta.w) +
                std::string(", h: ") + std::to_string(current_image_meta.h) +
                std::string(", n: ") + std::to_string(current_image_meta.n);
            nk_group_end(ctx);
        }

        // Status bar
        nk_layout_row_dynamic(ctx, LINE_HEIGHT, 1);
          nk_label(ctx, status.c_str(),  NK_TEXT_LEFT);

        nk_end(ctx);
    }

    void MainView::reload_image() {
        try {
            glDeleteTextures(1, (const GLuint*)&current_image->handle.id);
            *current_image = load_image(browser->get_full_path().c_str(), &current_image_meta);
            strcpy(path_buffer, browser->get_full_path().c_str());
            config[CFG_LATEST_SEEN] = path_buffer;
        } catch (std::runtime_error &e) {
            status = e.what();
        }
    }

    void MainView::up() {
        if (browser->prev() && !browser->is_dir()) {
            reload_image();
        }
    }

    void MainView::down() {
        if (browser->next() && !browser->is_dir()) {
            reload_image();
        }
    }

    void MainView::adopt_config() {
        if (!config[CFG_LATEST_SEEN].is_string()) {
             config[CFG_LATEST_SEEN] = "/"; //TODO: I should use a more smart way here to find the home dir like it is done in the main function
        }
    }

