#ifndef __BROWSER__
#define __BROWSER__

#include <vector>
#include <string>

    class Browser {
    public:

        struct FileEntry {
            bool is_directory;
            bool is_active;
            std::string name;
        };

        Browser() = default;

        virtual ~Browser() = default;

        virtual int can_do(std::string path) const = 0;

        typedef std::vector<FileEntry> Files;

        virtual void update_path(std::string path, bool force_update = false) = 0; // throw std::runtime_error()
        virtual void update_file(std::string file, bool force_update = false) = 0;

        virtual bool prev() = 0;
        virtual bool next() = 0;
        virtual bool enter() = 0;

        virtual std::string get_path() const = 0;
        virtual std::string get_full_path() const = 0;

        inline const Files get_dir() const {
            return files;
        }

        virtual bool is_dir() const = 0;

    protected:
        Files files;
    };

#endif /* __BROWSER__ */

