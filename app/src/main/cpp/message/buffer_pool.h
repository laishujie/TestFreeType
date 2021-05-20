#ifndef TRINITY_BUFFER_POOL_H
#define TRINITY_BUFFER_POOL_H

#include <set>
#include <vector>
#include <mutex>
#include "../util/logUtil.h"

class BufferPool {
 public:
    explicit BufferPool(int buffer_size) : buffer_size_(buffer_size) {};
    virtual ~BufferPool() {
        LOGCATI("enter: %s size: %d", __func__, buffers_.size());
        for (auto buffer : buffers_) {
            free(buffer);
        }
        buffers_.clear();
        LOGCATI("leave: %s", __func__);
    }

    // int* p = buffer_->GetBuffer<int>();
    template<typename T>
    T* GetBuffer() {
        std::lock_guard<std::mutex> guard(mutex_);
        if (!buffers_.empty()) {
            auto buffer = buffers_.back();
            buffers_.pop_back();
            return reinterpret_cast<T*>(buffer);
        } else {
            auto buffer = malloc(static_cast<size_t>(buffer_size_));
            return reinterpret_cast<T*>(buffer);
        }
    }

    template<typename T>
    void ReleaseBuffer(T* buffer) {
        if (nullptr == buffer) {
            return;
        }
        std::lock_guard<std::mutex> guard(mutex_);
        buffers_.push_back(buffer);
    }

 private:
    int buffer_size_;
    std::mutex mutex_;
    std::vector<void*> buffers_;
};

#endif  // TRINITY_BUFFER_POOL_H
