#pragma once

#include "struct/stu_with_score.h"


#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <vector>

class WebBridge : public QObject {
    Q_OBJECT

public:
    explicit WebBridge(QObject* parent = nullptr);

signals :
    void file_selected(const QString& filePath);

    void file_save_requested(const QString& filePath);

    void page_requested(const QString& pageUrl);

    void students_updated();

public
slots :
    void load_page(const QString& page);

    void open_file_dialog(const QString& title, const QString& filter);

    void save_file_dialog(const QString& title, const QString& filter);

    void log_message(const QString& message);

    void show_notification(const QString& title, const QString& message);

    void minimize_to_tray();

    QJsonObject get_app_info();

    QJsonArray get_students();

    void add_student(const QJsonObject& studentData);

    void update_student(const QJsonObject& studentData);

    void delete_student(long studentId);

    void save_students();

    void load_students();

private:
    void save_students_to_file(const QString& filePath);

    void load_students_from_file(const QString& filePath);

    QString get_backup_path() const;

    QWidget* m_parentWidget;
    std::vector<Stu_withScore> m_students;
};
