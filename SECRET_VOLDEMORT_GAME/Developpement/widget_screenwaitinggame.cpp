#include "widget_screenwaitinggame.h"
#include "ui_widget_screenwaitinggame.h"

#define TUTORIAL_DURATION_SEC 168

Widget_ScreenWaitingGame::Widget_ScreenWaitingGame(QWidget *parent) :
    QWidget(parent),
    mActivePage(E_CURRENT_SCREEN::WaitingGame),
    ui(new Ui::Widget_ScreenWaitingGame)
{
    ui->setupUi(this);

    /* Tutorial settings */
    mPlayerTutorial = new QMediaPlayer(this);
    mPlayerTutorial->setMedia(QUrl(getPathTutorial()));
    mVideoTutorial = new QVideoWidgetCustom(this);
    mPlayerTutorial->setNotifyInterval(250);
    mPlayerTutorial->setVideoOutput(mVideoTutorial);
    connect(mPlayerTutorial, SIGNAL(positionChanged(qint64)), this, SLOT(onUpdateSliderVideo(qint64)));
    connect(ui->sliderVideo, SIGNAL(sliderPressed()), this, SLOT(onSliderPressed()));
    connect(ui->sliderVideo, SIGNAL(sliderReleased()), this, SLOT(onSliderReleased()));
    connect(ui->sliderVideo, SIGNAL(sliderMoved(int)), this, SLOT(onSliderMoved(int)));
    ui->layoutVideo->addWidget(mVideoTutorial);

    /* Waiting game video settings */
    mPlayer = new QMediaPlayer(this);
    mPlaylist = new QMediaPlaylist(this);
    mPlaylist->addMedia(QUrl(getPathVideo()));
    mPlaylist->setPlaybackMode(QMediaPlaylist::Loop);
    mPlayer->setPlaylist(mPlaylist);
    mPlaylist->setCurrentIndex(1);
    mVideo = new QVideoWidgetCustom(this);
    mPlayer->setVideoOutput(mVideo);
    ui->layoutVideo->addWidget(mVideo);

    mActivePage = !mActivePage;
    on_buttonSwitchPage_clicked();
}

Widget_ScreenWaitingGame::~Widget_ScreenWaitingGame()
{
    delete ui;
}

void Widget_ScreenWaitingGame::startVideo()
{
    if(mActivePage)
    {
        mPlayerTutorial->play();
        ui->buttonVideo->setChecked(false);
    }
    else
    {
        mPlayer->play();
    }
}

void Widget_ScreenWaitingGame::stopVideo()
{
    if(mActivePage)
    {
        mPlayerTutorial->stop();
        ui->buttonVideo->setChecked(true);
    }
    else
    {
        mPlayer->pause();
    }
}

const QString Widget_ScreenWaitingGame::getPathVideo()const
{
#ifdef Q_OS_WINDOWS
    return "qrc:/video/win/trailer.avi";
#endif
#ifdef Q_OS_MACOS
    return "qrc:/video/mac/trailer.mov";
#endif
}

const QString Widget_ScreenWaitingGame::getPathTutorial()const
{
#ifdef Q_OS_WINDOWS
    return "qrc:/video/win/tutorial.avi";
#endif
#ifdef Q_OS_MACOS
    return "qrc:/video/mac/tutorial.mov";
#endif
}

void Widget_ScreenWaitingGame::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(QBrush("#191919"));
    painter.drawRect(0, 0, width(), height());
}

void Widget_ScreenWaitingGame::resizeEvent(QResizeEvent*)
{
    if(mActivePage)
    {
        mVideoTutorial->resizeIn(this);
    }
    else
    {
        mVideo->resizeIn(this);
    }
}

void Widget_ScreenWaitingGame::onUpdateSliderVideo(qint64 valueMs)
{
    ui->sliderVideo->setValue((valueMs/(TUTORIAL_DURATION_SEC*1000.0))*1000);
}

void Widget_ScreenWaitingGame::onSliderPressed()
{
    disconnect(mPlayerTutorial, SIGNAL(positionChanged(qint64)), this, SLOT(onUpdateSliderVideo(qint64)));
}

void Widget_ScreenWaitingGame::onSliderReleased()
{
    connect(mPlayerTutorial, SIGNAL(positionChanged(qint64)), this, SLOT(onUpdateSliderVideo(qint64)));
}

void Widget_ScreenWaitingGame::onSliderMoved(int position)
{
    mPlayerTutorial->setPosition((position/1000.0)*(TUTORIAL_DURATION_SEC*1000));
}

void Widget_ScreenWaitingGame::on_buttonVideo_clicked()
{
    if(ui->buttonVideo->isChecked())
    {
        mPlayerTutorial->pause();
    }
    else
    {
        mPlayerTutorial->play();
    }
}

void Widget_ScreenWaitingGame::on_buttonSwitchPage_clicked()
{
    stopVideo();
    mActivePage = !mActivePage;
    if(mActivePage)
    {
        mVideo->hide();
        mVideoTutorial->show();
        ui->sliderVideo->show();
        ui->buttonVideo->show();
        mVideoTutorial->resizeIn(this);
    }
    else
    {
        mVideoTutorial->hide();
        mVideo->show();
        ui->buttonVideo->hide();
        ui->sliderVideo->hide();
        mVideo->resizeIn(this);
    }
    startVideo();
}
