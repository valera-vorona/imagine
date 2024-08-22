#ifndef __FILEBROWSER__
#define __FILEBROWSER__

#include <list>
#include <string>
#include <filesystem>

    class FileBrowser {
    public:

        struct FileEntry {
            bool is_directory;
            std::string name;
        };

        FileBrowser(std::string path);

        ~FileBrowser() = default;

        typedef std::list<FileEntry> Files;

        void update(std::string path); // throw std::runtime_error()

        inline Files get_dir() {
            return files;
        }

    protected:
        std::filesystem::path path;
        Files files;
    };



#endif /* __FILEBROWSER__ */

