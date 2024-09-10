#ifndef __NORMALVIEW__
#define __NORMALVIEW__

#include "View.h"

    class NormalView : public View {
    public:

        NormalView(Model *model, const char *path);

        void draw(int content_width, int content_height, struct image_meta *image, bool show_progress = false) override; // throw std::runtime_error
    };

#endif /* __NORMALVIEW__ */

