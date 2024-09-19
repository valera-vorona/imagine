#include "Cache.h"
#include "Loader.h"

    Cache::Cache(Period backward, Period forward, size_t max_size, std::shared_ptr<Loader> loader) :
    backward(backward),
    forward(forward),
    MAX_SIZE(max_size),
    loader(loader),
    BLOCK_SIZE(backward.max + forward.max),
    i(0),
    bi(0) {

    }

    size_t Cache::next(cv::Mat &mat) {
        const size_t pos = bi * BLOCK_SIZE + i;

        if (pos >= MAX_SIZE) {
            mat = data[i == 0 ? data.size() - 2 : data.size() - 1][i - 1 % BLOCK_SIZE];
            return pos - 1;
        } else {
            for (;;) {
                if (bi < data.size()) {
                    if (i < BLOCK_SIZE) {
                        check_forward();
                        if (data[bi][i].empty()) {
                            loader->load_async(data[bi], BLOCK_SIZE, pos - pos % BLOCK_SIZE);
                        }

                        mat = data[bi][i];
                        return bi * BLOCK_SIZE + i++;
                    } else {
                        i = 0;
                        ++bi;
                    }
                } else {
                    if (loader->done()) {
                        if (loader->complete()) {
                            data.push_back({});
                            data.back().resize(BLOCK_SIZE);
                            loader->load_async(data.back(), BLOCK_SIZE);
                        }
                    }
                }
            }
        }
    }

    size_t Cache::set(cv::Mat &mat, size_t pos) {
        if (pos >= MAX_SIZE) {
            mat = data[i == 0 ? data.size() - 2 : data.size() - 1][i - 1 % BLOCK_SIZE];
            return pos - 1;
        } else {
            bi = pos / BLOCK_SIZE;
            i = pos % BLOCK_SIZE;

            if ((pos - 1) / BLOCK_SIZE > data.size()) {
                // fast forward
                int num = pos / BLOCK_SIZE + 1 - data.size();
                while (num--) {
                    data.push_back({});
                    data.back().resize(BLOCK_SIZE);
                }
            }

            if (data[bi][i].empty()) {
                loader->load_async(data[bi], BLOCK_SIZE, pos - pos % BLOCK_SIZE);
            }

            mat = data[bi][i];
            return bi * BLOCK_SIZE + i;
        }
    }

    void Cache::check_forward() {

        if ((data.size() - bi - 1) * BLOCK_SIZE < forward.min) {
            if (loader->done()) {
                if (loader->complete()) {
                    data.push_back({});
                    data.back().resize(BLOCK_SIZE);
                    loader->load_sync(data.back(), BLOCK_SIZE);
                }
            }
        }
    }

