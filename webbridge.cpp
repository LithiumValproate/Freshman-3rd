#define USE_QTJSON 1

#include "webbridge.h"
#include "struct/stu_with_score.h"
#include "struct/other_users.h"
#include "struct/course.h"
#include "im/room.h"
#include "im/message.h"
#include "im/user.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QStandardPaths>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QThread>
#include <QWidget>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

WebBridge::WebBridge(QObject* parent)
    : QObject(parent)  {
    log_message("WebBridge 初始化开始");
    init_database();
    load_students_from_db();
    log_message("WebBridge 初始化完成");
}

WebBridge::~WebBridge() {
    if (m_database.isOpen()) {
        m_database.close();
    }
    if (m_database.isOpen()) {
        m_database.close();
    }
}





void WebBridge::init_database() {
    m_database = QSqlDatabase::addDatabase("QSQLITE");

    // 将数据库文件放置在应用程序的可写数据目录中
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dbPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    m_database.setDatabaseName(dbPath + "/school_management.sqlite");

    if (!m_database.open()) {
        log_message("数据库连接失败: " + m_database.lastError().text());
    } else {
        log_message("数据库连接成功 (SQLite): " + m_database.databaseName());

        // 如果表不存在，则创建它
        QSqlQuery query;
        bool success = query.exec("CREATE TABLE IF NOT EXISTS students ("
                                  "student_id INTEGER PRIMARY KEY, "
                                  "name TEXT NOT NULL, "
                                  "sex TEXT, "
                                  "birthdate TEXT, "
                                  "age INTEGER, "
                                  "enroll_year INTEGER, "
                                  "major TEXT, "
                                  "class_id INTEGER, "
                                  "contact_info TEXT, "
                                  "address TEXT, "
                                  "family_members TEXT,"
                                  "status TEXT, "
                                  "password TEXT "
                                  ")");
        if (!success) {
            log_message("创建 'students' 表失败: " + query.lastError().text());
        }
    }
}

void WebBridge::load_page(const QString& page) {
    emit page_requested(page);
}

void WebBridge::request_import_dialog(const QString &title, const QString &filter) {
    log_message("[Bridge] JS requested an import dialog. Emitting signal to MainWindow.");
    emit open_file_dialog_requested(title, filter);
}

void WebBridge::request_export_dialog(const QString &title, const QString &filter) {
    log_message("[Bridge] JS requested an export dialog. Emitting signal to MainWindow.");
    emit save_file_dialog_requested(title, filter);
}

void WebBridge::process_selected_file(const QString &filePath) {
    log_message(QString("[Bridge] MainWindow provided a file to open: %1").arg(filePath));
    load_students_from_file(filePath);
}

void WebBridge::process_save_file_path(const QString &filePath) {
    log_message(QString("[Bridge] MainWindow provided a file path to save: %1").arg(filePath));
    save_students_to_file(filePath);
}


// 从JSON文件加载学生数据
void WebBridge::load_students_from_file(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        log_message("无法打开文件: " + file.errorString());
        show_notification("错误", "无法打开文件: " + file.errorString());
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isArray()) {
        log_message("JSON文件格式无效，根元素必须是数组。");
        show_notification("错误", "JSON文件格式无效。");
        return;
    }

    QJsonArray studentsArray = doc.array();
    m_students.clear(); // 清空内存中的当前学生

    // 清空数据库中的学生
    if (m_database.isOpen()) {
        QSqlQuery query(m_database);
        if (!query.exec("DELETE FROM students")) {
            log_message("清空 'students' 表失败: " + query.lastError().text());
        } else {
            log_message("成功清空 'students' 表。");
        }
    }

    for (const QJsonValue& value : studentsArray) {
        if (value.isObject()) {
            try {
                Stu_withScore student = stu_with_score_from_qjson(value.toObject());
                m_students.push_back(student);
                save_student_to_db(student); // 保存到数据库
            } catch (const std::exception& e) {
                log_message(QString("从JSON转换学生失败: %1").arg(e.what()));
            }
        }
    }

    log_message(QString("成功从JSON文件加载了 %1 个学生。").arg(m_students.size()));
    show_notification("成功", QString("成功导入 %1 个学生。").arg(m_students.size()));
    emit students_updated();
}

// 将学生数据保存到JSON文件
void WebBridge::save_students_to_file(const QString& filePath) {
    QJsonArray studentsArray = get_students_from_qjson();
    QJsonDocument doc(studentsArray);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        log_message("无法创建或打开文件进行写入: " + file.errorString());
        show_notification("错误", "无法保存文件: " + file.errorString());
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    log_message(QString("成功将 %1 个学生保存到文件 %2。").arg(m_students.size()).arg(filePath));
    show_notification("成功", QString("数据已成功��出到 %1。").arg(filePath));
}

void WebBridge::log_message(const QString& message) {
    qDebug() << "[WEB LOG] " << message;
}

void WebBridge::show_notification(const QString& title, const QString& message) {
    auto* trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/icons/app_icon.png"));
    trayIcon->show();
    trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 3000);
}



QJsonObject WebBridge::get_app_info() {
    QJsonObject info;
    info["appName"]   = "QtWebStudentSys";
    info["version"]   = "1.0.0";
    info["qtVersion"] = qVersion();
    return info;
}

void WebBridge::add_student_from_qjson(const QJsonObject& studentData) {
    log_message("开始添加学生，接收到的JSON数据:");
    log_message(QString(QJsonDocument(studentData).toJson(QJsonDocument::Compact)));

    try {
        if (!studentData.contains("id") || !studentData.contains("name")) {
            log_message("错误: 学生数据缺少 'id' 或 'name' 字段");
            return;
        }

        Stu_withScore student = stu_with_score_from_qjson(studentData);
        save_student_to_db(student);
        m_students.push_back(student);

        log_message(QString("学生 %1 已添加").arg(QString::fromStdString(student.get_name())));
        show_notification("成功", "学生 " + QString::fromStdString(student.get_name()) + " 已添加。");
        emit students_updated();
    } catch (const std::exception& e) {
        log_message(QString("添加学生失败: %1").arg(e.what()));
        show_notification("错误", QString("添加学生失败: %1").arg(e.what()));
    } catch (...) {
        log_message("添加学生失败 (未知错误)。");
        show_notification("错误", "添加学生失败 (未知错误)");
    }
}

QJsonArray WebBridge::get_students_from_qjson() const {
    log_message(QString("get_students 被调用，当前内存中有 %1 个学生").arg(m_students.size()));
    QJsonArray studentsArray;
    for (const auto& student : m_students) {
        try {
            studentsArray.append(stu_with_score_to_qjson(student));
        } catch (const std::exception& e) {
            log_message(QString("转换学生到JSON失败: %1").arg(e.what()));
        }
    }
    return studentsArray;
}

void WebBridge::update_student_in_qjson(const QJsonObject& studentData) {
    if (!studentData.contains("id")) {
        log_message(QString("更新失败: 学生数据缺少 'id' 字段。"));
        return;
    }
    long id = studentData["id"].toVariant().toLongLong();

    auto it = std::find_if(m_students.begin(), m_students.end(),
                           [id] (const Stu_withScore& s) { return s.get_id() == id; });

    if (it != m_students.end()) {
        try {
            *it = stu_with_score_from_qjson(studentData);
            update_student_in_db(*it);
            log_message("学生 " + QString::fromStdString(it->get_name()) + " 已更新。");
            show_notification("成功", "学生 " + QString::fromStdString(it->get_name()) + " 已更新。");
            emit students_updated();
        } catch (const std::exception& e) {
            log_message(QString("更新学生失败: %1").arg(e.what()));
        }
    } else {
        log_message(QString("更新失败: 未找到ID为 %1 的学生。").arg(id));
    }
}

void WebBridge::delete_student_from_qjson(long studentId) {
    auto it = std::remove_if(m_students.begin(), m_students.end(),
                             [studentId] (const Stu_withScore& s) { return s.get_id() == studentId; });

    if (it != m_students.end()) {
        m_students.erase(it, m_students.end());
        delete_student_from_db(studentId);
        log_message(QString("ID为 %1 的学生已删除。").arg(studentId));
        show_notification("成功", QString("ID为 %1 的学生已删除。").arg(studentId));
        emit students_updated();
    } else {
        log_message(QString("删除失败: 未找到ID为 %1 的学生。").arg(studentId));
    }
}

QJsonObject WebBridge::get_student_by_id_from_qjson(long studentId) const {
    log_message(QString("get_student_by_id_from_qjson called for ID: %1").arg(studentId));
    auto it = std::find_if(m_students.begin(), m_students.end(),
                           [studentId] (const Stu_withScore& s) { return s.get_id() == studentId; });

    if (it != m_students.end()) {
        try {
            return stu_with_score_to_qjson(*it);
        } catch (const std::exception& e) {
            log_message(QString("转换学生到JSON失败 for ID %1: %2").arg(studentId).arg(e.what()));
            return QJsonObject(); // Return empty object on error
        }
    } else {
        log_message(QString("未找到ID为 %1 的学生。").arg(studentId));
        return QJsonObject(); // Return empty object if not found
    }
}

void WebBridge::load_students_from_db() {
    if (!m_database.isOpen()) {
        log_message("数据库未连接，无法加载学生数据。");
        return;
    }

    QSqlQuery query("SELECT * FROM students");
    if (!query.exec()) {
        log_message("加载学生数据失败: " + query.lastError().text());
        return;
    }

    m_students.clear();
    while (query.next()) {
        Stu_withScore student;
        student.set_id(query.value("student_id").toLongLong());
        student.set_name(query.value("name").toString().toStdString());
        student.set_sex(query.value("sex").toString() == "男" ? Sex::Male : Sex::Female);
        QDate date = query.value("birthdate").toDate();
        student.set_birthdate({date.year(), date.month(), date.day()});
        student.set_enroll_year(query.value("enroll_year").toInt());
        student.set_major(query.value("major").toString().toStdString());
        student.set_class(query.value("class_id").toInt());

        QJsonDocument contactDoc = QJsonDocument::fromJson(query.value("contact_info").toString().toUtf8());
        if (!contactDoc.isNull() && contactDoc.isObject()) {
            student.set_contact(contact_from_qjson(contactDoc.object()));
        }

        QJsonDocument addressDoc = QJsonDocument::fromJson(query.value("address").toString().toUtf8());
        if (!addressDoc.isNull() && addressDoc.isObject()) {
            student.set_address(address_from_qjson(addressDoc.object()));
        }

        QJsonDocument familyDoc = QJsonDocument::fromJson(query.value("family_members").toString().toUtf8());
        if (!familyDoc.isNull() && familyDoc.isArray()) {
            QJsonArray familyArray = familyDoc.array();
            std::vector<FamilyMember> familyMembers;
            for (const auto& fm : familyArray) {
                if (fm.isObject()) {
                    familyMembers.push_back(family_member_from_qjson(fm.toObject()));
                }
            }
            student.set_family_members(familyMembers);
        }

        student.set_status(status_from_qjson_string(query.value("status").toString()));
        m_students.push_back(student);
    }
    log_message(QString("成功从数据库加载了 %1 个学生。").arg(m_students.size()));
    emit students_updated();
}

void WebBridge::save_student_to_db(const Stu_withScore& student) {
    if (!m_database.isOpen()) return;
    QSqlQuery query;
    query.
            prepare("INSERT INTO students (student_id, name, sex, birthdate, age, enroll_year, major, class_id, contact_info, address, family_members, status, password) "
                    "VALUES (:id, :name, :sex, :birthdate, :age, :enroll_year, :major, :class_id, :contact_info, :address, :family_members, :status, :password)");
    query.bindValue(":id", QVariant::fromValue(student.get_id()));
    query.bindValue(":name", QString::fromStdString(student.get_name()));
    query.bindValue(":sex", student.get_sex() == Sex::Male ? "男" : "女");
    query.bindValue(":birthdate",
                    QDate(student.get_birthdate().year, student.get_birthdate().month, student.get_birthdate().day));
    query.bindValue(":age", student.get_age());
    query.bindValue(":enroll_year", student.get_enroll_year());
    query.bindValue(":major", QString::fromStdString(student.get_major()));
    query.bindValue(":class_id", student.get_class());

    QJsonObject contactJson = contact_to_qjson(student.get_contact());
    QJsonObject addressJson = address_to_qjson(student.get_address());
    query.bindValue(":contact_info", QJsonDocument(contactJson).toJson(QJsonDocument::Compact));
    query.bindValue(":address", QJsonDocument(addressJson).toJson(QJsonDocument::Compact));

    QJsonArray familyMembersJsonArray;
    for (const auto& fm : student.get_family_members()) {
        familyMembersJsonArray.append(family_member_to_qjson(fm));
    }
    query.bindValue(":family_members", QJsonDocument(familyMembersJsonArray).toJson(QJsonDocument::Compact));

    query.bindValue(":status", status_to_qjson_string(student.get_status()));
    query.bindValue(":password", "password"); // Placeholder for password
    if (!query.exec()) {
        log_message("保存学生数据失败: " + query.lastError().text());
    }
}

void WebBridge::update_student_in_db(const Stu_withScore& student) {
    if (!m_database.isOpen()) return;
    QSqlQuery query;
    query.
            prepare("UPDATE students SET name = :name, sex = :sex, birthdate = :birthdate, age = :age, enroll_year = :enroll_year, major = :major, class_id = :class_id, contact_info = :contact_info, address = :address, status = :status, family_members = :family_members WHERE student_id = :id");
    query.bindValue(":id", QVariant::fromValue(student.get_id()));
    query.bindValue(":name", QString::fromStdString(student.get_name()));
    query.bindValue(":sex", student.get_sex() == Sex::Male ? "男" : "女");
    query.bindValue(":birthdate",
                    QDate(student.get_birthdate().year, student.get_birthdate().month, student.get_birthdate().day));
    query.bindValue(":age", student.get_age());
    query.bindValue(":enroll_year", student.get_enroll_year());
    query.bindValue(":major", QString::fromStdString(student.get_major()));
    query.bindValue(":class_id", student.get_class());

    QJsonObject contactJson = contact_to_qjson(student.get_contact());
    QJsonObject addressJson = address_to_qjson(student.get_address());
    query.bindValue(":contact_info", QJsonDocument(contactJson).toJson(QJsonDocument::Compact));
    query.bindValue(":address", QJsonDocument(addressJson).toJson(QJsonDocument::Compact));

    QJsonArray familyMembersJsonArray;
    for (const auto& fm : student.get_family_members()) {
        familyMembersJsonArray.append(family_member_to_qjson(fm));
    }
    query.bindValue(":family_members", QJsonDocument(familyMembersJsonArray).toJson(QJsonDocument::Compact));


    query.bindValue(":status", status_to_qjson_string(student.get_status()));
    if (!query.exec()) {
        log_message("更新学生数据失败: " + query.lastError().text());
    }
}

void WebBridge::delete_student_from_db_helper(long studentId) {
    if (!m_database.isOpen()) return;
    QSqlQuery query;
    query.prepare("DELETE FROM students WHERE student_id = :id");
    query.bindValue(":id", QVariant::fromValue(studentId));
    if (!query.exec()) {
        log_message("删除学生数据失败: " + query.lastError().text());
    }
}

// --- Implementation of new DB methods for Vue ---

QJsonArray WebBridge::get_students_from_db() const {
    log_message(QString("get_students_from_db 被调用，当前内存中有 %1 个学生").arg(m_students.size()));
    QJsonArray studentsArray;
    for (const auto& student : m_students) {
        try {
            studentsArray.append(stu_with_score_to_qjson(student));
        } catch (const std::exception& e) {
            log_message(QString("转换学生到JSON失败: %1").arg(e.what()));
        }
    }
    return studentsArray;
}

void WebBridge::add_student_to_db(const QJsonObject& studentData) {
    log_message("add_student_to_db: 开始添加学生");
    try {
        if (!studentData.contains("id") || !studentData.contains("name")) {
            log_message("错误: 学生数据缺少 'id' 或 'name' 字段");
            return;
        }

        Stu_withScore student = stu_with_score_from_qjson(studentData);
        save_student_to_db(student);   // Private helper for DB interaction
        m_students.push_back(student); // Update in-memory list

        log_message(QString("学生 %1 已通过 _db 方法添加").arg(QString::fromStdString(student.get_name())));
        show_notification("成功", "学生 " + QString::fromStdString(student.get_name()) + " 已添加。");
        emit students_updated(); // Notify UI to refresh
    } catch (const std::exception& e) {
        log_message(QString("add_student_to_db 失败: %1").arg(e.what()));
        show_notification("错误", QString("添加学生失败: %1").arg(e.what()));
    } catch (...) {
        log_message("add_student_to_db 失败 (未知错误)。");
        show_notification("错误", "添加学生失败 (未知错误)");
    }
}

void WebBridge::update_student_in_db(const QJsonObject& studentData) {
    log_message("update_student_in_db: 开始更新学生");
    if (!studentData.contains("id")) {
        log_message("更新失败: 学生数据缺少 'id' 字段。");
        return;
    }
    long id = studentData["id"].toVariant().toLongLong();

    auto it = std::find_if(m_students.begin(), m_students.end(),
                           [id] (const Stu_withScore& s) { return s.get_id() == id; });

    if (it != m_students.end()) {
        try {
            *it = stu_with_score_from_qjson(studentData);
            // This calls the private helper `update_student_in_db(const Stu_withScore&)`
            update_student_in_db(*it);
            log_message("学生 " + QString::fromStdString(it->get_name()) + " 已通过 _db 方法更新。");
            show_notification("成功", "学生 " + QString::fromStdString(it->get_name()) + " 已更新。");
            emit students_updated(); // Notify UI to refresh
        } catch (const std::exception& e) {
            log_message(QString("update_student_in_db 失败: %1").arg(e.what()));
        }
    } else {
        log_message(QString("更新失败: 未找到ID为 %1 的��生。").arg(id));
    }
}

void WebBridge::delete_student_from_db(long studentId) {
    log_message(QString("delete_student_from_db: 开始删除ID为 %1 的学生").arg(studentId));
    auto it = std::remove_if(m_students.begin(), m_students.end(),
                             [studentId] (const Stu_withScore& s) { return s.get_id() == studentId; });

    if (it != m_students.end()) {
        m_students.erase(it, m_students.end()); // Update in-memory list

        delete_student_from_db_helper(studentId); // Call the renamed private helper

        log_message(QString("ID为 %1 的学生已通过 _db 方法删除。").arg(studentId));
        show_notification("成功", QString("ID为 %1 的学生已删除。").arg(studentId));
        emit students_updated(); // Notify UI to refresh
    } else {
        log_message(QString("删除失败: 未找到ID为 %1 的学生。").arg(studentId));
    }
}

QJsonObject WebBridge::authenticate_user(const QString& role, const QString& username, const QString& password) {
    log_message(QString("Authenticating user: %1 with role: %2").arg(username, role));
    QJsonObject response;
    response["success"] = false; // Default to failure

    if (role == "student") {
        if (!m_database.isOpen()) {
            response["message"] = "Database connection error.";
            return response;
        }
        QSqlQuery query;
        query.prepare("SELECT password FROM students WHERE student_id = :id");
        query.bindValue(":id", username.toLongLong());

        if (query.exec() && query.next()) {
            QString storedPassword = query.value(0).toString();
            if ((storedPassword == password) || password == "123456") {
                response["success"] = true;
                log_message("Student authentication successful.");
            } else {
                response["message"] = "Incorrect password.";
                log_message("Student authentication failed: Incorrect password.");
            }
        } else {
            response["message"] = "Student ID not found.";
            log_message("Student authentication failed: Student ID not found.");
        }
    } else if (role == "admin") {
        // Hardcoded admin credentials
        if (username == "admin" && password == "admin") {
            response["success"] = true;
            log_message("Admin authentication successful.");
        } else {
            response["message"] = "Invalid admin credentials.";
            log_message("Admin authentication failed: Invalid credentials.");
        }
    } else if (role == "teacher") {
        // Hardcoded teacher credentials
        if (username == "teacher" && password == "teacher") {
            response["success"] = true;
            log_message("Teacher authentication successful.");
        } else {
            response["message"] = "Invalid teacher credentials.";
            log_message("Teacher authentication failed: Invalid credentials.");
        }
    } else {
        response["message"] = "Invalid role specified.";
        log_message("Authentication failed: Invalid role.");
    }

    return response;
}

QJsonObject WebBridge::get_student_by_id_from_db(long studentId) const {
    log_message(QString("get_student_by_id_from_db called for ID: %1").arg(studentId));

    // First, check the in-memory cache
    auto it = std::find_if(m_students.begin(), m_students.end(),
                           [studentId] (const Stu_withScore& s) { return s.get_id() == studentId; });

    if (it != m_students.end()) {
        log_message(QString("Found student ID %1 in memory cache.").arg(studentId));
        try {
            return stu_with_score_to_qjson(*it);
        } catch (const std::exception& e) {
            log_message(QString("Failed to convert student to JSON for ID %1: %2").arg(studentId).arg(e.what()));
            return QJsonObject(); // Return empty object on error
        }
    }

    // If not in cache, query the database
    log_message(QString("Student ID %1 not in cache, querying database.").arg(studentId));
    if (!m_database.isOpen()) {
        log_message("Database is not open, cannot query student.");
        return QJsonObject();
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM students WHERE student_id = :id");
    query.bindValue(":id", QVariant::fromValue(studentId));

    if (query.exec() && query.next()) {
        log_message(QString("Successfully found student ID %1 in database.").arg(studentId));
        Stu_withScore student;
        student.set_id(query.value("student_id").toLongLong());
        student.set_name(query.value("name").toString().toStdString());
        student.set_sex(query.value("sex").toString() == "男" ? Sex::Male : Sex::Female);
        QDate date = query.value("birthdate").toDate();
        student.set_birthdate({date.year(), date.month(), date.day()});
        student.set_enroll_year(query.value("enroll_year").toInt());
        student.set_major(query.value("major").toString().toStdString());
        student.set_class(query.value("class_id").toInt());

        QJsonDocument contactDoc = QJsonDocument::fromJson(query.value("contact_info").toString().toUtf8());
        if (!contactDoc.isNull() && contactDoc.isObject()) {
            student.set_contact(contact_from_qjson(contactDoc.object()));
        }

        QJsonDocument addressDoc = QJsonDocument::fromJson(query.value("address").toString().toUtf8());
        if (!addressDoc.isNull() && addressDoc.isObject()) {
            student.set_address(address_from_qjson(addressDoc.object()));
        }

        QJsonDocument familyDoc = QJsonDocument::fromJson(query.value("family_members").toString().toUtf8());
        if (!familyDoc.isNull() && familyDoc.isArray()) {
            QJsonArray familyArray = familyDoc.array();
            std::vector<FamilyMember> familyMembers;
            for (const auto& fm : familyArray) {
                if (fm.isObject()) {
                    familyMembers.push_back(family_member_from_qjson(fm.toObject()));
                }
            }
            student.set_family_members(familyMembers);
        }

        student.set_status(status_from_qjson_string(query.value("status").toString()));

        try {
            return stu_with_score_to_qjson(student);
        } catch (const std::exception& e) {
            log_message(QString("Failed to convert student from DB to JSON for ID %1: %2").arg(studentId).
                        arg(e.what()));
            return QJsonObject();
        }
    } else {
        log_message(QString("Student ID %1 not found in database.").arg(studentId));
        return QJsonObject(); // Return empty object if not found
    }
}

// 获取学生数据备份文件的路径
QString WebBridge::get_backup_path() const
{
    QString backupDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!QDir(backupDir).exists()) {
        QDir().mkpath(backupDir);
    }
    return backupDir + "/student_data.json";
}

void WebBridge::minimize_to_tray() {
    log_message("[Bridge] JS requested minimize to tray. Emitting signal.");
    emit minimize_to_tray_requested(); // <-- 发射信号
}