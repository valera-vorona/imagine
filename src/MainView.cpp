#include "MainView.h"
#include "nuklear.h"
#include <stdexcept>

    MainView::MainView(nk_context *ctx) : ctx(ctx) {

    }

    void MainView::draw() {
        if (!nk_begin(ctx, "MainView", nk_rect(0, 0, 1200, 800), NK_WINDOW_BORDER)) {
          nk_end(ctx);
          throw std::runtime_error("Error creating MainView");
        }

        nk_layout_row_static(ctx, 24, 100, 3);
          nk_button_label(ctx, "File");
          nk_button_label(ctx, "Edit");
          nk_button_label(ctx, "Help");

        nk_layout_row_dynamic(ctx, 48, 1);
          nk_label(ctx, "Image menu under construction", NK_TEXT_LEFT);

        nk_layout_row_template_begin(ctx, 600);
          nk_layout_row_template_push_static(ctx, 300);
          nk_layout_row_template_push_dynamic(ctx);
        nk_layout_row_template_end(ctx);

        nk_label(ctx, "File browzer under construction", NK_TEXT_LEFT);
        nk_label(ctx, "Image view under construction", NK_TEXT_LEFT);
        /*nk_layout_row_dynamic(ctx, 0, 2);
            if (nk_group_begin(ctx)) {
                nk_group_end(ctx);
            nk_group(*/

        nk_layout_row_dynamic(ctx, 24, 1);
          nk_label(ctx, "Status bar under construction",  NK_TEXT_LEFT);

        nk_end(ctx);
    }

