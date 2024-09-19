#ifndef __FULLSCREENVIEW__
#define __FULLSCREENVIEW__

#include "View.h"

    class FullScreenView : public View {
    public:

        FullScreenView(Model *model, const char *path);

        void draw(int content_width, int content_height, media_data *media, bool show_progress = false) override; // throw std::runtime_error
    };

#endif /* __FULLSCREENVIEW__ */

