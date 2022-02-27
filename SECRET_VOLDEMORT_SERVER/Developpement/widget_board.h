#ifndef WIDGET_BOARD_H
#define WIDGET_BOARD_H

#include <QWidget>
#include <QPainter>

#include "types.h"

namespace Ui {
class Widget_Board;
}

class Widget_Board : public QWidget
{
    Q_OBJECT

public:
    enum E_BOARD_TYPE{
        Mangemort,
        PhenixOrder
    };

public:
    explicit Widget_Board(QWidget *parent = nullptr, S_GAME_STATUS* game = nullptr, E_BOARD_TYPE type = E_BOARD_TYPE::Mangemort);
    ~Widget_Board();

protected:
    void paintEvent(QPaintEvent*);

private:
    S_GAME_STATUS* mGame;
    E_BOARD_TYPE mType;

private:
    Ui::Widget_Board *ui;
};

#endif // WIDGET_BOARD_H
