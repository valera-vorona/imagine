#include "NormalView.h"
#include "Model.h"

//#define NK_IMPLEMENTATION
#include "nuklear.h"

#include <stdexcept>

    NormalView::NormalView(Model *model, const char *path) : View(model, path) {

    }

    /* layout
     * -----------------------
     * Menu             (h=24)
     * Path edit line   (h=48)
     * Image menu line  (h=48)
     * FIle list w=300 | Image w=* (h=*)
     *                 | (optionally brogress bar h=16) 
     * Status bar       (h=24)
    */
    void NormalView::draw(int content_width, int content_height, struct image_meta *image, bool show_progress) {
        static const int LINE_HEIGHT = 24;

        auto ctx = model->get_context();
        auto browser = model->get_browser();

        if (!nk_begin(ctx, "NormalView", nk_rect(0, 0, content_width, content_height), NK_WINDOW_BORDER)) {
          nk_end(ctx);
          throw std::runtime_error("Error beginning NormalView");
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
                model->reload_image();
            } catch (std::runtime_error &e) {
                model->set_status(e.what());
            }
        }

        // Image menu line
        nk_layout_row_static(ctx, LINE_HEIGHT * 2, LINE_HEIGHT * 2, 1);
            if (nk_button_symbol(ctx, NK_SYMBOL_PLUS)) {
                model->toggle_view_mode();
            }

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
                        set_full_path(browser->get_full_path().c_str());
                        //if (!browser->is_dir()) {
                            model->reload_image();
                        //}
                    } catch (std::runtime_error &e) {
                        model->set_status(e.what());
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
            height -= 20; // TODO: case 1: deducting the group's border size which is found experementally, it is better to find out how to find the size of the current group in the nuklear code

            if (show_progress) {
                height -= 20; // it should be progress bar height + 4 by experiment. Perhaps it is the bar's or the group's border 
            }

            // calculating aspect ratios of the image and the view
            const float ar_image = (float)image->w / (float)image->h;
            const float ar_view  = (float)width / (float)height;

            if (ar_image > ar_view) {
                nk_layout_row_static(ctx, (float)width / ar_image , width, 1);
                //TODO: I create nk_image here every frame, maybe I should prepare it outside this function and send it as an argument
                nk_image(ctx, nk_image_id(image->id));
            } else {
                nk_layout_row_static(ctx, height, (float)height * ar_image, 1);
                //TODO: the same as above
                nk_image(ctx, nk_image_id(image->id));
            }

            if (show_progress) {
                nk_layout_row_static(ctx, 16 , width, 1);
                nk_progress(ctx, model->get_video_pos_ptr(), 1000. / model->get_video_fps() * model->get_video_frames_n(), nk_true);
            }

            model->set_status( std::string("w: ") + std::to_string(image->w) +
                std::string(", h: ") + std::to_string(image->h) +
                std::string(", n: ") + std::to_string(image->n) );

            nk_group_end(ctx);
        }

        // Status bar
        nk_layout_row_dynamic(ctx, LINE_HEIGHT, 1);
          nk_label(ctx, model->get_status().c_str(),  NK_TEXT_LEFT);

        nk_end(ctx);
    }

