#include "ben_afk.h"
#include "./ui_ben_afk.h"

Ben_AFK::Ben_AFK(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Ben_AFK)
{
    ui->setupUi(this);
    setStatusBar(nullptr);
    setFixedSize(1000, 600);

    ui->usernameField->setPlaceholderText("Username");
    ui->passwordField->setPlaceholderText("Password");
    ui->messageInput->setPlaceholderText("Type a message...");
    ui->sentHistoryBox->setReadOnly(true);
    ui->logViewer->setReadOnly(true);
    ui->stackedWidget->setCurrentWidget(ui->loginPage);

    // press enter call "sendButton_clicked"
    connect(ui->messageInput, &QLineEdit::returnPressed, this, &Ben_AFK::on_sendButton_clicked);


    // socket setup
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &Ben_AFK::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &Ben_AFK::onDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &Ben_AFK::onSocketError);

    // log polling
    logTimer = new QTimer(this);
    connect(logTimer, &QTimer::timeout, this, &Ben_AFK::updateLogView);
}


Ben_AFK::~Ben_AFK()
{
    if (socket) {
        socket->disconnect(this);
        socket->disconnectFromHost();
    }
    if (logTimer) {
        logTimer->stop();
    }
    delete ui;
}


void Ben_AFK::on_loginButton_clicked()
{
    QString username = ui->usernameField->text();
    QString password = ui->passwordField->text();

    if (username == "admin" && password == "admin") {
        ui->errorLabel->clear();
        ui->stackedWidget->setCurrentWidget(ui->appPage);
        socket->connectToHost("127.0.0.1", 4242);
        logTimer->start(500);
    } else {
        qDebug() << "Invalid username or password";
        ui->errorLabel->setText("Invalid username or password.");
        QTimer::singleShot(1000, this, [this]() {
            ui->errorLabel->clear();
        });
    }
}

void Ben_AFK::onConnected()
{
    qDebug() << "Connected to daemon";
    ui->connectionStatusLabel->setText("● Connected");
    ui->connectionStatusLabel->setStyleSheet("color: #4CAF50; font-size: 13px; font-weight: bold;");
}

void Ben_AFK::tryConnect()
{
    if (socket->state() == QAbstractSocket::UnconnectedState) {
        socket->connectToHost("127.0.0.1", 4242);
    }
}

void Ben_AFK::onDisconnected()
{
    qDebug() << "Disconnected from daemon";
    ui->connectionStatusLabel->setText("● Disconnected — retrying...");
    ui->connectionStatusLabel->setStyleSheet("color: #ff5555; font-size: 13px; font-weight: bold;");

    QTimer::singleShot(2000, this, [this]() {
        tryConnect();
    });
}

void Ben_AFK::onSocketError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    qDebug() << "Socket error:" << socket->errorString();

    ui->connectionStatusLabel->setText("● Disconnected — retrying...");
    ui->connectionStatusLabel->setStyleSheet("color: #ff5555; font-size: 13px; font-weight: bold;");

    QTimer::singleShot(2000, this, [this]() {
        tryConnect();
    });
}

void Ben_AFK::on_sendButton_clicked()
{
    QString msg = ui->messageInput->text();
    if (msg.isEmpty())
        return;

    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(msg.toUtf8() + "\n");
        socket->flush();

        ui->sentHistoryBox->append("> " + msg);
        ui->messageInput->clear();
    } else {
        ui->sentHistoryBox->append("[Error] Not connected to daemon.");
    }
}


void Ben_AFK::updateLogView()
{
    QFile file(logFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    file.seek(lastLogPos);
    QByteArray newData = file.readAll();
    lastLogPos = file.pos();

    if (!newData.isEmpty()) {
        ui->logViewer->insertPlainText(QString::fromUtf8(newData));
        ui->logViewer->verticalScrollBar()->setValue(
            ui->logViewer->verticalScrollBar()->maximum());
    }
}

