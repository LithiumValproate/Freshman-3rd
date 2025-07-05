#pragma once

#include "message.h"
#include "../struct/student.h"
#include "../struct/other_users.h"

#include <string>
#include <string_view>

template<typename T>
class Participant {
protected:
    T user;
    std::string nickname;

public:
    Participant(const T& usr, std::string_view nick)
        : user(usr), nickname(nick) {}

    virtual ~Participant() = default;

    virtual void send_message(const Message& msg) = 0;

    virtual void receive_message(const Message& msg) = 0;

    const T& get_user() const { return user; }
    const std::string& get_nickname() const { return nickname; }
};

class CommonParticipant : public Participant<Student> {
public:
    CommonParticipant(const Student& usr, std::string_view nick)
        : Participant<Student>(usr, nick) {}

    void send_message(const Message& msg) override;

    void receive_message(const Message& msg) override;
};

class Moderator : public Participant<Student> {
public:
    Moderator(const Student& usr, std::string_view nick)
        : Participant<Student>(usr, nick) {}

    void send_message(const Message& msg) override;

    void receive_message(const Message& msg) override;

    void mute_user(const Participant<Student>& p);

    void kick_user(const Participant<Student>& p);
};

class MutedParticipant : public Participant<Student> {
public:
    MutedParticipant(const Student& usr, std::string_view nick)
        : Participant<Student>(usr, nick) {}

    void send_message(const Message& msg) override;

    void receive_message(const Message& msg) override;
};

class BotParticipant : public Participant<Bot> {
public:
    BotParticipant(const Bot& bot, std::string_view nick)
        : Participant<Bot>(bot, nick) {}

    void send_message(const Message& msg) override;

    void receive_message(const Message& msg) override;
};
