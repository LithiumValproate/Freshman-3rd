#include "room.h"
#include "user.h"

#include <iostream>
#include <variant>

void CommonParticipant::send_message(const Message& msg) {
    const auto& content = msg.get_content();
    std::cout << "[" << get_nickname() << "] 发送消息: ";
    if (std::holds_alternative<std::string>(content)) {
        std::cout << std::get<std::string>(content);
    } else if (std::holds_alternative<Message::ImageCtn>(content)) {
        std::cout << "[Image: " << std::get<Message::ImageCtn>(content).path << "]";
    } else if (std::holds_alternative<Message::GifCtn>(content)) {
        std::cout << "[Gif: " << std::get<Message::GifCtn>(content).path << "]";
    } else if (std::holds_alternative<Message::VideoCtn>(content)) {
        std::cout << "[Video: " << std::get<Message::VideoCtn>(content).path << "]";
    }
    std::cout << std::endl;
}

void CommonParticipant::receive_message(const Message& msg) {
    const auto& content = msg.get_content();
    std::cout << "[" << get_nickname() << "] 收到消息: ";
    if (std::holds_alternative<std::string>(content)) {
        std::cout << std::get<std::string>(content);
    } else if (std::holds_alternative<Message::ImageCtn>(content)) {
        std::cout << "[Image: " << std::get<Message::ImageCtn>(content).path << "]";
    } else if (std::holds_alternative<Message::GifCtn>(content)) {
        std::cout << "[Gif: " << std::get<Message::GifCtn>(content).path << "]";
    } else if (std::holds_alternative<Message::VideoCtn>(content)) {
        std::cout << "[Video: " << std::get<Message::VideoCtn>(content).path << "]";
    }
    std::cout << std::endl;
}

void Moderator::send_message(const Message& msg) {
    const auto& content = msg.get_content();
    std::cout << "[Moderator " << get_nickname() << "] 发送消息: ";
    if (std::holds_alternative<std::string>(content)) {
        std::cout << std::get<std::string>(content);
    } else if (std::holds_alternative<Message::ImageCtn>(content)) {
        std::cout << "[Image: " << std::get<Message::ImageCtn>(content).path << "]";
    } else if (std::holds_alternative<Message::GifCtn>(content)) {
        std::cout << "[Gif: " << std::get<Message::GifCtn>(content).path << "]";
    } else if (std::holds_alternative<Message::VideoCtn>(content)) {
        std::cout << "[Video: " << std::get<Message::VideoCtn>(content).path << "]";
    }
    std::cout << std::endl;
}

void Moderator::receive_message(const Message& msg) {
    const auto& content = msg.get_content();
    std::cout << "[Moderator " << get_nickname() << "] 收到消息: ";
    if (std::holds_alternative<std::string>(content)) {
        std::cout << std::get<std::string>(content);
    } else if (std::holds_alternative<Message::ImageCtn>(content)) {
        std::cout << "[Image: " << std::get<Message::ImageCtn>(content).path << "]";
    } else if (std::holds_alternative<Message::GifCtn>(content)) {
        std::cout << "[Gif: " << std::get<Message::GifCtn>(content).path << "]";
    } else if (std::holds_alternative<Message::VideoCtn>(content)) {
        std::cout << "[Video: " << std::get<Message::VideoCtn>(content).path << "]";
    }
    std::cout << std::endl;
}

void Moderator::mute_user(const Participant<Student>& p) {
    std::cout << "[Moderator " << get_nickname() << "] 将用户 " << p.get_nickname() << " 禁言" << std::endl;
}

void Moderator::kick_user(const Participant<Student>& p) {
    std::cout << "[Moderator " << get_nickname() << "] 将用户 " << p.get_nickname() << " 踢出" << std::endl;
}

void MutedParticipant::send_message(const Message& msg) {
    std::cout << "[" << get_nickname() << "] 你已被禁言，无法发送消息。" << std::endl;
}

void MutedParticipant::receive_message(const Message& msg) {
    const auto& content = msg.get_content();
    std::cout << "[" << get_nickname() << "] 收到消息（即使被禁言）: ";
    if (std::holds_alternative<std::string>(content)) {
        std::cout << std::get<std::string>(content);
    } else if (std::holds_alternative<Message::ImageCtn>(content)) {
        std::cout << "[Image: " << std::get<Message::ImageCtn>(content).path << "]";
    } else if (std::holds_alternative<Message::GifCtn>(content)) {
        std::cout << "[Gif: " << std::get<Message::GifCtn>(content).path << "]";
    } else if (std::holds_alternative<Message::VideoCtn>(content)) {
        std::cout << "[Video: " << std::get<Message::VideoCtn>(content).path << "]";
    }
    std::cout << std::endl;
}

void BotParticipant::send_message(const Message& msg) {
    const auto& content = msg.get_content();
    std::cout << "[Bot " << get_nickname() << "] 自动发送消息: ";
    if (std::holds_alternative<std::string>(content)) {
        std::cout << std::get<std::string>(content);
    } else if (std::holds_alternative<Message::ImageCtn>(content)) {
        std::cout << "[Image: " << std::get<Message::ImageCtn>(content).path << "]";
    } else if (std::holds_alternative<Message::GifCtn>(content)) {
        std::cout << "[Gif: " << std::get<Message::GifCtn>(content).path << "]";
    } else if (std::holds_alternative<Message::VideoCtn>(content)) {
        std::cout << "[Video: " << std::get<Message::VideoCtn>(content).path << "]";
    }
    std::cout << std::endl;
}

void BotParticipant::receive_message(const Message& msg) {
    const auto& content = msg.get_content();
    std::cout << "[Bot " << get_nickname() << "] 收到消息: ";
    if (std::holds_alternative<std::string>(content)) {
        std::cout << std::get<std::string>(content);
    } else if (std::holds_alternative<Message::ImageCtn>(content)) {
        std::cout << "[Image: " << std::get<Message::ImageCtn>(content).path << "]";
    } else if (std::holds_alternative<Message::GifCtn>(content)) {
        std::cout << "[Gif: " << std::get<Message::GifCtn>(content).path << "]";
    } else if (std::holds_alternative<Message::VideoCtn>(content)) {
        std::cout << "[Video: " << std::get<Message::VideoCtn>(content).path << "]";
    }
    std::cout << std::endl;
}
