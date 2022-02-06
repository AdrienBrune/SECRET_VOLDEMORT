#ifndef WIDGET_SCREENWAITINGGAMEH_H
#define WIDGET_SCREENWAITINGGAMEH_H

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QPainter>
#include <QTimer>

namespace Ui {
class Widget_ScreenWaitingGame;
}

class QVideoWidgetCustom : public QVideoWidget
{
    Q_OBJECT

public:
    explicit QVideoWidgetCustom(QWidget *parent = nullptr):QVideoWidget(parent){timer = new QTimer(this); timer->setInterval(10);timer->setSingleShot(true);}
    ~QVideoWidgetCustom(){}

public:
    void resizeIn(QWidget * parent)
    {
        int newWidth = (parent->height()-70)*16.0/9.0;
        int newHeight = parent->width()*9.0/16.0;
        if(newWidth < parent->width())
        {
            setMaximumWidth(newWidth);
            setMaximumHeight(100000);

        }
        else
        {
            setMaximumHeight(newHeight);
            setMaximumWidth(100000);

        }
        if(parent)
        {
            p = parent;
            if(timer->isActive())
                timer->stop();
            timer->singleShot(10, this, [&]{moveTo(); p->update();});
        }
    }
private:
    void moveTo(){ move((p->width()-width())/2, ((p->height()-70)-height())/2); }

private:
    QTimer* timer;
    QWidget* p;
};









class Widget_ScreenWaitingGame : public QWidget
{
    Q_OBJECT

private:
    typedef enum
    {
        WaitingGame = 0,
        Tutorial = 1

    }E_CURRENT_SCREEN;

public:
    explicit Widget_ScreenWaitingGame(QWidget *parent = nullptr);
    ~Widget_ScreenWaitingGame();

private slots:
    void on_buttonVideo_clicked();
    void on_buttonSwitchPage_clicked();
    void onUpdateSliderVideo(qint64);
    void onSliderPressed();
    void onSliderReleased();
    void onSliderMoved(int);

public:
    void startVideo();
    void stopVideo();
    const QString getPathVideo()const;
    const QString getPathTutorial()const;

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);

private:
    QMediaPlayer* mPlayerTutorial;
    QVideoWidgetCustom* mVideoTutorial;

    QMediaPlayer* mPlayer;
    QMediaPlaylist* mPlaylist;
    QVideoWidgetCustom* mVideo;

    quint8 mActivePage;

private:
    Ui::Widget_ScreenWaitingGame *ui;
};

#endif // WIDGET_SCREENWAITINGGAMEH_H
