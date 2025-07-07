#pragma once

#include "message.h"
#include "user.h"

#include <algorithm>
#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace im {
    class Room {
    public:
        using ParticipantPtr = std::shared_ptr<IParticipant>;

        explicit Room(const std::string& name);
        Room(uint64_t room_id, const std::string& name);

        void join(const ParticipantPtr& p);

        void leave(const ParticipantPtr& p);

        void broadcast(const Message& msg);

        void send(const Message& msg, const ParticipantPtr& to);

        const std::string& get_name() const;
        uint64_t get_id() const;

    private:
        uint64_t id;
        std::string name;
        std::vector<ParticipantPtr> participants;
        std::mutex mtx;
        static std::atomic<uint64_t> next_id;
    };

    inline Room::Room(const std::string& n) : id(++next_id), name(n) {}

    inline Room::Room(uint64_t room_id, const std::string& n) : id(room_id), name(n) {
        // 更新next_id以确保不会产生重复ID
        uint64_t expected = next_id.load();
        while (expected <= room_id && !next_id.compare_exchange_weak(expected, room_id + 1)) {
            expected = next_id.load();
        }
    }

    inline void Room::join(const ParticipantPtr& p) {
        std::lock_guard<std::mutex> lock(mtx);
        participants.push_back(p);
    }

    inline void Room::leave(const ParticipantPtr& p) {
        std::lock_guard<std::mutex> lock(mtx);
        participants.erase(
                           std::remove(participants.begin(), participants.end(), p),
                           participants.end());
    }

    inline void Room::broadcast(const Message& msg) {
        std::lock_guard<std::mutex> lock(mtx);
        for (auto& p : participants) {
            p->receive_message(msg);
        }
    }

    inline void Room::send(const Message& msg, const ParticipantPtr& to) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = std::find(participants.begin(), participants.end(), to);
        if (it != participants.end()) {
            to->receive_message(msg);
        }
    }

    inline const std::string& Room::get_name() const { return name; }

    inline uint64_t Room::get_id() const { return id; }
} // namespace im
