#pragma once

#include "../struct/student.h"

#include <string>
#include <variant>

class Message {
public:
    enum class Type {
        Text,
        Image,
        Gif,
        Video,
        Emoji
    };

    using Content = std::variant<
        std::string, // 文字或emoji
        std::string, // 图片路径
        std::string, // gif路径
        std::string  // 视频路径
    >;

    Message(Type typ, const Content& ctn) : type(typ), content(ctn) {}

    Type get_type() const { return type; }
    const Content& get_content() const { return content; }

private:
    Type type;
    Content content;
};


class Participant {
    Student user;
    std::string nickname;

public:
    Participant(const Student& usr, const std::string& nick)
        : user(usr), nickname(nick) {}

    virtual ~Participant() = default;

    virtual void send_message(const Message& msg) = 0;

    virtual void receive_message(const Message& msg) = 0;
};