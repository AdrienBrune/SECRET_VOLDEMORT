#include "tcp_api.h"
#include "util_functions.h"

TCP_API::TCP_API()
{
    mServer = new QTcpServer(this);
}

void TCP_API::startServer()
{
    mServer->listen(QHostAddress::Any, static_cast<quint16>(PORT));
    connect(mServer, SIGNAL(newConnection()), this, SLOT(newPlayer()));
}

void TCP_API::newPlayer()
{
    QTcpSocket* socket = mServer->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(receive_MSG()));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketEvent(QAbstractSocket::SocketState)));

    sig_playerConnection(socket);
}

void TCP_API::socketEvent(QAbstractSocket::SocketState state)
{
    QTcpSocket* socket = nullptr;

    switch(state)
    {
        case QAbstractSocket::UnconnectedState:
            emit sig_print("SOCKET : unconnected");
            socket = static_cast<QTcpSocket*>(QObject::sender());
            emit sig_playerDisconnection(socket);
            break;

        case QAbstractSocket::ClosingState:
            emit sig_print("SOCKET : closing state");
            break;

        default:
            emit sig_print("SOCKET : erreur");
            break;
    }
}

void TCP_API::setCommand(quint8 command)
{
    mMSG.command = command;
}

void TCP_API::send_MSG(S_GAME_STATUS game)
{
    mMSG.gameStatus = game;

    for(S_PLAYER player : game.players)
    {
        if(player.socket == nullptr)
        {
            qDebug() << "Player sans socket";
            continue;
        }

        if(player.socket->state() == QTcpSocket::UnconnectedState)
        {
            qDebug() << "Player non connecté";
            continue;
        }

        mMSG.identifier = player.identifier;
        QDataStream stream(player.socket);
        stream << mMSG;
    }

    sig_print("[ENV] " + getCommandString(mMSG.command));
}

void TCP_API::send_MSG(S_MESSAGE MSG, QTcpSocket* socket)
{
    QDataStream stream(socket);
    stream << MSG;
}

void TCP_API::receive_MSG()
{
    QTcpSocket* socket = static_cast<QTcpSocket*>(QObject::sender());
    QDataStream stream(socket);
    stream >> mMSG;

    emit sig_print("[REC] " + mMSG.gameStatus.players[mMSG.identifier].name + "(" + QString("%1").arg(mMSG.identifier) + ")");

    emit sig_MSG_received(mMSG);
}
