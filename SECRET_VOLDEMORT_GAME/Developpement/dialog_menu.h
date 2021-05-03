#ifndef WELCOME_MENU_H
#define WELCOME_MENU_H

#include <QDialog>
#include <QTimer>
#include <QTcpSocket>
#include "soundmanager.h"

#define DEFAULT_PORT    30000
#define DEFAULT_IP      "127.0.0.1"

namespace Ui {
class Menu;
}

class Dialog_Menu : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Menu(QWidget *parent = nullptr);
    ~Dialog_Menu();

signals:
    void sig_connection();
    void sig_showBoard();
    void sig_changeName();
    void sig_joinGame();
    void sig_playSound(SoundManager::E_SOUND);

public slots:
    void onShowConnectionResult(QAbstractSocket::SocketState);

private slots:
    void onEditIp(const QString &);
    void onEditPort(const QString &);
    void onEditName(const QString &);
    void onButtonConnectionClicked();
    void onButtonBoardClicked();
    void onButtonCloseClicked();
    void onButtonJoinGame();

public:
    void OpenMenu(QTcpSocket* socket);
    QString* getName();
    QString* getIp();
    int* getPort();
    void showMessageStatus(QString);

protected:
    void paintEvent(QPaintEvent *);

public:
    int mPort;
    QString mIp;
    QString mName;

private:
    Ui::Menu *ui;
};

#endif // WELCOME_MENU_H
