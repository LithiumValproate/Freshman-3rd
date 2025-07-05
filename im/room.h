#pragma once

#include "message.h"

#include <algorithm>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace im {
    class Room {
    public:
        using ParticipantPtr = std::shared_ptr<Participant>;

        explicit Room(const std::string& name);

        void join(const ParticipantPtr& p);

        void leave(const ParticipantPtr& p);

        void broadcast(const Message& msg);

        void send(const Message& msg, const ParticipantPtr& to);

        const std::string& get_name() const;

    private:
        std::string name;
        std::vector<ParticipantPtr> participants;
        std::mutex mtx;
    };

    inline Room::Room(const std::string& n) : name(n) {}

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
} // namespace im
