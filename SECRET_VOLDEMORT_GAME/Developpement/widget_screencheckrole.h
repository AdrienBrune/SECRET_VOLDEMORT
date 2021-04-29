#ifndef WIDGET_SEE_PLAYER_ROLE_H
#define WIDGET_SEE_PLAYER_ROLE_H

#include <QWidget>
#include <QPainter>
#include "data.h"
#include "soundmanager.h"

namespace Ui {
class Widget_ScreenCheckRole;
}

class Widget_ScreenCheckRole : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_ScreenCheckRole(QWidget *parent = nullptr, S_MESSAGE* MSG = nullptr);
    ~Widget_ScreenCheckRole();

signals:
    void sig_playSound(SoundManager::E_SOUND);

protected:
    void paintEvent(QPaintEvent*);

protected:
    S_MESSAGE* mMSG;

private:
    Ui::Widget_ScreenCheckRole *ui;
};

#endif // WIDGET_SEE_PLAYER_ROLE_H
