#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_qnam(new QNetworkAccessManager(this))
{
    // In order to connect to the WebSocket instance the address and id are required.
    // The gateway id and instance address are available from your account
    // at https://console.qtcloudservices.com
    QString MWS_GATEWAY_ID = QStringLiteral(YOUR_MWS_GATEWAY_ID_HERE);
    QString MWS_INSTANCE_ADDRESS = QStringLiteral(YOUR_MWS_INSTANCE_ADDRESS_HERE);

    m_baseUrl = QUrl(MWS_INSTANCE_ADDRESS);
    m_basePath = ("/v1/gateways/" + MWS_GATEWAY_ID);

    connect(m_qnam, &QNetworkAccessManager::finished, this, &MainWindow::deleteReply);
    // Set up all communication with the server to use JSON
    m_requestTemplate.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_requestTemplate.setRawHeader("Accept", "application/json");

    // First a REST call is needed to get the final websocket URI
    QUrl getSocketUrl = m_baseUrl;
    getSocketUrl.setPath(m_basePath + "/websocket_uri");
    QNetworkRequest request(m_requestTemplate);
    request.setUrl(getSocketUrl);

    QNetworkReply *reply = m_qnam->get(request);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::socketFound);

    // Set up the UI
    ui->setupUi(this);
    ui->lineEdit->setEnabled(false);
    ui->sendButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::socketFound()
{
    // We have a response from the server, hopefully containing the URI of the websocket instance
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    QByteArray data = reply->readAll();

    if (reply->error()) {
        ui->plainTextEdit->setPlainText(QString("Error: %1 - %2: %3").arg(QString::number(reply->error()), reply->errorString(), data));
        return;
    }

    QJsonObject dataObject = QJsonDocument::fromJson(data).object();

    // Create a websocket
    m_websocket = new QWebSocket;
    m_websocket->setParent(this);
    connect(m_websocket, &QWebSocket::connected, this, &MainWindow::websocketConnected);
    connect(m_websocket, &QWebSocket::disconnected, this, &MainWindow::connectionClosed);

    // And open it with the URI
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
    // Do not leak network replies
    reply->deleteLater();
}

void MainWindow::sendMessage()
{
    // Currently the way to broadcast messages with Qt Cloud Service's
    // WebSockets is to make a rest call.
    // With the sockets and tags parameters in JSON it's possible to specify the
    // recipient, but for this example we send our messages to all users.

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

