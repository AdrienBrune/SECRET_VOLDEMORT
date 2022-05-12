#ifndef TCP_API_H
#define TCP_API_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "types.h"
#include "util_functions.h"

#define PORT 30000

class TCP_API : public QObject
{
    Q_OBJECT

public:
    TCP_API();

signals:
    void sig_print(QString message);
    void sig_MSG_received(S_MESSAGE MSG);
    void sig_playerConnection(QTcpSocket* socket);
    void sig_playerDisconnection(QTcpSocket* socket);

private slots:
    void newPlayer();
    void receive_MSG();
    void socketEvent(QAbstractSocket::SocketState socketState);

public slots:
    void ackTimeout();

public:
    void setCommand(quint8 command);

public:
    void startServer();
    void send_MSG(S_GAME_STATUS game);
    void send_MSG(S_MESSAGE MSG, QTcpSocket* socket);
    void sendLast_MSG();

public:
    S_GAME_STATUS* mGame;

private:
    S_MESSAGE mMSG;
    QTcpServer* mServer;
};

#endif // TCP_API_H
