#ifndef WIDGET_POWER_ANIMATION_H
#define WIDGET_POWER_ANIMATION_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include "data.h"
#include "soundmanager.h"

namespace Ui {
class Widget_ScreenPowerUnlocked;
}

class Widget_ScreenPowerUnlocked : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_ScreenPowerUnlocked(QWidget* parent = nullptr);
    ~Widget_ScreenPowerUnlocked();

signals:
    void sig_startUsingPower(E_POWER);
    void sig_playSound(SoundManager::E_SOUND);

private slots:
    void animate();

public:
    void startPowerAnimation(E_POWER);
    void stopPowerAnimation();

private:
    QRect getRect(QRect, int);

protected:
    void paintEvent(QPaintEvent*);

private:
    bool mAnimate;
    int mCount;
    E_POWER mPower;
    int mCurrentTrace;

    QTimer * tAnimation;

private:
    Ui::Widget_ScreenPowerUnlocked *ui;
};

#endif // WIDGET_POWER_ANIMATION_H
