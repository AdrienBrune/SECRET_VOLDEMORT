#include "widget_boardphenixorder.h"
#include "ui_widget_boardphenixorder.h"
#include <QDebug>

Widget_BoardPhenixOrder::Widget_BoardPhenixOrder(QWidget* parent, S_MESSAGE* MSG):
    QWidget(parent),
    mMSG(MSG),
    ui(new Ui::Widget_BoardPhenixOrder)
{
    ui->setupUi(this);
}

Widget_BoardPhenixOrder::~Widget_BoardPhenixOrder()
{
    delete ui;
}

void Widget_BoardPhenixOrder::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    int horizontalMargin = static_cast<int>(0.146 * this->width()),
        verticalMargin = static_cast<int>(0.225 * this->height()),
        width = static_cast<int>(0.119 * this->width()),
        height = static_cast<int>(0.556 * this->height()),
        spacing = static_cast<int>(0.0332 * this->width());

    painter.drawPixmap(0,0,this->width(),this->height(),QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_BOARD_PHENIX))));

    if(mMSG->gameStatus.board.boardPhenixOrder > 0)
    {
        painter.drawPixmap(horizontalMargin, verticalMargin, width, height, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_PHENIXORDER))));
    }
    if(mMSG->gameStatus.board.boardPhenixOrder > 1)
    {
        painter.drawPixmap(horizontalMargin + width + spacing, verticalMargin, width, height, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_PHENIXORDER))));
    }
    if(mMSG->gameStatus.board.boardPhenixOrder > 2)
    {
        painter.drawPixmap(horizontalMargin + 2 * (width + spacing), verticalMargin, width, height, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_PHENIXORDER))));
    }
    if(mMSG->gameStatus.board.boardPhenixOrder > 3)
    {
        painter.drawPixmap(horizontalMargin + 3 * (width + spacing), verticalMargin, width, height, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_PHENIXORDER))));
    }
    if(mMSG->gameStatus.board.boardPhenixOrder > 4)
    {
        painter.drawPixmap(horizontalMargin + 4 * (width + spacing), verticalMargin, width, height, QPixmap::fromImage(QImage(":/images/liberal_card.png")));
    }
}


