#ifndef __CACHE__
#define __CACHE__

#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>

    class Loader;

    struct Period {
        size_t min;
        size_t max;
    };

    class Cache {
    public:
        typedef std::vector<cv::Mat> Chunk;
        typedef std::vector<std::shared_ptr<Chunk>> Chunks;

        Cache(Period backward, Period forward, size_t size, std::shared_ptr<Loader> loader);

        class iterator {
        public:
            iterator() = default;

        protected:
            iterator(Cache *cache, size_t pos);
            Cache *cache = nullptr;
            size_t pos = 0;

        public:
            cv::Mat &operator * ();
            cv::Mat *ref();
            size_t index() const;
            iterator operator = (size_t pos);
            iterator operator ++ ();
            iterator operator ++ (int);
            friend class Cache;
        };

        iterator begin();

    protected:
        cv::Mat &get(size_t pos);
        void check_forward(size_t pos);

        size_t get_chunk_by_pos(size_t pos) const;
        size_t get_pos_in_chunk(size_t pos) const;

        Period backward;
        Period forward;
        const size_t SIZE;
        Chunks data;
        const size_t CHUNK_SIZE;
        const size_t MAX_CHUNKS_NUM;

        std::shared_ptr<Loader> loader;
    };

#endif /* __CACHE__ */

