#include "room.h"

namespace im {
    // 静态成员定义
    std::atomic<uint64_t> Room::next_id{0};
}