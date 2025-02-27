#ifndef __LOADER__
#define __LOADER__

#include "Cache.h"
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include <string>

    namespace cv {
        class VideoCapture;
    }

    struct media_data;

    class Worker {
        Worker(std::string filename, int offs, int step, std::atomic_int *read, std::atomic_int *done);

        std::thread load(Cache::Chunk &texs, int rounds_n, size_t pos);

        std::shared_ptr<cv::VideoCapture> vc;

        const int offs;
        const int step;

        std::atomic_int *read;
        std::atomic_int *done;

        friend class Loader;
    };

    class Loader : public std::enable_shared_from_this<Loader> {
    public:
        Loader(std::string filename, media_data *image, int threads_n = 1); // throw std::runtime_error

        void load_sync(Cache::Chunk &texs, int frames_n, size_t pos = -1);
        void load_async(Cache::Chunk &texs, int frames_n, size_t pos = -1);

        bool complete() const;
        bool done() const;

    protected:
        std::atomic_int read_;
        std::atomic_int done_;

        const int threads_n;

        std::vector<Worker> workers;
    };

#endif /* __LOADER__ */

