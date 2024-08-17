#ifndef __MAINVIEW__
#define __MAINVIEW__

    struct nk_context;

    class MainView {
    public:

        MainView(nk_context *ctx);
        virtual ~MainView() = default;

        void draw();

    protected:
        nk_context *ctx;
    };



#endif /* __MAINVIEW__ */

