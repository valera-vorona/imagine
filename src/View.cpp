#include "View.h"
#include "Model.h"

#include "nuklear.h"

#include <cstring>
#include <stdexcept>

    char View::path_buffer[MAX_PATH_LEN]; // static path_buffer is shared among all the views

    View::View(Model *model, const char *path) : model(model) {
        if (!model) {
          throw std::runtime_error("Error creating View, Model class pointer not provided");
        }

        set_full_path(path);
    }

    void View::set_full_path(const char *path) {
        if (strlen(path) > MAX_PATH_LEN - 1) {
            memcpy(path_buffer, path, MAX_PATH_LEN - 1);
            path_buffer[MAX_PATH_LEN - 1] = '\0';
            throw std::runtime_error("Path too long");
        } else {
            strcpy(path_buffer, path);
        }
    }

    void View::draw_loader_bar_widget() {
        auto ctx = model->get_context();

        nk_command_buffer *canvas = nk_window_get_canvas(ctx);

        struct nk_rect space;
        enum nk_widget_layout_states state = nk_widget(&space, ctx);
        if (state) {
            auto cache = model->get_cache();
            auto chunks = cache->get_chunks();
            const auto chunks_num = cache->MAX_CHUNKS_NUM;
            space.w /= chunks_num;
            ++space.w; // -- to see a gap between chunks

            for (auto i = 0; i < chunks_num; ++i) {
                nk_fill_rect(canvas, space, 0, (i < chunks.size() && chunks[i] != nullptr) ? nk_rgb(80, 80, 80) : nk_rgb(180, 180, 180));
                space.x += space.w - 1; // +1 to compensate the gap
            }
        }
    }

