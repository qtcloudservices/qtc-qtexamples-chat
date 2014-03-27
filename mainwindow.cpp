#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_qnam(new QNetworkAccessManager(this))
{
    ui->setupUi(this);

    // Get the gateway id from console.qtcloudservices.com
    QString MWS_GATEWAY_ID = QStringLiteral("YOUR_MWS_GATEWAY_ID_HERE");

    m_baseUrl = QUrl("YOUR_MWS_INSTANCE_ADDRESS_HERE");
    m_basePath = ("/v1/gateways/" + MWS_GATEWAY_ID);

    QUrl getSocketUrl = m_baseUrl;
    getSocketUrl.setPath(m_basePath + "/websocket_uri");

    connect(m_qnam, &QNetworkAccessManager::finished, this, &MainWindow::deleteReply);

    m_requestTemplate.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_requestTemplate.setRawHeader("Accept", "application/json");

    QNetworkRequest request(m_requestTemplate);
    request.setUrl(getSocketUrl);

    QNetworkReply *reply = m_qnam->get(request);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::socketFound);

    ui->lineEdit->setEnabled(false);
    ui->sendButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::socketFound()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    QByteArray data = reply->readAll();

    if (reply->error()) {
        ui->plainTextEdit->setPlainText(QString("Error: %1 - %2: %3").arg(QString::number(reply->error()), reply->errorString(), data));
        return;
    }

    QJsonObject dataObject = QJsonDocument::fromJson(data).object();

    m_websocket = new QWebSocket;
    m_websocket->setParent(this);
    connect(m_websocket, &QWebSocket::connected, this, &MainWindow::websocketConnected);
    connect(m_websocket, &QWebSocket::disconnected, this, &MainWindow::connectionClosed);

    QUrl url = dataObject["uri"].toString();
    m_websocket->open(url);
}

void MainWindow::websocketConnected()
{
    qDebug() << "Websocket connected";
    connect(ui->lineEdit, &QLineEdit::editingFinished, this, &MainWindow::sendMessage);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(m_websocket, &QWebSocket::textMessageReceived, this, &MainWindow::messageReceived);

    ui->lineEdit->setEnabled(true);
    ui->sendButton->setEnabled(true);
}

void MainWindow::connectionClosed()
{
    ui->plainTextEdit->appendPlainText("Connection closed.");
    ui->lineEdit->setEnabled(false);
    ui->sendButton->setEnabled(false);
}

void MainWindow::deleteReply(QNetworkReply *reply)
{
    reply->deleteLater();
}

void MainWindow::sendMessage()
{
    QString text = ui->lineEdit->text();
    if (text.isEmpty())
        return;
    ui->lineEdit->clear();
    text.prepend(ui->comboBox->currentText() + QStringLiteral(": "));

    QUrl sendMessageUrl = m_baseUrl;
    sendMessageUrl.setPath(m_basePath + "/messages");

    QNetworkRequest request(m_requestTemplate);
    request.setUrl(sendMessageUrl);

    QJsonObject receiversObject;
    QJsonArray everyone;
    everyone.append("*");
    receiversObject["sockets"] = everyone;
    receiversObject["tags"] = QJsonArray();

    QJsonObject dataObject;
    dataObject["data"] = text;
    dataObject["receivers"] = receiversObject;
    QJsonDocument doc(dataObject);
    QNetworkReply *reply = m_qnam->post(request, doc.toJson());

    connect(reply, &QNetworkReply::finished, this, &MainWindow::messageSent);
}

void MainWindow::messageSent()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error())
        ui->plainTextEdit->appendPlainText(QString("Error sending message: %1 - %2, %3").arg(QString::number(reply->error()), reply->errorString(), QString::fromUtf8(reply->readAll())));
}

void MainWindow::messageReceived(const QString &message)
{
    ui->plainTextEdit->appendPlainText(message);
}

