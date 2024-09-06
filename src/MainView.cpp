#include "MainView.h"

//#define NK_IMPLEMENTATION
#include "nuklear.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <stdexcept>


    MainView::MainView(nk_context *ctx, const char *path) : ctx(ctx) {
        strcpy(path_buffer, path);
    }

    /* layout
     * -----------------------
     * Menu             (h=24)
     * Path edit line   (h=48)
     * Image menu line  (h=48)
     * FIle list w=300 | Image w=* (h=*)
     * Status bar       (h=24)
    */
    void MainView::draw(int content_width, int content_height, nk_image image) {
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
                        if (e.name == "..") {
                            strcpy(path_buffer, browser->get_full_path().c_str());
                        }
                        //if (!browser->is_dir()) {
                            reload_image();
                        //}
                    } catch (std::runtime_error &e) {
                        status = e.what();
                    }
                }

                if (!active_gone_through) {
                    y_offset += LINE_HEIGHT;
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

