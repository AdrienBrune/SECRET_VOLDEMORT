#include "widget_popupmessage.h"
#include "ui_widget_popupmessage.h"
#include <QTimer>
#include <QPainter>

#define MAX_STEP    10
#define MIN_STEP    0

Widget_PopupMessage::Widget_PopupMessage(QWidget *parent) :
    QWidget(parent),
    mMessage(""),
    mAnimation(MIN_STEP),
    ui(new Ui::Widget_PopupMessage)
{
    ui->setupUi(this);

    t_animation = new QTimer(this);
    connect(t_animation, SIGNAL(timeout()), this, SLOT(onAnimate()));
}

Widget_PopupMessage::~Widget_PopupMessage()
{
    delete ui;
}

void Widget_PopupMessage::setText(QString message)
{
    mMessage = message;
    update();
}

void Widget_PopupMessage::startAnimation()
{
    mAnimation = MIN_STEP;
    t_animation->start(10);
    emit sig_playSound(SoundManager::E_SOUND::ready);
}

void Widget_PopupMessage::onAnimate()
{
    if(++mAnimation > MAX_STEP)
    {
        mAnimation = MAX_STEP;
        stopAnimation();
    }
    update();
}


void Widget_PopupMessage::stopAnimation()
{
    t_animation->stop();
    mAnimation = MAX_STEP;

    QTimer::singleShot(1500, [&](){ hide(); });
}

void Widget_PopupMessage::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setOpacity(static_cast<qreal>(mAnimation) / (static_cast<qreal>(MAX_STEP)));

    // Draw banner.
    painter.setBrush(QBrush(QColor(0x43,0x43,0x43)));
    painter.drawRect(0, height()/3, width(), height()/3);

    QRect bounding;
    QFont fontMask1("Germania", height()/10);
    QFontMetrics toolMask1(fontMask1);
    QFont fontMask2("Germania", height()/10);
    QFontMetrics toolMask2(fontMask2);
    QFont fontMask3("Germania", height()/10);
    QFontMetrics toolMask3(fontMask3);

    // Draw first test mask.
    painter.setPen(Qt::black);
    painter.setFont(fontMask1);
    bounding = toolMask1.boundingRect(mMessage);

    QRect textMask1((width() + static_cast<int>(bounding.width()*0.05) - static_cast<int>(bounding.width()*1.05))/2,
                    (height() - static_cast<int>(bounding.height()*1.05))/2,
                    static_cast<int>(bounding.width()*1.05),
                    static_cast<int>(bounding.height()*1.05));
    painter.drawText(textMask1, mMessage);

    // Draw second text mask.
    painter.setPen(Qt::white);
    painter.setFont(fontMask2);
    bounding = toolMask2.boundingRect(mMessage);

    QRect textMask2(4 + (width() + static_cast<int>(bounding.width()*0.05) - static_cast<int>(bounding.width()*1.05))/2,
                    (height() - static_cast<int>(bounding.height()*1.05))/2,
                    static_cast<int>(bounding.width()*1.05),
                    static_cast<int>(bounding.height()*1.05));
    painter.drawText(textMask2, mMessage);

}

