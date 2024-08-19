#ifndef __MAINVIEW__
#define __MAINVIEW__

    struct nk_context;
    struct nk_image;

    class MainView {
    public:

        MainView(nk_context *ctx);
        virtual ~MainView() = default;

        void draw(struct nk_image *image);

    protected:
        nk_context *ctx;
    };



#endif /* __MAINVIEW__ */

