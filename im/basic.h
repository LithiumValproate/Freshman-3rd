#pragma once

#include <string>
#include <string_view>
#include <variant>

#include "../struct/student.h"

class Message {
public:
    enum class Type {
        Text,
        Image,
        Gif,
        Video,
        Emoji
    };

    // 明确每种类型的内容
    struct ImageCtn { std::string path; };
    struct GifCtn { std::string path; };
    struct VideoCtn { std::string path; };

    using Content = std::variant<
        std::string,      // Text or Emoji
        ImageCtn,     // Image
        GifCtn,       // Gif
        VideoCtn      // Video
    >;

    Message(Type typ, const Content& ctn) : type(typ), content(ctn) {}

    Type get_type() const { return type; }
    const Content& get_content() const { return content; }

private:
    Type type;
    Content content;
};


class Participant {
protected:
    Student user;
    std::string nickname;

public:
    Participant(const Student& usr, std::string_view nick)
        : user(usr), nickname(nick) {}

    virtual ~Participant() = default;

    virtual void send_message(const Message& msg) = 0;
    virtual void receive_message(const Message& msg) = 0;

    const Student& get_user() const { return user; }
    const std::string& get_nickname() const { return nickname; }
};