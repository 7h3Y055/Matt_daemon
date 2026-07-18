#ifndef BEN_AFK_H
#define BEN_AFK_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QFile>
#include <QTimer>
#include <QScrollBar>


QT_BEGIN_NAMESPACE
namespace Ui {
class Ben_AFK;
}
QT_END_NAMESPACE

class Ben_AFK : public QMainWindow
{
    Q_OBJECT

public:
    explicit Ben_AFK(QWidget *parent = nullptr);
    ~Ben_AFK() override;

private slots:
    void on_loginButton_clicked();
    void on_sendButton_clicked();

    void onConnected();
    void onDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);
    void updateLogView();
    void tryConnect();

private:
    Ui::Ben_AFK *ui;
    QTcpSocket *socket;
    QTimer *logTimer;
    qint64 lastLogPos = 0;
    QString logFilePath = "/var/log/matt_daemon/matt_daemon.log";

};
#endif // BEN_AFK_H
