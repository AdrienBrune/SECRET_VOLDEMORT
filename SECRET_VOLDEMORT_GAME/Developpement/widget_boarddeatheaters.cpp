#include "widget_boarddeatheaters.h"
#include "ui_widget_boarddeatheaters.h"

Widget_BoardDeathEaters::Widget_BoardDeathEaters(QWidget* parent, S_MESSAGE* MSG) :
    QWidget(parent),
    mMSG(MSG),
    ui(new Ui::Widget_BoardDeathEaters)
{
    ui->setupUi(this);
}

Widget_BoardDeathEaters::~Widget_BoardDeathEaters()
{
    delete ui;
}

void Widget_BoardDeathEaters::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    int horizontalMargin = static_cast<int>(0.070 * this->width()),
        verticalMargin = static_cast<int>(0.2254 * this->height()),
        width = static_cast<int>(0.119 * this->width()),
        height = static_cast<int>(0.553 * this->height()),
        spacing = static_cast<int>(0.03125 * this->width());

    // Get number players.
    quint8 numberPlayerPlaying = 0;
    for(const S_PLAYER &player: mMSG->gameStatus.players)
    {
        if(player.status != E_PLAYER_STATUS::notPlaying)
        {
            numberPlayerPlaying++;
        }
    }

    switch(numberPlayerPlaying)
    {
        case 5:
        case 6:
            painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_BOARD_DEATHEATERS_56))));
            break;

        case 7:
        case 8:
            painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_BOARD_DEATHEATERS_78))));

            break;

        case 9:
        case 10:
            painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_BOARD_DEATHEATERS_910))));
            break;

        default:
            painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_BOARD_DEATHEATERS_56))));
            break;
    }

    if(mMSG->gameStatus.board.boardDeathEaters > 0)
    {
       painter.drawPixmap(horizontalMargin, verticalMargin, width, height, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_DEATHEATERS))));
    }
    if(mMSG->gameStatus.board.boardDeathEaters > 1)
    {
        painter.drawPixmap(horizontalMargin + width + spacing, verticalMargin, width, height, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_DEATHEATERS))));
    }
    if(mMSG->gameStatus.board.boardDeathEaters > 2)
    {
        painter.drawPixmap(horizontalMargin + 2 * (width + spacing), verticalMargin, width, height, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_DEATHEATERS))));
    }
    if(mMSG->gameStatus.board.boardDeathEaters > 3)
    {
        painter.drawPixmap(horizontalMargin + 3 * (width + spacing), verticalMargin, width, height, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_DEATHEATERS))));
    }
    if(mMSG->gameStatus.board.boardDeathEaters > 4)
    {
        painter.drawPixmap(horizontalMargin + 4 * (width + spacing), verticalMargin, width, height, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_DEATHEATERS))));
    }
    if(mMSG->gameStatus.board.boardDeathEaters > 5)
    {
        painter.drawPixmap(horizontalMargin + 5 * (width + spacing), verticalMargin, width, height, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_DEATHEATERS))));
    }
}

