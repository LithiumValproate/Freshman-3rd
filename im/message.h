#pragma once

#include <string>
#include <variant>

#ifdef USE_QTJSON
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#endif

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


#ifdef USE_QTJSON

inline auto message_type_to_qjson_string(Message::Type type) -> QString {
    switch (type) {
        case Message::Type::Text: return "Text";
        case Message::Type::Image: return "Image";
        case Message::Type::Gif: return "Gif";
        case Message::Type::Video: return "Video";
        case Message::Type::Emoji: return "Emoji";
        default: return "Unknown";
    }
}

inline auto message_type_from_qjson_string(const QString& str) -> Message::Type {
    if (str == "Text") return Message::Type::Text;
    else if (str == "Image") return Message::Type::Image;
    else if (str == "Gif") return Message::Type::Gif;
    else if (str == "Video") return Message::Type::Video;
    else if (str == "Emoji") return Message::Type::Emoji;
    return Message::Type::Text; // Default to Text
}

inline auto message_content_to_qjson(const Message::Content& content) -> QJsonValue {
    if (std::holds_alternative<std::string>(content)) {
        return QString::fromStdString(std::get<std::string>(content));
    } else if (std::holds_alternative<Message::ImageCtn>(content)) {
        const auto& img = std::get<Message::ImageCtn>(content);
        return QJsonObject{{"path", QString::fromStdString(img.path)}};
    } else if (std::holds_alternative<Message::GifCtn>(content)) {
        const auto& gif = std::get<Message::GifCtn>(content);
        return QJsonObject{{"path", QString::fromStdString(gif.path)}};
    } else if (std::holds_alternative<Message::VideoCtn>(content)) {
        const auto& video = std::get<Message::VideoCtn>(content);
        return QJsonObject{{"path", QString::fromStdString(video.path)}};
    }
    return QJsonValue();
}

inline auto message_conntent_from_qjson(const QJsonValue& value) -> Message::Content {
    if (value.isString()) {
        return value.toString().toStdString();
    } else if (value.isObject()) {
        const auto obj = value.toObject();
        if (obj.contains("path")) {
            const std::string path = obj["path"].toString().toStdString();
            if (obj.contains("type")) {
                const QString type = obj["type"].toString();
                if (type == "Image") {
                    return Message::ImageCtn{path};
                } else if (type == "Gif") {
                    return Message::GifCtn{path};
                } else if (type == "Video") {
                    return Message::VideoCtn{path};
                }
            }
        }
    }
    return std::string(); // Default to empty string
}

inline auto message_to_qjson(const Message& msg) -> QJsonObject {
    QJsonObject obj;
    obj["type"] = message_type_to_qjson_string(msg.get_type());
    obj["content"] = message_content_to_qjson(msg.get_content());
    return obj;
}

inline auto message_from_qjson(const QJsonObject& obj) -> Message {
    Message::Type type = message_type_from_qjson_string(obj["type"].toString());
    Message::Content content = message_conntent_from_qjson(obj["content"]);
    return Message(type, content);
}

#endif