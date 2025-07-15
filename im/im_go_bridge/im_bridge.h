#ifndef IM_BRIDGE_H
#define IM_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h> // For uint64_t

// Callback function type for delivering messages from C++ to Go
typedef void (*CGoMessageDeliveryCallback)(const char* participant_id, const char* message_content);

// Initialize the IM system and set the message delivery callback
void im_init(CGoMessageDeliveryCallback callback);

// Create a new room
uint64_t im_create_room(const char* room_name);

// Join a room
// Returns 0 on success, non-zero on failure
int im_join_room(uint64_t room_id, const char* participant_id, const char* nickname);

// Send a message to a room
// Returns 0 on success, non-zero on failure
int im_send_message(uint64_t room_id, const char* sender_id, const char* message_content);

// Leave a room
// Returns 0 on success, non-zero on failure
int im_leave_room(uint64_t room_id, const char* participant_id);

// Get room name by ID
const char* im_get_room_name(uint64_t room_id);

// Get room ID by name
uint64_t im_get_room_id(const char* room_name);

// List all room IDs
// The caller is responsible for freeing the returned array and its contents
uint64_t* im_list_room_ids(int* count);

// Free memory allocated by C++ and returned to Go (e.g., for strings or arrays)
void im_free_string(char* str);
void im_free_uint64_array(uint64_t* arr);

#ifdef __cplusplus
}
#endif

#endif // IM_BRIDGE_H
