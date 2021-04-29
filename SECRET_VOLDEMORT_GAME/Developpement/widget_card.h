#ifndef CARD_H
#define CARD_H

#include <QPushButton>
#include <QEvent>
#include <QHoverEvent>
#include "data.h"

class Widget_Card : public QPushButton
{
    Q_OBJECT

public:
    explicit Widget_Card(QWidget* parent = nullptr, int i = 10);
    ~Widget_Card();

signals:
    void sig_hoverIn(int);
    void sig_hoverOut(int);

public:
    void setLaw(E_CARD);
    E_CARD getLaw();
    int getIdentifier();

protected:
    void hoverEnter(QHoverEvent* event);
    void hoverLeave(QHoverEvent* event);
    bool event(QEvent* event);

private:
    int mIdentifier;
    E_CARD mLaw;
};

#endif // CARD_H
