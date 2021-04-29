#include "widget_card.h"

Widget_Card::Widget_Card(QWidget* parent, int i):
    QPushButton(parent),
    mIdentifier(i),
    mLaw(E_CARD::deathEatersLaw)
{
    this->setAttribute(Qt::WA_Hover, true);

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
}

Widget_Card::~Widget_Card()
{

}

void Widget_Card::setLaw(E_CARD law)
{
    mLaw = law;
}

E_CARD Widget_Card::getLaw()
{
    return mLaw;
}

int Widget_Card::getIdentifier()
{
    return mIdentifier;
}

bool Widget_Card::event(QEvent *event)
{
    switch (event->type())
    {
        case QEvent::HoverEnter:
            hoverEnter(static_cast<QHoverEvent*>(event));
            return true;

        case QEvent::HoverLeave:
            hoverLeave(static_cast<QHoverEvent*>(event));
            return true;

        case QEvent::HoverMove:
            return true;

        default:
            break;
    }

    return QWidget::event(event);
}

void Widget_Card::hoverLeave(QHoverEvent*)
{
    emit sig_hoverOut(mIdentifier);
}

void Widget_Card::hoverEnter(QHoverEvent*)
{
    emit sig_hoverIn(mIdentifier);
}
