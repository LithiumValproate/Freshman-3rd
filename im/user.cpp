#include "user.h"

void CommonParticipant::send_message(const Message& msg) {
    // Implementation for sending a message
}

void CommonParticipant::receive_message(const Message& msg) {
    // Implementation for receiving a message
}

void Moderator::send_message(const Message& msg) {
    // Implementation for sending a message
}

void Moderator::receive_message(const Message& msg) {
    // Implementation for receiving a message
}

void Moderator::mute_user(const Participant<Student>& p) {
    // Implementation for muting a user
}

void Moderator::kick_user(const Participant<Student>& p) {
    // Implementation for kicking a user
}

void MutedParticipant::send_message(const Message& msg) {
    // Implementation for sending a message while muted
}

void MutedParticipant::recieve_message(const Message& msg) {
    // Implementation for receiving a message while muted
}

void BotParticipant::send_message(const Message& msg) {

}

void BotParticipant::receive_message(const Message& msg) {
    // Implementation for receiving a message as a bot
}

