#ifndef FASCISTE_BOARD_H
#define FASCISTE_BOARD_H

#include <QWidget>
#include <QPainter>
#include "data.h"

namespace Ui {
class Widget_BoardDeathEaters;
}

class Widget_BoardDeathEaters : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_BoardDeathEaters(QWidget *parent = nullptr, S_MESSAGE* MSG = nullptr);
    ~Widget_BoardDeathEaters();

protected:
    void paintEvent(QPaintEvent *);

private:
    S_MESSAGE* mMSG;

private:
    Ui::Widget_BoardDeathEaters *ui;
};

#endif // FASCISTE_BOARD_H
