#include "tcp_api.h"
#include "dialog_menu.h"
#include <QTimer>

TCP_API::TCP_API(QObject *parent, QString* name, QString* ip):
    QObject(parent),
    mName(name),
    mIp(ip)
{
    mSocket = new QTcpSocket(this);
    connect(mSocket,SIGNAL(connected()),this,SLOT(connection()));
    connect(mSocket, SIGNAL(readyRead()), this, SLOT(receive_MSG()));
}

void TCP_API::connection()
{
    qDebug() << "Client connecté";
}

void TCP_API::connectToServer()
{
    if(mSocket->state() == QAbstractSocket::ConnectedState || mSocket->state() == QAbstractSocket::ConnectingState)
    {
        mSocket->close();

        mSocket = new QTcpSocket(this);
        connect(mSocket,SIGNAL(connected()),this,SLOT(connection()));
        connect(mSocket, SIGNAL(disconnected()), SLOT(disconnected()));
        connect(mSocket, SIGNAL(readyRead()), this, SLOT(receive_MSG()));
    }

    mSocket->connectToHost(QHostAddress(*mIp), static_cast<quint16>(DEFAULT_PORT));

    QTimer::singleShot(500, [&](){ emit sig_connectionResult(mSocket->state()); });
}

void TCP_API::disconnected()
{
    qDebug() << "Déconnection du client";
}

void TCP_API::socketError(int)
{
    qDebug() << "TCP error";
}

void TCP_API::send_MSG(S_MESSAGE MSG)
{
    if(mSocket->state() == QTcpSocket::SocketState::UnconnectedState)
        return;

    // Name update if necessary.
    MSG.gameStatus.players[MSG.identifier].name = QString(*mName);

    QDataStream stream(mSocket);
    stream << MSG;
}

void TCP_API::receive_MSG()
{
    S_MESSAGE MSG;
    QDataStream stream(mSocket);

    // If messages(MSG) are packed, the last one is kept.
    while(mSocket->bytesAvailable() != 0)
        stream >> MSG;

    // First MSG comming from server.
    if(MSG.command == CMD_TO_PLAYER_INIT_COMMUNICATION)
    {
        MSG.gameStatus.players[MSG.identifier].name = QString(*mName);
        MSG.command = CMD_TO_SERVER_CHANGE_NAME;
        send_MSG(MSG);
    }

    emit sig_MSG_received(MSG);
}
