#pragma once

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
