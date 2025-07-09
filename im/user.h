#pragma once

#include "message.h"
#include "../struct/student.h"
#include "../struct/other_users.h"

#include <string>
#include <string_view>
#include <utility>
#include <functional> // For std::function

// 基础参与者接口
class IParticipant {
public:
    virtual ~IParticipant() = default;
    virtual void send_message(const Message& msg) = 0;
    virtual void receive_message(const Message& msg) = 0;
    virtual const std::string& get_nickname() const = 0;
};

template<typename T>
class Participant : public IParticipant {
protected:
    T user;
    std::string nickname;

public:
    Participant(const T& usr, const std::string_view nick)
        : user(std::move(usr)), nickname(nick) {}

    virtual ~Participant() = default;

    virtual void send_message(const Message& msg) = 0;

    virtual void receive_message(const Message& msg) = 0;

    const T& get_user() const { return user; }
    const std::string& get_nickname() const override { return nickname; }
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

// New: Callback for network messages
class NetworkParticipant : public IParticipant {
private:
    std::string nickname;
    std::string participant_id; // Unique ID provided by Go client
    std::function<void(const std::string&, const std::string&)> delivery_callback;

public:
    NetworkParticipant(std::string_view nick, std::string_view p_id, std::function<void(const std::string&, const std::string&)> callback)
        : nickname(nick), participant_id(p_id), delivery_callback(callback) {}

    void send_message(const Message& msg) override; // Not used for outgoing messages from C++ core
    void receive_message(const Message& msg) override;
    const std::string& get_nickname() const override { return nickname; }
    const std::string& get_participant_id() const { return participant_id; }
};

class BotParticipant : public Participant<Bot> {
public:
    BotParticipant(const Bot& bot, std::string_view nick)
        : Participant<Bot>(bot, nick) {}

    void send_message(const Message& msg) override;

    void receive_message(const Message& msg) override;
};
