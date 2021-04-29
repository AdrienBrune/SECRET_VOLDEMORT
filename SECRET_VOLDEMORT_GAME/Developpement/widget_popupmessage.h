#ifndef WIDGET_POPUPMESSAGE_H
#define WIDGET_POPUPMESSAGE_H

#include <QWidget>
#include "soundmanager.h"

namespace Ui {
class Widget_PopupMessage;
}

class Widget_PopupMessage : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_PopupMessage(QWidget *parent = nullptr);
    ~Widget_PopupMessage();

signals:
    void sig_playSound(SoundManager::E_SOUND);

private slots:
    void onAnimate();

public:
    void setText(QString message);
    void startAnimation();
    void stopAnimation();

protected:
    void paintEvent(QPaintEvent*);

private:
    QString mMessage;

    QTimer* t_animation;
    quint8 mAnimation;

private:
    Ui::Widget_PopupMessage *ui;
};

#endif // WIDGET_POPUPMESSAGE_H
