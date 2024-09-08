#include "FullScreenView.h"
#include "Model.h"

//#define NK_IMPLEMENTATION
#include "nuklear.h"

#include <stdexcept>

    FullScreenView::FullScreenView(Model *model, const char *path) : View(model, path) {

    }

    void FullScreenView::draw(int content_width, int content_height, struct image_meta *image) {
        auto ctx = model->get_context();

        if (!nk_begin(ctx, "FullScreenView", nk_rect(0, 0, content_width, content_height), NK_WINDOW_BORDER)) {
          nk_end(ctx);
          throw std::runtime_error("Error beginning FullScreenView");
        }

        auto size = nk_window_get_content_region_size(ctx);
        const int width = size.x - 20;
        const int height = size.y - 20;

        //nk_layout_row_static(ctx, size.y, size.x, 1);

        // Image
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

        nk_end(ctx);
    }

