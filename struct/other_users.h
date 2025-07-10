#pragma once

#include <string>
#include <unordered_map>

enum class UserType {
    Admin   = 1,
    Bot     = 0,
    Teacher = 2,
};


class OtherUsers {
protected:
    std::unordered_map<UserType, std::string> idStr;

public:
    OtherUsers(UserType type, const std::string& id_str) {
        idStr[type] = id_str;
    }

    ~OtherUsers() = default;

    auto get_type() const -> UserType {
        for (const auto& pair : idStr) {
            if (!pair.second.empty()) {
                return pair.first;
            }
        }
        throw std::runtime_error("No valid user type found");
    }

    auto get_id() const -> std::string {
        for (const auto& pair : idStr) {
            if (!pair.second.empty()) {
                return pair.second;
            }
        }
        throw std::runtime_error("No valid user ID found");
    }
};

class Admin : public OtherUsers {
public:
    Admin(const std::string& id) : OtherUsers(UserType::Admin, id) {}

    ~Admin() = default;
};

class Bot : public OtherUsers {
public:
    Bot(const std::string& id) : OtherUsers(UserType::Bot, id) {}

    ~Bot() = default;
};

class Teacher : public OtherUsers {
public:
    Teacher(const std::string& id) : OtherUsers(UserType::Teacher, id) {}

    ~Teacher() = default;
};

#ifdef USE_QTJSON
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

inline auto user_type_to_qjson_string(UserType type) -> QString {
    switch (type) {
        case UserType::Admin: return "Admin";
        case UserType::Bot: return "Bot";
        case UserType::Teacher: return "Teacher";
        default: return "Unknown";
    }
}

inline auto user_type_from_qjson_string(const QString& str) -> UserType {
    if (str == "Admin") return UserType::Admin;
    else if (str == "Bot") return UserType::Bot;
    else if (str == "Teacher") return UserType::Teacher;
    throw std::invalid_argument("Unknown UserType string: " + str.toStdString());
}

inline auto other_users_to_qjson(const OtherUsers& user) -> QJsonObject {
    QJsonObject obj;
    obj["type"] = user_type_to_qjson_string(user.get_type());
    obj["id"] = QString::fromStdString(user.get_id());
    return obj;
}

inline auto other_users_from_qjson(const QJsonObject& obj) -> OtherUsers {
    UserType type = user_type_from_qjson_string(obj["type"].toString());
    std::string id = obj["id"].toString().toStdString();
    switch (type) {
        case UserType::Admin: return Admin(id);
        case UserType::Bot: return Bot(id);
        case UserType::Teacher: return Teacher(id);
        default: throw std::invalid_argument("Unknown UserType in QJsonObject");
    }
}

#endif
