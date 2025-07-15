
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
    QJsonArray get_students() const;
    void add_student(const QJsonObject& studentData);
    void update_student(const QJsonObject& studentData);
    void delete_student(long studentId);

private:
    void init_database();
    void load_students_from_db();
    void save_student_to_db(const Stu_withScore& student);
    void update_student_in_db(const Stu_withScore& student);
    void delete_student_from_db(long studentId);

    QWidget* m_parentWidget;
    std::vector<Stu_withScore> m_students;
    QSqlDatabase m_database;
};

