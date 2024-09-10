#ifndef __FILEBROWSER__
#define __FILEBROWSER__

#include "Browser.h"
#include <filesystem>

    class FileBrowser : public Browser {
    public:

        FileBrowser() = default;

        int can_do(std::string path) const override;

        void update_path(std::string path, bool force_update = false) override; // throw std::runtime_error()
        void update_file(std::string file, bool force_update = false) override;

        bool prev() override;
        bool next() override;
        bool enter() override;

        std::string get_path() const override;
        std::string get_full_path() const override;

        bool is_dir() const override;

    protected:
        std::filesystem::path path;
        std::filesystem::path full_path;
    };

#endif /* __FILEBROWSER__ */

