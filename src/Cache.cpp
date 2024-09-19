#include "Cache.h"
#include "Loader.h"

    Cache::Cache(Period backward, Period forward, std::shared_ptr<Loader> loader) :
    backward(backward),
    forward(forward),
    loader(loader),
    BLOCK_SIZE(backward.max + forward.max),
    i(0),
    bi(0) {

    }

    size_t Cache::next(cv::Mat &mat) {
        for (;;) {
            if (bi < data.size()) {
                if (i < BLOCK_SIZE) {
                    check_forward();
                    mat = data[bi][i];
                    return bi * BLOCK_SIZE + i++;
                } else {
                    i = 0;
                    ++bi;
                }
            } else {
                data.push_back({});
                data.back().resize(BLOCK_SIZE);
                loader->load_async(data.back(), BLOCK_SIZE);
            }
        }
    }

    void Cache::check_forward() {
        if ((data.size() - bi - 1) * BLOCK_SIZE < forward.min) {
            if (loader->done()) {
                data.push_back({});
                data.back().resize(BLOCK_SIZE);
                loader->load_sync(data.back(), BLOCK_SIZE);
            }
        }
    }

