#ifndef WIDGET_SCREENWAITINGSTART_H
#define WIDGET_SCREENWAITINGSTART_H

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QPainter>
#include <QTimer>

class QVideoWidgetCustom : public QVideoWidget
{
    Q_OBJECT

public:
    explicit QVideoWidgetCustom(QWidget *parent = nullptr):QVideoWidget(parent){timer = new QTimer(this); timer->setInterval(10);timer->setSingleShot(true);}
    ~QVideoWidgetCustom(){}

public:
    void resizeIn(QWidget * parent)
    {
        int newWidth = parent->height()*16.0/9.0;
        int newHeigth = parent->width()*9.0/16.0;
        if(newWidth < parent->width())
        {
            setMaximumWidth(newWidth);
            setMaximumHeight(100000);

        }
        else
        {
            setMaximumHeight(newHeigth);
            setMaximumWidth(100000);

        }
        if(parent)
        {
            p = parent;
            if(timer->isActive())
                timer->stop();
            timer->singleShot(10, this, [&]{moveTo();});
        }
    }
private:
    void moveTo(){ move((p->width()-width())/2, (p->height()-height())/2); }

private:
    QTimer* timer;
    QWidget* p;
};

class Widget_ScreenWaitingStart : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_ScreenWaitingStart(QWidget *parent = nullptr);
    ~Widget_ScreenWaitingStart();

public:
    void startVideo();
    void stopVideo();

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);

private:
    QMediaPlayer* mPlayer;
    QMediaPlaylist* mPlaylist;
    QVideoWidgetCustom* mVideo;
};

#endif // WIDGET_SCREENWAITINGSTART_H
