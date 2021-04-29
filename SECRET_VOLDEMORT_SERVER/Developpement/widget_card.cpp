#include "widget_card.h"
#include "ui_widget_card.h"

Widget_Card::Widget_Card(QWidget* parent, E_CARD card):
    QWidget(parent),
    mCard(card),
    ui(new Ui::Widget_Card)
{
    ui->setupUi(this);
}

Widget_Card::~Widget_Card()
{
    delete ui;
}

void Widget_Card::setCard(E_CARD card)
{
    mCard = card;
}

E_CARD Widget_Card::getCard()
{
    return mCard;
}

void Widget_Card::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    switch(mCard)
    {
        case E_CARD::facisteLaw:
            painter.drawPixmap(QRect(0, 0, width(), height()), QPixmap(":/images/fasciste_card.png"));
            break;

        case E_CARD::liberalLaw:
            painter.drawPixmap(QRect(0, 0, width(), height()), QPixmap(":/images/liberal_card.png"));
            break;
    }

}
