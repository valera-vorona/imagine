#include "Loader.h"
#include "Model.h"
#include <opencv2/opencv.hpp>
#include <GL/gl.h>
#include <stdexcept>

struct image_meta;

    Worker::Worker(std::string filename, int offs, int step, std::atomic_int *done) :
    vc(std::make_shared<cv::VideoCapture>(filename)),
    offs(offs),
    step(step),
    done(done) {
        //vc->set(cv::CAP_PROP_POS_FRAMES, 0.);
    }

    std::thread Worker::load(Block &texs, int rounds_n) {
        return std::thread([this, &texs, rounds_n]{
            if (!vc->isOpened()) {

            }

            for (auto r = 0; r < rounds_n; ++r) {
                for (auto i = 0; i < offs; ++i) {
                    vc->grab();
                }

                if (!vc->read(texs[r * step + offs])) {
                    puts("no more images in video");
                }

                //if (texs[r * step + offs].empty()) {
                //    TODO: do something 
                //}

                for (auto i = step - 1; i > offs; --i) {
                    vc->grab();
                }
            }

            ++*done;
        });
    }

    Loader::Loader(std::string filename, media_data *media, int threads_n = 1) :
    threads_n(threads_n),
    done_(threads_n) {
        for (auto i = 0; i < threads_n; ++i) {
            workers.push_back({filename, i, threads_n, &done_});
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

    void Loader::load_sync(Block &texs, int frames_n) {
        while (!done());
        done_ = 0;
        for (auto i = 0; i < threads_n; ++i) {
            workers[i].load(texs, frames_n / threads_n + (i < frames_n % threads_n ? 1 : 0)).detach();
        }
    }

    void Loader::load_async(Block &texs, int frames_n) {
        while (!done());
        done_ = 0;
        std::vector<std::thread> ths;
        ths.reserve(threads_n);
        for (auto i = 0; i < threads_n; ++i) {
            ths.push_back( workers[i].load(texs, frames_n / threads_n + (i < frames_n % threads_n ? 1 : 0)) );
        }

        for (auto &t: ths) {
            t.join();
        }
    }

    bool Loader::done() const {
        return done_ == threads_n;
    }

