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
#include <QWidget>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

WebBridge::WebBridge(QObject *parent)
    : QObject(parent)
{
    m_parentWidget = qobject_cast<QWidget*>(parent);
    log_message("WebBridge 初始化开始");
    init_database();
    load_students_from_db();
    log_message("WebBridge 初始化完成");
}

WebBridge::~WebBridge()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

void WebBridge::init_database()
{
    m_database = QSqlDatabase::addDatabase("QMYSQL");
    m_database.setHostName("localhost");
    m_database.setDatabaseName("school_management");
    m_database.setUserName("kasugano");
    const char* password = std::getenv("DB_PASSWORD");
    m_database.setPassword(password ? password : "");

    if (!m_database.open()) {
        log_message("数据库连接失败: " + m_database.lastError().text());
    } else {
        log_message("数据库连接成功");
    }
}

void WebBridge::load_page(const QString &page)
{
    emit page_requested(page);
}

void WebBridge::open_file_dialog(const QString &title, const QString &filter)
{
    QString filePath = QFileDialog::getOpenFileName(m_parentWidget, title, "", filter);
    if (!filePath.isEmpty()) {
        emit file_selected(filePath);
    }
}

void WebBridge::save_file_dialog(const QString &title, const QString &filter)
{
    QString filePath = QFileDialog::getSaveFileName(m_parentWidget, title, "data", filter);
    if (!filePath.isEmpty()) {
        emit file_save_requested(filePath);
    }
}

void WebBridge::log_message(const QString &message)
{
    qDebug() << "[WEB LOG] " << message;
}

void WebBridge::show_notification(const QString &title, const QString &message)
{
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/icons/app_icon.png"));
    trayIcon->show();
    trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 3000);
}

void WebBridge::minimize_to_tray() const {
    if (m_parentWidget) {
        m_parentWidget->hide();
    }
}

QJsonObject WebBridge::get_app_info()
{
    QJsonObject info;
    info["appName"] = "QtWebStudentSys";
    info["version"] = "1.0.0";
    info["qtVersion"] = qVersion();
    return info;
}

void WebBridge::add_student_from_qjson(const QJsonObject &studentData)
{
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
    for (const auto &student : m_students) {
        try {
            studentsArray.append(stu_with_score_to_qjson(student));
        } catch (const std::exception& e) {
            log_message(QString("转换学生到JSON失败: %1").arg(e.what()));
        }
    }
    return studentsArray;
}

void WebBridge::update_student_in_qjson(const QJsonObject &studentData)
{
    if (!studentData.contains("id")) {
        log_message("更新失败: 学生数据缺少 'id' 字段。");
        return;
    }
    long id = studentData["id"].toVariant().toLongLong();

    auto it = std::find_if(m_students.begin(), m_students.end(),
                           [id](const Stu_withScore& s) { return s.get_id() == id; });

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

void WebBridge::delete_student_from_qjson(long studentId)
{
    auto it = std::remove_if(m_students.begin(), m_students.end(),
                             [studentId](const Stu_withScore& s) { return s.get_id() == studentId; });

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

void WebBridge::load_students_from_db()
{
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
        // student.set_contact(contact_from_qjson(query.value("contact_info").toString()));
        // student.set_address(address_from_qjson(query.value("address").toString()));
        student.set_status(status_from_qjson_string(query.value("status").toString()));
        m_students.push_back(student);
    }
    log_message(QString("成功从数据库加载了 %1 个学生。").arg(m_students.size()));
    emit students_updated();
}

void WebBridge::save_student_to_db(const Stu_withScore& student)
{
    if (!m_database.isOpen()) return;
    QSqlQuery query;
    query.prepare("INSERT INTO students (student_id, name, sex, birthdate, age, enroll_year, major, class_id, contact_info, address, status, password) "
                  "VALUES (:id, :name, :sex, :birthdate, :age, :enroll_year, :major, :class_id, :contact_info, :address, :status, :password)");
    query.bindValue(":id", QVariant::fromValue(student.get_id()));
    query.bindValue(":name", QString::fromStdString(student.get_name()));
    query.bindValue(":sex", student.get_sex() == Sex::Male ? "男" : "女");
    query.bindValue(":birthdate", QDate(student.get_birthdate().year, student.get_birthdate().month, student.get_birthdate().day));
    query.bindValue(":age", student.get_age());
    query.bindValue(":enroll_year", student.get_enroll_year());
    query.bindValue(":major", QString::fromStdString(student.get_major()));
    query.bindValue(":class_id", student.get_class());
    query.bindValue(":contact_info", QString::fromStdString(student.get_contact().phone));
    query.bindValue(":address", QString::fromStdString(student.get_address().province));
    query.bindValue(":status", status_to_qjson_string(student.get_status()));
    query.bindValue(":password", "password"); // Placeholder for password
    if (!query.exec()) {
        log_message("保存学生数据失败: " + query.lastError().text());
    }
}

void WebBridge::update_student_in_db(const Stu_withScore& student)
{
    if (!m_database.isOpen()) return;
    QSqlQuery query;
    query.prepare("UPDATE students SET name = :name, sex = :sex, birthdate = :birthdate, age = :age, enroll_year = :enroll_year, major = :major, class_id = :class_id, contact_info = :contact_info, address = :address, status = :status WHERE student_id = :id");
    query.bindValue(":id", QVariant::fromValue(student.get_id()));
    query.bindValue(":name", QString::fromStdString(student.get_name()));
    query.bindValue(":sex", student.get_sex() == Sex::Male ? "男" : "女");
    query.bindValue(":birthdate", QDate(student.get_birthdate().year, student.get_birthdate().month, student.get_birthdate().day));
    query.bindValue(":age", student.get_age());
    query.bindValue(":enroll_year", student.get_enroll_year());
    query.bindValue(":major", QString::fromStdString(student.get_major()));
    query.bindValue(":class_id", student.get_class());
    query.bindValue(":contact_info", QString::fromStdString(student.get_contact().phone));
    query.bindValue(":address", QString::fromStdString(student.get_address().province));
    query.bindValue(":status", status_to_qjson_string(student.get_status()));
    if (!query.exec()) {
        log_message("更新学生数据失败: " + query.lastError().text());
    }
}

void WebBridge::delete_student_from_db(long studentId)
{
    if (!m_database.isOpen()) return;
    QSqlQuery query;
    query.prepare("DELETE FROM students WHERE student_id = :id");
    query.bindValue(":id", QVariant::fromValue(studentId));
    if (!query.exec()) {
        log_message("删除学生数据失败: " + query.lastError().text());
    }
}
