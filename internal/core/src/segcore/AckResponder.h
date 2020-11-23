#pragma once
#include <shared_mutex>
#include <mutex>
#include <set>
#include <atomic>
namespace milvus::segcore {
class AckResponder {
 public:
    void
    AddSegment(int64_t seg_begin, int64_t seg_end) {
        std::lock_guard lck(mutex_);
        fetch_and_flip(seg_end);
        auto old_begin = fetch_and_flip(seg_begin);
        if (old_begin) {
            minimum_ = *acks_.begin();
        }
    }

    int64_t
    GetAck() const {
        return minimum_;
    }

 private:
    bool
    fetch_and_flip(int64_t endpoint) {
        if (acks_.count(endpoint)) {
            acks_.erase(endpoint);
            return true;
        } else {
            acks_.insert(endpoint);
            return false;
        }
    }

 private:
    std::shared_mutex mutex_;
    std::set<int64_t> acks_ = {0};
    std::atomic<int64_t> minimum_ = 0;
};
}  // namespace milvus::segcore
