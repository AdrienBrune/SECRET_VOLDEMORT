#ifndef LIBERAL_BOARD_H
#define LIBERAL_BOARD_H

#include <QWidget>
#include <QPainter>
#include "data.h"

namespace Ui {
class Widget_BoardPhenixOrder;
}

class Widget_BoardPhenixOrder : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_BoardPhenixOrder(QWidget* parent = nullptr, S_MESSAGE* MSG = nullptr);
    ~Widget_BoardPhenixOrder();

protected:
    void paintEvent(QPaintEvent*);

private:
    S_MESSAGE* mMSG;

private:
    Ui::Widget_BoardPhenixOrder *ui;
};

#endif // LIBERAL_BOARD_H
