#ifndef WIDGET_SCREENENDGAME_H
#define WIDGET_SCREENENDGAME_H

#include <QWidget>
#include <QPainter>
#include "data.h"
#include "soundmanager.h"

class Widget_ScreenEndGame : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_ScreenEndGame(QWidget* parent = nullptr, S_MESSAGE* MSG = nullptr);
    ~Widget_ScreenEndGame();

signals:
    void sig_playSound(SoundManager::E_SOUND);

protected:
    void paintEvent(QPaintEvent*);

private:
    S_MESSAGE* mMSG;

};

#endif // WIDGET_SCREENENDGAME_H
