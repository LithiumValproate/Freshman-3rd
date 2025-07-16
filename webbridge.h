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
    void set_parent_widget(QWidget* parentWidget);

signals:
    // [核心] 发射给 MainWindow 的信号，请求UI操作
    void open_file_dialog_requested(const QString& title, const QString& filter);
    void save_file_dialog_requested(const QString& title, const QString& filter);

    // 其他信号
    void page_requested(const QString& pageUrl);
    void students_updated();

    void minimize_to_tray_requested();

public slots:
    // [核心] JS调用的入口函数，它们只发射信号
    void request_import_dialog(const QString& title, const QString& filter);
    void request_export_dialog(const QString& title, const QString& filter);

    // [核心] 由 MainWindow 在获取路径后调用的函数，用于处理数据
    void process_selected_file(const QString& filePath);
    void process_save_file_path(const QString& filePath);

    // 其他暴露给JS的辅助函数
    void load_page(const QString& page);
    void show_notification(const QString& title, const QString& message);
    void minimize_to_tray();
    QJsonObject get_app_info();
    void add_student_from_qjson(const QJsonObject& studentData);

    // 静态日志函数，可以在任何地方使用
    static void log_message(const QString& message);

    // JSON & DB 方法
    QJsonArray get_students_from_db() const;
    QJsonObject get_student_by_id_from_db(long studentId) const;
    QString get_backup_path() const;
    void add_student_to_db(const QJsonObject& studentData);
    void update_student_in_db(const QJsonObject& studentData);
    void delete_student_from_db(long studentId);
    QJsonObject authenticate_user(const QString& role, const QString& username, const QString& password);

private:
    // 私有数据处理函数
    void load_students_from_file(const QString& filePath);
    void save_students_to_file(const QString& filePath);

    // 私有数据库函数
    void init_database();
    QJsonArray get_students_from_qjson() const;
    void update_student_in_qjson(const QJsonObject& studentData);
    void delete_student_from_qjson(long studentId);
    QJsonObject get_student_by_id_from_qjson(long studentId) const;
    void load_students_from_db();
    void save_student_to_db(const Stu_withScore& student);
    void update_student_in_db(const Stu_withScore& student);
    void delete_student_from_db_helper(long studentId);

    // 数据成员
    std::vector<Stu_withScore> m_students;
    QSqlDatabase m_database;
};