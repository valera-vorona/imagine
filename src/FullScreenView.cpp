#include "FullScreenView.h"
#include "Model.h"

#include "nuklear.h"

#include <stdexcept>

    FullScreenView::FullScreenView(Model *model, const char *path) : View(model, path) {

    }

    void FullScreenView::draw(int content_width, int content_height, media_data *media, bool show_progress) {
        auto ctx = model->get_context();

        if (!nk_begin(ctx, "FullScreenView", nk_rect(0, 0, content_width, content_height), NK_WINDOW_NO_SCROLLBAR)) {
          nk_end(ctx);
          throw std::runtime_error("Error beginning FullScreenView");
        }

        auto size = nk_window_get_content_region_size(ctx);
        const int width = size.x;
        const int height = size.y - (show_progress ? 8 + 16 + 4 : 8);

        // Image
        // calculating aspect ratios of the image and the view
        const float ar_image = (float)media->w / (float)media->h;
        const float ar_view  = (float)width / (float)height;

        if (ar_image > ar_view) {
            nk_layout_row_static(ctx, (float)width / ar_image , width, 1);
            //TODO: I create nk_image here every frame, maybe I should prepare it outside this function and send it as an argument
            nk_image(ctx, nk_image_id(media->id));
        } else {
            nk_layout_row_static(ctx, height, (float)height * ar_image, 1);
            //TODO: the same as above
            nk_image(ctx, nk_image_id(media->id));
        }

        // Progress bar
        if (show_progress) {
            nk_layout_row_static(ctx, 16 , size.x, 1);
            nk_progress(ctx, model->get_video_pos_ptr(), model->get_video_frames_n(), nk_true);
        }

        nk_end(ctx);
    }

