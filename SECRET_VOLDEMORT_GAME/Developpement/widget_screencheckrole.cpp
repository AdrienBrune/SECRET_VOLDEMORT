#include "widget_screencheckrole.h"
#include "ui_widget_screencheckrole.h"

Widget_ScreenCheckRole::Widget_ScreenCheckRole(QWidget* parent, S_MESSAGE* MSG):
    QWidget(parent),
    mMSG(MSG),
    ui(new Ui::Widget_ScreenCheckRole)
{
    ui->setupUi(this);
}

void Widget_ScreenCheckRole::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap::fromImage(QImage(":/images/background_motif.png")));

    if(mMSG->gameStatus.players[mMSG->identifier].electionRole != E_ELECTION_ROLE::minister)
        return;

    // Draw player role.
    switch(mMSG->gameStatus.players[mMSG->gameStatus.playerFocus].role)
    {
        case E_ROLE::deathEaters:
        case E_ROLE::voldemort:
            ui->role->setStyleSheet(QString("QLabel{border-image:url(:/images/%1)}").arg(RES_ALLEGIANCE_DEATHEATERS));
            break;

        case E_ROLE::phenixOrder:
            ui->role->setStyleSheet(QString("QLabel{border-image:url(:/images/%1)}").arg(RES_ALLEGIANCE_PHENIXORDER));
            break;

        default:
            break;
    }
}

Widget_ScreenCheckRole::~Widget_ScreenCheckRole()
{
    delete ui;
}
