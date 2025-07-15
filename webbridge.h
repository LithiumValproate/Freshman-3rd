#pragma once

#include "struct/stu_with_score.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <vector>
#include <QtSql/QSqlDatabase>

class WebBridge : public QObject {
    Q_OBJECT

public:
    explicit WebBridge(QObject* parent = nullptr);

    ~WebBridge();

signals:
    void file_selected(const QString& filePath);
    void file_save_requested(const QString& filePath);
    void page_requested(const QString& pageUrl);
    void students_updated();

public slots:
    void load_page(const QString& page);
    void open_file_dialog(const QString& title, const QString& filter);
    void save_file_dialog(const QString& title, const QString& filter);
    static void log_message(const QString& message);
    void show_notification(const QString& title, const QString& message);
    void minimize_to_tray() const;
    QJsonObject get_app_info();

    // JSON methods
    QJsonArray get_students_from_qjson() const;
    QJsonObject get_student_by_id_from_qjson(long studentId) const;
    void add_student_from_qjson(const QJsonObject& studentData);
    void update_student_in_qjson(const QJsonObject& studentData);
    void delete_student_from_qjson(long studentId);

    // Database-specific methods for Vue
    QJsonArray get_students_from_db() const;
    QJsonObject get_student_by_id_from_db(long studentId) const;
    void add_student_to_db(const QJsonObject& studentData);
    void update_student_in_db(const QJsonObject& studentData);
    void delete_student_from_db(long studentId);
    QJsonObject authenticate_user(const QString& role, const QString& username, const QString& password);

private:
    // Private helper functions for database interaction
    void init_database();
    void load_students_from_db();
    void save_student_to_db(const Stu_withScore& student);
    void update_student_in_db(const Stu_withScore& student);
    void delete_student_from_db_helper(long studentId);

    QWidget* m_parentWidget;
    std::vector<Stu_withScore> m_students;
    QSqlDatabase m_database;
};
