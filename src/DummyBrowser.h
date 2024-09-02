#ifndef __DUMMYBROWSER__
#define __DUMMYBROWSER__

#include "Browser.h"

    class DummyBrowser : public Browser {
    public:

        DummyBrowser() = default;

        int can_do(std::string path) const override;

        void update_path(std::string path, bool force_update = false) override; // throw std::runtime_error()
        void update_file(std::string file, bool force_update = false) override;

        bool prev() override;
        bool next() override;

        std::string get_path() const override;
        std::string get_full_path() const override;

        bool is_dir() const override;
    };

#endif /* __DUMMYBROWSER__ */

