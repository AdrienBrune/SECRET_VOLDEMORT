#ifndef INFO_SCREEN_H
#define INFO_SCREEN_H

#include <QWidget>
#include <QPainter>
#include "data.h"

class Widget_FacistTeam : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_FacistTeam(QWidget *parent = nullptr, S_MESSAGE* MSG = nullptr);
    ~Widget_FacistTeam();

signals:
    void sig_hideFacistTeam();

protected:
    void paintEvent(QPaintEvent*);

private:
    S_MESSAGE* mMSG;
};

#endif // INFO_SCREEN_H
