#include "Loader.h"
#include "Cache.h"
#include <opencv2/opencv.hpp>
#include <GL/gl.h>

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

    Loader::Loader(std::string filename, int threads_n) :
    threads_n(threads_n),
    done_(threads_n) {
        for (auto i = 0; i < threads_n; ++i) {
            workers.push_back({filename, i, threads_n, &done_});
        }
    }

    void Loader::load_sync(Block &texs, int frames_n) {
        done_ = 0;
        for (auto i = 0; i < threads_n; ++i) {
            workers[i].load(texs, frames_n / threads_n + (i < frames_n % threads_n ? 1 : 0)).detach();
        }
    }

    void Loader::load_async(Block &texs, int frames_n) {
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

