#include "widget_card.h"
#include "ui_widget_card.h"

Widget_Card::Widget_Card(QWidget* parent, E_CARD card, bool* antispoil):
    QWidget(parent),
    mCard(card),
    mAntiSpoil(antispoil),
    mHover(false),
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

    QRect canvas;
    if(mHover)
    {
        canvas.setRect(0,0,width(),height());
    }
    else
    {
        canvas.setRect((width()/10)/2,(height()/10)/2,width()*9/10,height()*9/10);
    }

    if(*mAntiSpoil)
    {
        painter.drawPixmap(canvas, QPixmap(":/images/Tuile_back.png"));
        return;
    }

    switch(mCard)
    {
        case E_CARD::deathEatersLaw:
            painter.drawPixmap(canvas, QPixmap(":/images/Tuile_Mangemort.png"));
            break;

        case E_CARD::phenixOrderLaw:
            painter.drawPixmap(canvas, QPixmap(":/images/Tuile_Phenix.png"));
            break;
    }

}

void Widget_Card::enterEvent(QEvent*)
{
    mHover = true;
    update();
}

void Widget_Card::leaveEvent(QEvent*)
{
    mHover = false;
    update();
}
