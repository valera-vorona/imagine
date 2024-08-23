#include "MainView.h"

//#define NK_IMPLEMENTATION
#include "nuklear.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdexcept>


struct nk_image load_image(const char *filename) // throw std::runtine_error
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

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, ifmt, x, y, 0, fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return nk_image_id((int)tex);
}

    MainView::MainView(nk_context *ctx, int content_width, int content_height) :
        ctx(ctx),
        content_width(content_width),
        content_height(content_height),
        file_browser("/home/valeri/Pictures/1.jpg"),
        current_image(new struct nk_image) {

        strcpy(path_buffer, file_browser.get_full_path().c_str());

        try {
            *current_image = load_image(path_buffer);
        } catch (std::runtime_error &e) {
            status = e.what();
        }
    }

    MainView::~MainView() {
        glDeleteTextures(1, (const GLuint*)&current_image->handle.id);
        delete current_image;
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
                file_browser.update_path(path_buffer);
                reload_image();
            } catch (std::runtime_error &e) {
                status = e.what();
            }
        }

        // Image menu line
        nk_layout_row_dynamic(ctx, LINE_HEIGHT * 2, 1);
          nk_label(ctx, "Image menu under construction", NK_TEXT_LEFT);

        // Main part
        // Height is calculated from the window content region height - sum of all the others' widget sizes
        nk_layout_row_template_begin(ctx, nk_window_get_content_region_size(ctx).y - LINE_HEIGHT * 7);
            nk_layout_row_template_push_static(ctx, 400);
            nk_layout_row_template_push_static(ctx, nk_window_get_content_region_size(ctx).y - 400);
        nk_layout_row_template_end(ctx);

        {
            //  File list
            nk_list_view out;
            if (nk_list_view_begin(ctx, &out, "File list", NK_WINDOW_BORDER, LINE_HEIGHT, 2)) {
                nk_layout_row_dynamic(ctx, LINE_HEIGHT, 1);

                for (const auto &e : file_browser.get_dir()) {
                    nk_symbol_type symbol = e.is_directory ? NK_SYMBOL_TRIANGLE_RIGHT : NK_SYMBOL_NONE;
                    bool pushed = false;
                    if (e.is_active) {
                        nk_style_push_style_item(ctx, &ctx->style.button.normal, nk_style_item_color(nk_rgb(255, 0, 0)));
                        pushed = true;
                    }

                    if (nk_button_symbol_label(ctx, symbol, e.name.c_str(), NK_TEXT_RIGHT)) {
                        try {
                            file_browser.update_file(e.name);
                            if (!file_browser.is_dir()) {
                                reload_image();
                            }
                        } catch (std::runtime_error &e) {
                            status = e.what();
                        }
                    }

                    if (pushed) {
                        nk_style_pop_style_item(ctx);
                    }
                }

                nk_list_view_end(&out);
            }
        }

        // Image
        nk_image(ctx, *current_image);

        // Status bar
        nk_layout_row_dynamic(ctx, LINE_HEIGHT, 1);
          nk_label(ctx, status.c_str(),  NK_TEXT_LEFT);

        nk_end(ctx);
    }

    void MainView::reload_image() {
        try {
            glDeleteTextures(1, (const GLuint*)&current_image->handle.id);
            *current_image = load_image(file_browser.get_full_path().c_str());
            strcpy(path_buffer, file_browser.get_full_path().c_str());
        } catch (std::runtime_error &e) {
            status = e.what();
        }
    }

    void MainView::up() {
        if (file_browser.prev() && !file_browser.is_dir()) {
            reload_image();
        }
    }

    void MainView::down() {
        if (file_browser.next() && !file_browser.is_dir()) {
            reload_image();
        }
    }

