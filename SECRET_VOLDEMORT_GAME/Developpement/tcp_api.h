#ifndef TCP_API_H
#define TCP_API_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include "data.h"

class TCP_API : public QObject
{
    Q_OBJECT
public:
    explicit TCP_API(QObject *parent = nullptr, QString* name = nullptr, QString* ip = nullptr, int* port = nullptr);

signals :
    void sig_MSG_received(S_MESSAGE);
    void sig_showMessageConnexion();
    void sig_connectionResult(QAbstractSocket::SocketState);

private slots :
    void connection();
    void receive_MSG();
    void disconnected();
    void socketError(int);

public slots :
    void connectToServer();

public:
    void send_MSG(S_MESSAGE);

private :
    QString* mName;
    QString* mIp;
    int* mPort;

public :
    QTcpSocket * mSocket;

};

#endif // TCP_API_H
