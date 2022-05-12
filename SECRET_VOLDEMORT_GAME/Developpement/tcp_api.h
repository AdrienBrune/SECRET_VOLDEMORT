#ifndef TCP_API_H
#define TCP_API_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include "data.h"

class TCP_API : public QObject
{
    Q_OBJECT
public:
    explicit TCP_API(QObject *parent = nullptr, S_MESSAGE * msg = nullptr, QString* name = nullptr, QString* ip = nullptr);

signals :
    void sig_MSG_received(S_MESSAGE);
    void sig_showMessageConnexion();
    void sig_connectionResult(QAbstractSocket::SocketState);

private slots :
    void connection();
    void receive_MSG();
    void disconnected();
    void socketError(int);
    void ackTimeout();

public slots :
    void connectToServer();

public:
    void send_MSG(S_MESSAGE);

private :
    S_MESSAGE * mMSG;
    QString* mName;
    QString* mIp;
    QTimer * mAckTimeout;
    uint8_t mLastCommand;

public :
    QTcpSocket * mSocket;

};

#endif // TCP_API_H
