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
        Worker(std::string filename, int offs, int step, std::atomic_int *done);
        std::thread load(Block &texs, int rounds_n);

        std::shared_ptr<cv::VideoCapture> vc;

        const int offs;
        const int step;

        std::atomic_int *done;

        friend class Loader;
    };

    class Loader {
    public:
        Loader(std::string filename, media_data *image, int threads_n); // throw std::runtime_error

        void load_sync(Block &texs, int frames_n);
        void load_async(Block &texs, int frames_n);

        bool done() const;

    protected:
        std::atomic_int done_;

        const int threads_n;

        std::vector<Worker> workers;
    };

#endif /* __LOADER__ */

