#ifndef __CACHE__
#define __CACHE__

#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>

    class Loader;

    struct Period {
        int min;
        int max;
    };

    typedef std::vector<cv::Mat> Block;
    typedef std::vector<Block> Blocks;

    class Cache {
    public:
        Cache(Period backward, Period forward, size_t max_size, std::shared_ptr<Loader> loader);

        size_t next(cv::Mat &mat);
        size_t set(cv::Mat &mat, size_t pos);

    protected:
        void check_forward();

        Period backward;
        Period forward;
        const size_t MAX_SIZE;
        Blocks data;
        Block::size_type i, bi; // next item to read data[bi][i]
        const int BLOCK_SIZE;

        std::shared_ptr<Loader> loader;
    };

#endif /* __CACHE__ */

