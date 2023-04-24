#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QSettings>>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    hl2.setDocument(ui->tb_session->document());
}


MainWindow::~MainWindow()
{
    delete ui;
}
QString MainWindow::sendGptRequest(const QString& prompt, const int maxTokens, const int n) {

    QSettings settings("config.ini", QSettings::IniFormat);

    // 读取proxy部分的配置项
    settings.beginGroup("proxy");
    QString proxyType = settings.value("proxy_type").toString();
    QString host = settings.value("host").toString();
    int port = settings.value("port").toInt();
    QString username = settings.value("username").toString();
    QString password = settings.value("password").toString();
    settings.endGroup();

    // 读取openai部分的配置项
    settings.beginGroup("openai");
    QString apiKey = settings.value("api_key").toString();
    settings.endGroup();
    QNetworkProxy proxy;
    if(proxyType == "http")
    {
        proxy.setType(QNetworkProxy::HttpProxy);
    }else if(proxyType == "socks5")
    {
        proxy.setType(QNetworkProxy::Socks5Proxy);
    }
    proxy.setHostName(host);
    proxy.setPort(port);
    proxy.setUser(username);
    proxy.setPassword(password);
    QNetworkProxy::setApplicationProxy(proxy);

    // 创建网络访问管理器
    QNetworkAccessManager networkManager;

    // 设置 HTTP 请求
    QNetworkRequest request;
    request.setUrl(QUrl("https://api.openai.com/v1/chat/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + apiKey).toUtf8());

    // 设置请求体
    QJsonObject requestBody;
    requestBody.insert("model", "gpt-3.5-turbo");
    QJsonArray messages;
    QJsonObject message;
    message.insert("role", "user");
    message.insert("content", prompt);
    messages.append(message);
    requestBody.insert("messages", messages);

    QJsonDocument jsonDoc(requestBody);
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact);

    // 发送请求并等待响应
    QNetworkReply* reply = networkManager.post(request, jsonData);
    while (!reply->isFinished()) {
        qApp->processEvents();
    }

    // 处理响应数据
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error: " << reply->errorString();
        return QString(reply->errorString());
    }

    auto response = reply->readAll();
    QJsonObject response_object = QJsonDocument::fromJson(response).object();
    QJsonArray choices_array = response_object.value("choices").toArray();
    QString generated_text{};
    if (!choices_array.isEmpty()) {
        generated_text = choices_array[0].toObject().value("message").toObject().value("content").toString();
    }
    reply->deleteLater();
    return generated_text;
}
void MainWindow::on_btn_send_clicked()
{
    // 生成文本
    int num_tokens = 200;
    ui->tb_session->append("Q:\n" + ui->textEdit->toPlainText());

    QString promt = ui->checkBox_context->isChecked() ? ui->tb_session->toPlainText() + '\n' + ui->textEdit->toPlainText() : ui->textEdit->toPlainText();
    ui->textEdit->clear();

    QString generated_text = sendGptRequest(promt, num_tokens, 1);

    ui->tb_session->append("A:\n" + generated_text);
}


void MainWindow::on_btn_new_session_clicked()
{
    ui->tb_session->clear();
}

