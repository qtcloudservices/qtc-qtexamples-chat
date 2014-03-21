#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QtWebSockets>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void sendMessage();
    void messageReceived(const QString &message);
    void socketFound();
    void websocketConnected();
    void messageSent();

    void connectionClosed();
    void deleteReply(QNetworkReply *reply);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *m_qnam;
    QWebSocket *m_websocket;
    QUrl m_baseUrl;
    QString m_basePath;
    QNetworkRequest m_requestTemplate;
};

#endif // MAINWINDOW_H
