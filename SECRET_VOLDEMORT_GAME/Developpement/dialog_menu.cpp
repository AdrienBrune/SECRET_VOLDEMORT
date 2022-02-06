#include "dialog_menu.h"
#include "ui_dialog_menu.h"
#include <QDebug>
#include <QPainter>

char version[15+1];

Dialog_Menu::Dialog_Menu(QWidget *parent) :
    QDialog(parent),
    mIp(DEFAULT_IP),
    mName(""),
    ui(new Ui::Menu)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);

    QCursor cursor(QPixmap(":/images/custom_cursor.png"), 20, 20);
    setCursor(cursor);

    connect(ui->editIp, SIGNAL(textEdited(QString)), this, SLOT(onEditIp(QString)));
    connect(ui->editName, SIGNAL(textEdited(QString)), this, SLOT(onEditName(QString)));
    connect(ui->buttonConnect, SIGNAL(clicked()), this, SLOT(onButtonConnectionClicked()));
    connect(ui->buttonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));
    connect(ui->buttonJoinGame, SIGNAL(clicked()), this, SLOT(onButtonJoinGame()));

    ui->buttonJoinGame->hide();

    ui->version->setText(version);
}

Dialog_Menu::~Dialog_Menu()
{
    delete ui;
}

void Dialog_Menu::onShowConnectionResult(QAbstractSocket::SocketState state)
{
    switch(state)
    {
        case QAbstractSocket::ConnectedState:
            ui->message->setText("Connexion réussi");
            QTimer::singleShot(1000, this, [&](){ui->message->setText("");});
            break;

        default:
            ui->message->setText("Connexion échoué");
            QTimer::singleShot(1000, this, [&](){ui->message->setText("");});
            break;
    }
}

void Dialog_Menu::OpenMenu(QTcpSocket* socket)
{
    if(socket != nullptr)
    {
        switch(socket->state())
        {
            case QTcpSocket::SocketState::ConnectedState:
                ui->buttonJoinGame->show();
                break;

            case QTcpSocket::SocketState::UnconnectedState:
            default:
                ui->buttonJoinGame->hide();
                break;
        }
    }

    exec();
}

QString* Dialog_Menu:: getName()
{
    return &mName;
}

QString* Dialog_Menu::getIp()
{
    return &mIp;
}

void Dialog_Menu::showMessageStatus(QString message)
{
    ui->message->setText(message);
    QTimer::singleShot(1000, this, [&](){ui->message->setText("");});
}

void Dialog_Menu::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setOpacity(0.96);
    painter.setBrush(QColor(19,19,19));
    painter.drawRect(QRect(0, 0, width(), height()));
}

void Dialog_Menu::onEditName(const QString & text)
{
    mName = text;
}

void Dialog_Menu::onEditIp(const QString &text)
{
    mIp = text;
}

void Dialog_Menu::onButtonConnectionClicked()
{
    if((ui->editName->text() != "") && (ui->editIp->text() != "") && ui->editName->text().length() <= 10)
    {
        emit sig_connection();
    }
    else
    {
        if(ui->editName->text().length() > 10)
        {
            showMessageStatus("Le pseudo ne doit pas dépasser 10 caractères");
        }
        else
        {
            showMessageStatus("Veuillez remplir tous les champs");
        }
    }
}

void Dialog_Menu::onButtonCloseClicked()
{
    accept();
}

void Dialog_Menu::onButtonJoinGame()
{
    emit sig_joinGame();
}

