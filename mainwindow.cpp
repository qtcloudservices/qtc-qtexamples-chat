#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_qnam(new QNetworkAccessManager(this))
{
    ui->setupUi(this);

    QUrl url = QUrl("https://staging-mws-api-eu-1.qtc.io");
    url.setPath("/v1/channels/5301ec989a357417b9000028/subscribe");

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("QtC-Websocket-Id", "5301ec979a357417b9000025");
    request.setUrl(url);

    QNetworkReply *reply = m_qnam->post(request, (QIODevice*)0);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::socketFound);

    ui->lineEdit->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::socketFound()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    QByteArray data = reply->readAll();
    QJsonObject dataObject = QJsonDocument::fromJson(data).object();

    m_websocket = new QWebSocket;
    m_websocket->setParent(this);
    connect(m_websocket, &QWebSocket::connected, this, &MainWindow::websocketConnected);
    connect(m_websocket, &QWebSocket::disconnected, this, &MainWindow::connectionClosed);

    QUrl url = dataObject["expiringUrl"].toString();
    qDebug() << url;
    m_websocket->open(url);
    reply->deleteLater();
}

void MainWindow::websocketConnected()
{
    qDebug() << "Websocket connected";
    connect(ui->lineEdit, &QLineEdit::editingFinished, this, &MainWindow::sendMessage);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(m_websocket, &QWebSocket::textMessageReceived, this, &MainWindow::messageReceived);

    ui->lineEdit->setEnabled(true);
}

void MainWindow::connectionClosed()
{
    qDebug() << "Connection closed";
    ui->lineEdit->setEnabled(false);
    ui->sendButton->setEnabled(false);
}

void MainWindow::sendMessage()
{
    QString text = ui->lineEdit->text();
    if (text.isEmpty())
        return;
    ui->lineEdit->clear();
    text.prepend(ui->comboBox->currentText() + QStringLiteral(": "));

    m_websocket->sendTextMessage(text);
    ui->plainTextEdit->appendPlainText(text);
}

void MainWindow::messageReceived(const QString &message)
{
    qDebug() << "message received:" << message;
    ui->plainTextEdit->appendPlainText(message);
}

