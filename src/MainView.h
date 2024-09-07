#ifndef __MAINVIEW__
#define __MAINVIEW__

#include "View.h"

    class MainView : public View {
    public:

        MainView(Model *model, const char *path);

        void draw(int content_width, int content_height, struct image_meta *image) override; // throw std::runtime_error
    };

#endif /* __MAINVIEW__ */

