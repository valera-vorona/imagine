#include "Loader.h"
#include "Model.h"
#include <opencv2/opencv.hpp>
#include <GL/gl.h>
#include <stdexcept>

struct image_meta;

    Worker::Worker(std::string filename, int offs, int step, std::atomic_int *read, std::atomic_int *done) :
    vc(std::make_shared<cv::VideoCapture>(filename)),
    offs(offs),
    step(step),
    read(read),
    done(done) {

    }

    std::thread Worker::load(Block &texs, int rounds_n, size_t pos) {
        return std::thread([this, &texs, rounds_n, pos] {
            if ((signed long int)pos != -1) {
                vc->set(cv::CAP_PROP_POS_FRAMES, pos);
            }

            auto succeed = true;
            if (!vc->isOpened()) {

            }

            for (auto r = 0; succeed && r < rounds_n; ++r) {
                for (auto i = 0; succeed && i < offs; ++i) {
                    succeed &= vc->grab();
                }

                succeed &= succeed && vc->read(texs[r * step + offs]);

                for (auto i = step - 1; succeed && i > offs; --i) {
                    succeed &= vc->grab();
                }
            }

            if (succeed) {
                ++*read;
            }

            ++*done;
        });
    }

    Loader::Loader(std::string filename, media_data *media, int threads_n) :
    threads_n(threads_n),
    read_(threads_n),
    done_(threads_n) {
        for (auto i = 0; i < threads_n; ++i) {
            workers.push_back({filename, i, threads_n, &read_, &done_});
            if (!workers.back().vc->isOpened()) {
                throw std::runtime_error(std::string("Can't open video file: '") + filename + "'");
            }
        }

        if (media && threads_n > 0) {
            const auto &vc = workers[0].vc;
            media->w                    = vc->get(cv::CAP_PROP_FRAME_WIDTH);
            media->h                    = vc->get(cv::CAP_PROP_FRAME_HEIGHT);
            media->fps                  = vc->get(cv::CAP_PROP_FPS);
            media->frames_n             = vc->get(cv::CAP_PROP_FRAME_COUNT);
            media->pos = media->pos2    = vc->get(cv::CAP_PROP_POS_MSEC);
        }
    }

    void Loader::load_sync(Block &texs, int frames_n, size_t pos) {
        while (!done());
        read_ = done_ = 0;
        for (auto i = 0; i < threads_n; ++i) {
            workers[i].load(texs, frames_n / threads_n + (i < frames_n % threads_n ? 1 : 0), pos).detach();
        }
    }

    void Loader::load_async(Block &texs, int frames_n, size_t pos) {
        while (!done());
        read_ = done_ = 0;
        std::vector<std::thread> ths;
        ths.reserve(threads_n);
        for (auto i = 0; i < threads_n; ++i) {
            ths.push_back( workers[i].load(texs, frames_n / threads_n + (i < frames_n % threads_n ? 1 : 0), pos) );
        }

        for (auto &t: ths) {
            t.join();
        }
    }

    bool Loader::complete() const {
        return read_ == threads_n;
    }

    bool Loader::done() const {
        return done_ == threads_n;
    }

