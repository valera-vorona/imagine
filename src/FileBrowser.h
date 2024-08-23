#ifndef __FILEBROWSER__
#define __FILEBROWSER__

#include <vector>
#include <string>
#include <filesystem>

    class FileBrowser {
    public:

        struct FileEntry {
            bool is_directory;
            bool is_active;
            std::string name;
        };

        FileBrowser(std::string path);

        ~FileBrowser() = default;

        typedef std::vector<FileEntry> Files;

        void update(std::string path); // throw std::runtime_error()

        bool prev();
        bool next();

        inline std::string get_path() const {
            return path;
        }

        inline std::string get_full_path() const {
            return full_path;
        }

        inline const Files &get_dir() const {
            return files;
        }

    protected:
        std::filesystem::path path;
        std::filesystem::path full_path;
        Files files;
    };



#endif /* __FILEBROWSER__ */

