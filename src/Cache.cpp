#include "Cache.h"
#include "Loader.h"

    /*
     * class Cache implementation
     */

    Cache::Cache(Period backward, Period forward, size_t size, std::shared_ptr<Loader> loader) :
    backward(backward),
    forward(forward),
    SIZE(size),
    loader(loader),
    CHUNK_SIZE(forward.max),
    MAX_CHUNKS_NUM(size / CHUNK_SIZE + (size % CHUNK_SIZE ? 1 : 0)) {

    }

    Cache::iterator Cache::begin() {
        return iterator(this, 0);
    }

    cv::Mat &Cache::get(size_t pos) {
        if (pos >= SIZE) {
            pos = SIZE - 1; // Move the pos to the latest item
        }

        const auto chunk = get_chunk_by_pos(pos);
        if (data.size() <= chunk) {
            // Chunk has not been loaded
            auto chunks_to_load = chunk - data.size() + 1;
            while (chunks_to_load--) {
                data.push_back(nullptr); // Just expand the data array with nullptr chunks
            }
        }

        if (!data[chunk]) {
            // Load the chunk
            data[chunk] = std::make_shared<Chunk>(CHUNK_SIZE);
            loader->load_async(*data[chunk], CHUNK_SIZE, chunk * CHUNK_SIZE);
        }

        check_forward(pos);
        // Now the asked chunk has been loaded, we can get the item from it
        return (*data[chunk])[get_pos_in_chunk(pos)];
    }

    void Cache::check_forward(size_t pos) {
        const Period to_load{get_chunk_by_pos(pos), get_chunk_by_pos(pos + forward.max)};

        for (auto chunk = to_load.min; chunk <= to_load.max; ++chunk) {
            if (data.size() <= chunk) {
                data.push_back(nullptr); // Expand array with only one chunk
            }

            // Looad only one chunk
            if (!data[chunk]) {
                data[chunk] = std::make_shared<Chunk>(CHUNK_SIZE);
                if (loader->done()) {
                    loader->load_sync(*data[chunk], CHUNK_SIZE, chunk * CHUNK_SIZE);
                }

                break;
            }
        }
    }

    size_t Cache::get_chunk_by_pos(size_t pos) const {
        return pos / CHUNK_SIZE;
    }

    size_t Cache::get_pos_in_chunk(size_t pos) const {
        return pos % CHUNK_SIZE;
    }

    /*
     * class Cache::iterator implementation
     */

    Cache::iterator::iterator(Cache *cache, size_t pos) :
    cache(cache),
    pos(pos) {

    }

    cv::Mat &Cache::iterator::operator * () {
        return cache->get(pos);
    }

    cv::Mat *Cache::iterator::ref() {
        return &cache->get(pos);
    }

    size_t Cache::iterator::index() const {
        return pos;
    }

    Cache::iterator Cache::iterator::operator = (size_t pos) {
        this->pos = pos;
        return *this;
    }

    Cache::iterator Cache::iterator::operator ++ () {
        ++pos;
        return *this;
    }

    Cache::iterator Cache::iterator::operator ++ (int) {
        auto rv = *this;
        ++pos;
        return rv;
    }

