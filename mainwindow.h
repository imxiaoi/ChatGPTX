#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "highlighter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_send_clicked();

    void on_btn_new_session_clicked();

private:
    Ui::MainWindow *ui;

    const QString MODEL_ID = "gpt-3.5-turbo";

    // 定义 API 请求 URL
    const QString API_URL = "https://api.openai.com/v1/";

    QString sendGptRequest(const QString& prompt, const int maxTokens, const int n);

    Highlighter hl2{};

};
#endif // MAINWINDOW_H
