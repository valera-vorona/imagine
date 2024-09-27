#include "FullScreenView.h"
#include "Model.h"

#include "nuklear.h"

#include <stdexcept>

    FullScreenView::FullScreenView(Model *model, const char *path) : View(model, path) {

    }

    void FullScreenView::draw(int content_width, int content_height, media_data *media, bool show_progress) {
        static const int PROGRESS_HEIGHT    = 16;
        static const int LOADER_HEIGHT      = 16;

        auto ctx = model->get_context();

        // Setting window padding to 0
        //TRICK: it's not documented in nuklear library, but I don't know how to set window padding to 0 in a different way
        auto padding = ctx->style.window.padding;
        ctx->style.window.padding = {0., 0.};

        if (!nk_begin(ctx, "FullScreenView", nk_rect(0, 0, content_width, content_height), NK_WINDOW_NO_SCROLLBAR)) {
            nk_end(ctx);
            throw std::runtime_error("Error beginning FullScreenView");
        }

        auto size = nk_window_get_content_region_size(ctx);
        const int width = size.x;
        const int height = show_progress ? size.y - PROGRESS_HEIGHT - LOADER_HEIGHT : size.y;

        // Image
        // calculating aspect ratios of the image and the view
        const float ar_image = (float)media->w / (float)media->h;
        const float ar_view  = (float)width / (float)height;

        if (ar_image > ar_view) {
            const float content_height = (float)width / ar_image;
            const float margin_height = ((float)height - content_height) / 2.;
            nk_layout_row_static(ctx, margin_height, width, 1);
            nk_spacer(ctx);
            nk_layout_row_static(ctx, content_height , width, 1);
            //TODO: I create nk_image here every frame, maybe I should prepare it outside this function and send it as an argument
            nk_image(ctx, nk_image_id(media->id));
        } else {
            //nk_layout_row_static(ctx, height, (float)height * ar_image, 1);
            const float content_width = (float)height * ar_image;
            const float margin_width = ((float)width - content_width) / 2;
            const float ratio[] = { margin_width, content_width };
            nk_layout_row(ctx, NK_STATIC, height, 2, ratio);
            nk_spacer(ctx);
            //TODO: the same as above
            nk_image(ctx, nk_image_id(media->id));
        }

        // Progress bar
        if (show_progress) {
            nk_layout_row_static(ctx, PROGRESS_HEIGHT , size.x, 1);
            nk_progress(ctx, model->get_video_pos_ptr(), model->get_video_frames_n(), nk_true);
            nk_layout_row_static(ctx, LOADER_HEIGHT , size.x, 1);
            draw_loader_bar_widget();
        }

        // Restoring window padding
        ctx->style.window.padding = padding;

        nk_end(ctx);
    }

