#include "widget_screenwaitingstart.h"
#include <QVBoxLayout>
#include <QEvent>

Widget_ScreenWaitingStart::Widget_ScreenWaitingStart(QWidget *parent) :
    QWidget(parent)
{
    mPlayer = new QMediaPlayer(this);
    mPlaylist = new QMediaPlaylist(this);
    mPlaylist->addMedia(QUrl("qrc:/video/trailer.avi"));
    mPlaylist->setPlaybackMode(QMediaPlaylist::Loop);

    mPlayer->setPlaylist(mPlaylist);
    mPlaylist->setCurrentIndex(1);

    mVideo = new QVideoWidgetCustom(this);
    mPlayer->setVideoOutput(mVideo);

    QBoxLayout* layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(mVideo);
    setLayout(layout);
}

Widget_ScreenWaitingStart::~Widget_ScreenWaitingStart()
{

}

void Widget_ScreenWaitingStart::startVideo()
{
    mPlayer->play();
}

void Widget_ScreenWaitingStart::stopVideo()
{
    mPlayer->stop();
}

void Widget_ScreenWaitingStart::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(QBrush("#191919"));
    painter.drawRect(0, 0, width(), height());
}

void Widget_ScreenWaitingStart::resizeEvent(QResizeEvent*)
{
    mVideo->resizeIn(this);
}
