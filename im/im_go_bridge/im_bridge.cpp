#include "im_bridge.h"
#include "../room.h"
#include "../user.h"
#include "../message.h"

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cstring> // For strdup
#include <mutex>   // For std::mutex and std::lock_guard

// Global mutex to protect shared data
static std::mutex g_mutex;

// Global callback for delivering messages to Go
static CGoMessageDeliveryCallback g_message_delivery_callback = nullptr;

// Map to store active participants, mapping Go's participant_id to C++ IParticipant
static std::map<std::string, std::shared_ptr<IParticipant>> g_participants;

// Map to store rooms, mapping room_id to im::Room
static std::map<uint64_t, std::shared_ptr<im::Room>> g_rooms;

// Helper to find a room by ID (must be called with g_mutex locked)
std::shared_ptr<im::Room> find_room_by_id_unsafe(uint64_t room_id) {
    auto it = g_rooms.find(room_id);
    if (it != g_rooms.end()) {
        return it->second;
    }
    return nullptr;
}

// Helper to find a participant by ID (must be called with g_mutex locked)
std::shared_ptr<IParticipant> find_participant_by_id_unsafe(const char* participant_id) {
    auto it = g_participants.find(participant_id);
    if (it != g_participants.end()) {
        return it->second;
    }
    return nullptr;
}

// Custom NetworkParticipant that uses the Go callback
class GoNetworkParticipant : public NetworkParticipant {
public:
    GoNetworkParticipant(std::string_view nick, std::string_view p_id)
        : NetworkParticipant(nick, p_id, [](const std::string& p_id_str, const std::string& msg_content) {
            // The callback itself doesn't need to be locked if it's just calling into Go,
            // as Go side is responsible for its own thread safety.
            if (g_message_delivery_callback) {
                g_message_delivery_callback(p_id_str.c_str(), msg_content.c_str());
            }
        }) {}
};

extern "C" {

void im_init(CGoMessageDeliveryCallback callback) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_message_delivery_callback = callback;
    std::cout << "IM system initialized with Go callback." << std::endl;
}

uint64_t im_create_room(const char* room_name) {
    if (!room_name) {
        return 0;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    std::string name_str(room_name);
    auto room = std::make_shared<im::Room>(name_str);
    g_rooms[room->get_id()] = room;
    std::cout << "Created room: " << name_str << " (ID: " << room->get_id() << ")" << std::endl;
    return room->get_id();
}

int im_join_room(uint64_t room_id, const char* participant_id, const char* nickname) {
    if (!participant_id || !nickname) {
        return -1;
    }

    std::lock_guard<std::mutex> lock(g_mutex);

    auto room = find_room_by_id_unsafe(room_id);
    if (!room) {
        std::cerr << "Error: Room with ID " << room_id << " not found." << std::endl;
        return -1;
    }

    // Create or retrieve participant
    std::shared_ptr<IParticipant> participant = find_participant_by_id_unsafe(participant_id);
    if (!participant) {
        participant = std::make_shared<GoNetworkParticipant>(nickname, participant_id);
        g_participants[participant_id] = participant;
        std::cout << "Created new participant: " << nickname << " (ID: " << participant_id << ")" << std::endl;
    } else {
        std::cout << "Participant " << nickname << " (ID: " << participant_id << ") already exists." << std::endl;
    }

    room->join(participant);
    std::cout << "Participant " << nickname << " (ID: " << participant_id << ") joined room " << room->get_name() << std::endl;
    return 0;
}

int im_send_message(uint64_t room_id, const char* sender_id, const char* message_content) {
    if (!sender_id || !message_content) {
        return -1;
    }

    std::shared_ptr<im::Room> room;
    std::shared_ptr<IParticipant> sender;

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        room = find_room_by_id_unsafe(room_id);
        if (!room) {
            std::cerr << "Error: Room with ID " << room_id << " not found." << std::endl;
            return -1;
        }

        sender = find_participant_by_id_unsafe(sender_id);
        if (!sender) {
            std::cerr << "Error: Sender with ID " << sender_id << " not found." << std::endl;
            return -1;
        }
    } // Mutex is released here

    Message msg(Message::Type::Text, std::string(message_content));
    room->broadcast(msg); // broadcast is internally thread-safe
    std::cout << "Message from " << sender->get_nickname() << " in room " << room->get_name() << ": " << message_content << std::endl;
    return 0;
}

int im_leave_room(uint64_t room_id, const char* participant_id) {
    if (!participant_id) {
        return -1;
    }

    std::lock_guard<std::mutex> lock(g_mutex);

    auto room = find_room_by_id_unsafe(room_id);
    if (!room) {
        std::cerr << "Error: Room with ID " << room_id << " not found." << std::endl;
        return -1;
    }

    auto participant = find_participant_by_id_unsafe(participant_id);
    if (!participant) {
        std::cerr << "Error: Participant with ID " << participant_id << " not found." << std::endl;
        return -1;
    }

    room->leave(participant);
    // Note: Participant is not removed from g_participants, which is a potential memory leak
    // if clients disconnect and never reconnect. For this exercise, we leave it as is.
    std::cout << "Participant " << participant->get_nickname() << " (ID: " << participant_id << ") left room " << room->get_name() << std::endl;
    return 0;
}

const char* im_get_room_name(uint64_t room_id) {
    std::lock_guard<std::mutex> lock(g_mutex);
    auto room = find_room_by_id_unsafe(room_id);
    if (room) {
        // Go is responsible for freeing this memory via im_free_string
        return strdup(room->get_name().c_str());
    }
    return nullptr;
}

uint64_t im_get_room_id(const char* room_name) {
    if (!room_name) {
        return 0;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    std::string name_str(room_name);
    for (const auto& pair : g_rooms) {
        if (pair.second->get_name() == name_str) {
            return pair.first;
        }
    }
    return 0; // Not found
}

uint64_t* im_list_room_ids(int* count) {
    if (!count) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    std::vector<uint64_t> ids;
    for (const auto& pair : g_rooms) {
        ids.push_back(pair.first);
    }

    *count = ids.size();
    if (ids.empty()) {
        return nullptr;
    }

    // Go is responsible for freeing this memory via im_free_uint64_array
    uint64_t* arr = (uint64_t*)malloc(ids.size() * sizeof(uint64_t));
    if (arr) {
        std::copy(ids.begin(), ids.end(), arr);
    }
    return arr;
}

void im_free_string(char* str) {
    free(str);
}

void im_free_uint64_array(uint64_t* arr) {
    free(arr);
}

} // extern "C"
