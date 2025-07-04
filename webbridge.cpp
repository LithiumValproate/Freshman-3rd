#define USE_QTJSON

#include "webbridge.h"
#include "struct/stu_with_score.h"

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

// 构造函数，初始化WebBridge对象，加载学生数据
WebBridge::WebBridge(QObject *parent)
    : QObject(parent)
{
    m_parentWidget = qobject_cast<QWidget*>(parent);
    log_message("WebBridge 初始化开始");
    QString backupPath = get_backup_path();
    log_message("数据文件路径: " + backupPath);
    load_students(); // 应用启动时加载学生数据
    log_message("WebBridge 初始化完成");
}

// 加载指定页面，发射信号通知前端
void WebBridge::load_page(const QString &page)
{
    emit page_requested(page);
}

// 打开文件选择对话框，选择学生数据文件并加载
void WebBridge::open_file_dialog(const QString &title, const QString &filter)
{
    QString filePath = QFileDialog::getOpenFileName(m_parentWidget, title, "", filter);
    if (!filePath.isEmpty()) {
        emit file_selected(filePath);
        load_students_from_file(filePath);
        emit students_updated(); // 通知前端数据已更新
    }
}

// 打开保存文件对话框，保存学生数据到指定文件
void WebBridge::save_file_dialog(const QString &title, const QString &filter)
{
    QString filePath = QFileDialog::getSaveFileName(m_parentWidget, title, "", filter);
    if (!filePath.isEmpty()) {
        emit file_save_requested(filePath);
        save_students_to_file(filePath);
        emit students_updated(); // 通知前端数据已更新（可选）
    }
}

// 日志输出函数，便于调试
void WebBridge::log_message(const QString &message)
{
    qDebug() << "[WEB LOG] " << message;
}

// 显示系统托盘通知
void WebBridge::show_notification(const QString &title, const QString &message)
{
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/icons/app_icon.png"));
    trayIcon->show();
    trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 3000);
}

// 最小化窗口到托盘
void WebBridge::minimize_to_tray()
{
    if (m_parentWidget) {
        m_parentWidget->hide();
    }
}

// 获取应用信息（名称、版本、Qt版本）
QJsonObject WebBridge::get_app_info()
{
    QJsonObject info;
    info["appName"] = "QtWebStudentSys";
    info["version"] = "1.0.0";
    info["qtVersion"] = qVersion();
    return info;
}

// 添加学生，参数为JSON对象
void WebBridge::add_student(const QJsonObject &studentData)
{
    log_message("开始添加学生，接收到的JSON数据:");
    log_message(QString(QJsonDocument(studentData).toJson(QJsonDocument::Compact)));

    try {
        // 验证必需字段
        if (!studentData.contains("id")) {
            log_message("错误: 学生数据缺少 'id' 字段");
            return;
        }
        if (!studentData.contains("name")) {
            log_message("错误: 学生数据缺少 'name' 字段");
            return;
        }

        log_message("正在转换JSON到学生对象...");
        Stu_withScore student = stu_with_score_from_qjson(studentData);
        log_message("JSON转换成功");

        log_message(QString("添加学生: ID=%1, Name=%2").arg(student.get_id()).arg(QString::fromStdString(student.get_name())));

        // 检查是否已存在相同ID的学生，存在则覆盖，否则添加
        auto it = std::find_if(m_students.begin(), m_students.end(),
                              [&student](const Stu_withScore& s) {
                                  return s.get_id() == student.get_id();
                              });
        if (it != m_students.end()) {
            log_message(QString("警告: ID为 %1 的学生已存在，将覆盖原数据").arg(student.get_id()));
            *it = student;
        } else {
            m_students.push_back(student);
        }

        log_message(QString("当前学生总数: %1").arg(m_students.size()));
        log_message("开始保存学生数据...");
        save_students(); // 自动保存
        log_message("学生添加完成");

        show_notification("成功", "学生 " + QString::fromStdString(student.get_name()) + " 已添加。");
        emit students_updated(); // 通知前端数据已更新
    } catch (const std::exception& e) {
        log_message(QString("添加学生失败: %1").arg(e.what()));
        show_notification("错误", QString("添加学生失败: %1").arg(e.what()));
    } catch (...) {
        log_message("添加学生失败 (未知错误)。");
        show_notification("错误", "添加学生失败 (未知错误)");
    }
}

// 获取所有学生信息，返回JSON数组
QJsonArray WebBridge::get_students()
{
    log_message(QString("get_students 被调用，当前内存中有 %1 个学生").arg(m_students.size()));

    QJsonArray studentsArray;
    for (const auto &student : m_students) {
        try {
            QJsonObject studentJson = stu_with_score_to_qjson(student);
            studentsArray.append(studentJson);
            log_message(QString("转换学生: ID=%1, Name=%2").arg(student.get_id()).arg(QString::fromStdString(student.get_name())));
        } catch (const std::exception& e) {
            log_message(QString("转换学生到JSON失败: %1").arg(e.what()));
        } catch (...) {
            log_message("转换学生到JSON失败 (未知错误)");
        }
    }

    log_message(QString("返回 %1 个学生的JSON数据").arg(studentsArray.size()));
    return studentsArray;
}

// 更新学生信息，参数为JSON对象
void WebBridge::update_student(const QJsonObject &studentData)
{
    if (!studentData.contains("id")) {
        log_message("更新失败: 学生数据缺少 'id' 字段。");
        return;
    }
    long id = studentData["id"].toVariant().toLongLong();

    // 查找对应ID的学生并更新
    auto it = std::find_if(m_students.begin(), m_students.end(),
                           [id](const Stu_withScore& s) {
                               return s.get_id() == id;
                           });

    if (it != m_students.end()) {
        try {
            *it = stu_with_score_from_qjson(studentData);
            log_message("Student " + QString::fromStdString(it->get_name()) + " updated.");
            show_notification("成功", "学生 " + QString::fromStdString(it->get_name()) + " 已更新。");
            save_students(); // 自动保存
            emit students_updated(); // 通知前端数据已更新
        } catch (const std::exception& e) {
            log_message(QString("更新学生失败: %1").arg(e.what()));
        } catch (...) {
            log_message("更新学生失败 (未知错误)。");
        }
    } else {
        log_message(QString("更新失败: 未找到ID为 %1 的学生。").arg(id));
    }
}

// 删除指定ID的学生
void WebBridge::delete_student(long studentId)
{
    // 使用remove_if删除匹配ID的学生
    auto it = std::remove_if(m_students.begin(), m_students.end(),
                             [studentId](const Stu_withScore& s) {
                                 return s.get_id() == studentId;
                             });

    if (it != m_students.end()) {
        m_students.erase(it, m_students.end());
        log_message(QString("Student with ID %1 deleted.").arg(studentId));
        show_notification("成功", QString("ID为 %1 的学生已删除。").arg(studentId));
        save_students(); // 自动保存
        emit students_updated(); // 通知前端数据已更新
    } else {
        log_message(QString("删除失败: 未找到ID为 %1 的学生。").arg(studentId));
    }
}

// 保存学生数据到默认备份路径
void WebBridge::save_students()
{
    save_students_to_file(get_backup_path());
}

// 从默认备份路径加载学生数据
void WebBridge::load_students()
{
    load_students_from_file(get_backup_path());
}

// 保存学生数据到指定文件
void WebBridge::save_students_to_file(const QString &filePath)
{
    try {
        // 确保目录存在
        QFileInfo fileInfo(filePath);
        QDir dir = fileInfo.absoluteDir();
        if (!dir.exists()) {
            if (!dir.mkpath(".")) {
                log_message("无法创建目录: " + dir.absolutePath());
                return;
            }
        }

        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            log_message("无法打开文件进行写入: " + file.errorString());
            return;
        }

        QJsonArray studentsArray = get_students();
        QJsonDocument doc(studentsArray);
        QByteArray jsonData = doc.toJson();

        qint64 bytesWritten = file.write(jsonData);
        if (bytesWritten == -1) {
            log_message("写入文件失败: " + file.errorString());
            return;
        }

        file.close();
        log_message(QString("学生数据已保存到: %1 (写入 %2 字节)").arg(filePath).arg(bytesWritten));

        // 验证文件是否真的保存成功
        if (QFile::exists(filePath)) {
            QFileInfo info(filePath);
            log_message(QString("文件验证成功，大小: %1 字节").arg(info.size()));
        } else {
            log_message("警告: 文件保存后不存在");
        }
    } catch (const std::exception& e) {
        log_message(QString("保存学生数据时发生异常: %1").arg(e.what()));
    } catch (...) {
        log_message("保存学生数据时发生未知异常");
    }
}

// 从指定文件加载学生数据
void WebBridge::load_students_from_file(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists()) {
        log_message("学生数据文件不存在: " + filePath);
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        log_message("无法打开文件进行读取: " + file.errorString());
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isArray()) {
        m_students.clear();
        QJsonArray studentsArray = doc.array();
        for (const auto& item : studentsArray) {
            if (item.isObject()) {
                try {
                    // 直接添加到内存，不调用add_student避免重复保存
                    Stu_withScore student = stu_with_score_from_qjson(item.toObject());
                    m_students.push_back(student);
                } catch (const std::exception& e) {
                    log_message(QString("加载学生数据失败: %1").arg(e.what()));
                } catch (...) {
                    log_message("加载学生数据失败 (未知错误)。");
                }
            }
        }
        log_message(QString("成功从 %1 加载了 %2 个学生。").arg(filePath).arg(m_students.size()));
    } else {
        log_message("文件格式错误: 不是有效的JSON数组。");
    }
    file.close();
    emit students_updated(); // 通知前端数据已更新
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
