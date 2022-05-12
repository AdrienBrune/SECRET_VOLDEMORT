#include "tcp_api.h"
#include "util_functions.h"

uint8_t lastCommand = 0;

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

    emit sig_playerConnection(socket);
}

void TCP_API::socketEvent(QAbstractSocket::SocketState state)
{
    QTcpSocket* socket = static_cast<QTcpSocket*>(QObject::sender());
    QStringList events = {
        "UnconnectedState",
        "HostLookupState",
        "ConnectingState",
        "ConnectedState",
        "BoundState",
        "ClosingState",
        "ListeningState",
        "unknown"
    };

    for(const S_PLAYER &player : qAsConst(mMSG.gameStatus.players))
    {
        if(player.socket == socket)
        {
            emit sig_print("Socket event (" + player.name + ") : " + events[state < (events.size()-1) ? state : (events.size()-1)]);
        }
    }

    if(state == QAbstractSocket::UnconnectedState)
    { 
        emit sig_playerDisconnection(socket);
    }
}

void TCP_API::ackTimeout()
{
    QObject* playerTimeout = QObject::sender();
    for(S_PLAYER player : qAsConst(mGame->players))
    {
        if(playerTimeout == player.ackTimeout)
        {
            if(player.ackTimeout)
                player.ackTimeout->stop();

            emit sig_print("ACK Timeout : " + player.name);

            mMSG.command = lastCommand;
            send_MSG(mMSG, mGame->players[player.identifier].socket);

            emit sig_print(STR_SEND + QString("%1(%2)\n").arg(player.name).arg(player.identifier) + STR_SPACER + getCommandString(mMSG.command));
        }
    }
}

void TCP_API::setCommand(quint8 command)
{
    mMSG.command = command;
    lastCommand = command;
}

void TCP_API::send_MSG(S_GAME_STATUS game)
{
    mMSG.gameStatus = game;

    for(S_PLAYER player : qAsConst(game.players))
    {
        if(player.socket == nullptr)
        {
            emit sig_print(QString("%1(%2) sans socket").arg(player.name).arg(player.identifier));
            continue;
        }

        if(player.socket->state() == QTcpSocket::UnconnectedState)
        {
            emit sig_print(QString("%1(%2) non connecté").arg(player.name).arg(player.identifier));
            continue;
        }

        mMSG.identifier = player.identifier;
        QDataStream stream(player.socket);
        stream << mMSG;

        if(player.ackTimeout)
        {
            player.ackTimeout->start(MSG_TIMEOUT);
            qDebug() << "Wait ACK : " << player.name;
        }

        emit sig_print(STR_SEND + QString("%1(%2)\n").arg(player.name).arg(player.identifier) + STR_SPACER + getCommandString(mMSG.command));
    }
}

void TCP_API::send_MSG(S_MESSAGE MSG, QTcpSocket* socket)
{
    QDataStream stream(socket);
    stream << MSG;

    for(S_PLAYER player : qAsConst(mGame->players))
    {
        if(player.socket == socket)
        {
            if(player.ackTimeout)
            {
                if(MSG.command != CMD_ACKNOLEDGE)
                    player.ackTimeout->start(MSG_TIMEOUT);
            }
        }
    }
}

void TCP_API::receive_MSG()
{
    uint8_t commandTmp;
    S_MESSAGE MSG;
    QTcpSocket* socket = static_cast<QTcpSocket*>(QObject::sender());
    QDataStream stream(socket);

    // If messages(MSG) are packed, all are read.
    while(socket->bytesAvailable() != 0)
    {
        stream >> MSG;

        if(MSG.command == CMD_ACKNOLEDGE)
        {
            for(const S_PLAYER &player : qAsConst(mGame->players))
            {
                if(mGame->players[player.identifier].socket == socket)
                {
                    if(mGame->players[player.identifier].ackTimeout)
                    {
                        mGame->players[player.identifier].ackTimeout->stop();
                        emit sig_print(QString("ACK received : %1").arg(player.name));
                    }
                }
            }
            continue;
        }

        /* Send acknoledge message */
        commandTmp = MSG.command;
        MSG.command = CMD_ACKNOLEDGE;
        send_MSG(MSG, socket);
        MSG.command = commandTmp;

        mMSG = MSG;
        emit sig_print(STR_RECEIVE + QString("%1(%2)\n").arg(mMSG.gameStatus.players[mMSG.identifier].name).arg(mMSG.identifier) + STR_SPACER + getCommandString(mMSG.command));
        emit sig_MSG_received(mMSG);
     }
}
