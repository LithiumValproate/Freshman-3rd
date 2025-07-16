#include "mainwindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QStatusBar>
#include <QUrl>
#include <QVBoxLayout>
#include <QWebChannel>
#include <QWebEngineSettings>
#include <QWebEngineView>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
      , m_webView(nullptr)
      , m_webChannel(nullptr)
      , m_webBridge(nullptr) {
    setup_ui();
    setup_menu_bar();
    setup_status_bar();
    setup_web_view();
    connect(m_webBridge, &WebBridge::open_file_dialog_requested, this, &MainWindow::show_open_dialog);
    connect(m_webBridge, &WebBridge::save_file_dialog_requested, this, &MainWindow::show_save_dialog);

    connect(m_webBridge, &WebBridge::minimize_to_tray_requested, this, &MainWindow::handle_minimize_to_tray);
    setWindowTitle("Qt Web学校综合系统");
    resize(1400, 900);
}

MainWindow::~MainWindow() = default;


void MainWindow::setup_ui() {
    QWidget* centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    m_webView = new QWebEngineView;
    layout->addWidget(m_webView);
}

void MainWindow::setup_menu_bar() {
    QMenu* fileMenu = menuBar()->addMenu("文件(&F)");

    QAction* refreshAction = new QAction("刷新(&R)", this);
    refreshAction->setShortcut(QKeySequence::Refresh);
    connect(refreshAction, &QAction::triggered, this, &MainWindow::refresh_page);
    fileMenu->addAction(refreshAction);

    fileMenu->addSeparator();

    QAction* exitAction = new QAction("退出(&X)", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);

    QMenu* helpMenu      = menuBar()->addMenu("帮助(&H)");
    QAction* aboutAction = new QAction("关于(&A)", this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::show_about);
    helpMenu->addAction(aboutAction);
}

void MainWindow::setup_status_bar() {
    m_statusLabel = new QLabel("就绪");
    statusBar()->addWidget(m_statusLabel);

    m_progressBar = new QProgressBar;
    m_progressBar->setVisible(false);
    m_progressBar->setMaximumWidth(200);
    statusBar()->addPermanentWidget(m_progressBar);
}

void MainWindow::setup_web_view() {
    QWebEngineSettings* settings = m_webView->settings();
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

    connect(m_webView, &QWebEngineView::loadStarted, this, &MainWindow::on_load_started);
    connect(m_webView, &QWebEngineView::loadProgress, this, &MainWindow::on_load_progress);
    connect(m_webView, &QWebEngineView::loadFinished, this, &MainWindow::on_load_finished);

    m_webChannel = new QWebChannel(this);

    // 1. 创建 WebBridge，现在它的 parent 是 QWebChannel
    m_webBridge = new WebBridge(m_webChannel);

    // 3. 注册对象
    m_webChannel->registerObject("qtBridge", m_webBridge);
    m_webView->page()->setWebChannel(m_webChannel);

    connect(m_webBridge, &WebBridge::page_requested, this, &MainWindow::on_page_requested);
    // 加载HTML文件
    m_webView->load(QUrl("qrc:/web/vue-proj/dist/index.html"));
}

void MainWindow::on_load_started() {
    m_progressBar->setVisible(true);
    m_statusLabel->setText("正在加载...");
}

void MainWindow::on_load_progress(int progress) {
    m_progressBar->setValue(progress);
}

void MainWindow::on_load_finished(bool success) {
    m_progressBar->setVisible(false);

    if (success) {
        m_statusLabel->setText("加载完成");
    } else {
        m_statusLabel->setText("加载失败");
        QMessageBox::warning(this, "错误", "无法加载Web页面，请检查web目录下的HTML文件是否存在。");
    }
}

void MainWindow::on_page_requested(const QString& pageUrl) {
    m_webView->load(QUrl(QString("qrc:/web/vue-proj/dist/index.html#%1").arg(pageUrl)));
}

void MainWindow::refresh_page() {
    m_webView->reload();
}

void MainWindow::show_about() {
    QMessageBox::about(this, "关于",
                       QString("Qt Web学校综合系统\n版本: %1\n\n基于Qt WebEngine的学校综合系统")
                       .arg(QApplication::applicationVersion()));
}

void MainWindow::show_open_dialog(const QString &title, const QString &filter)
{
    qDebug() << "[MainWindow] Received signal. Opening file dialog now.";
    // 由 MainWindow 自己来打开对话框，'this' 是绝对安全的父窗口
    QString filePath = QFileDialog::getOpenFileName(this, title, "", filter);

    if (!filePath.isEmpty()) {
        qDebug() << "[MainWindow] File selected:" << filePath << ". Calling WebBridge to process it.";
        // 将结果返回给 WebBridge 处理
        m_webBridge->process_selected_file(filePath);
    }
}

void MainWindow::show_save_dialog(const QString &title, const QString &filter)
{
    qDebug() << "[MainWindow] Received signal. Opening save dialog now.";
    QString filePath = QFileDialog::getSaveFileName(this, title, "", filter);

    if (!filePath.isEmpty()) {
        qDebug() << "[MainWindow] File path to save:" << filePath << ". Calling WebBridge to process it.";
        m_webBridge->process_save_file_path(filePath);
    }
}

void MainWindow::handle_minimize_to_tray()
{
    qDebug() << "[MainWindow] Received request to minimize. Hiding window.";
    // 这里执行真正的UI操作
    this->hide();
    // 你可能还想在这里显示一个托盘图标的提示消息
    // m_trayIcon->showMessage("提示", "程序已最小化到托盘");
}