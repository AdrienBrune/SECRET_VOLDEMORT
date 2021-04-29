#include "widget_screenpowerunlocked.h"
#include "ui_widget_screenpowerunlocked.h"

#define MAX_COUNT STEP_6
#define STEP_1 5
#define STEP_2 10
#define STEP_3 15
#define STEP_4 30
#define STEP_5 40
#define STEP_6 100

#define FULL_CIRCLE 400
#define START_ANGLE -30

Widget_ScreenPowerUnlocked::Widget_ScreenPowerUnlocked(QWidget* parent):
    QWidget(parent),
    mAnimate(false),
    mCount(0),
    mPower(E_POWER::noPower),
    mCurrentTrace(0),
    ui(new Ui::Widget_ScreenPowerUnlocked)
{
    ui->setupUi(this);

    tAnimation = new QTimer(this);
    connect(tAnimation, SIGNAL(timeout()), this, SLOT(animate()));
}

void Widget_ScreenPowerUnlocked::startPowerAnimation(E_POWER power)
{
    mPower = power;
    mAnimate = true;
    mCount = 0;
    mCurrentTrace = 0;
    tAnimation->start(10);
}

void Widget_ScreenPowerUnlocked::animate()
{
    mCount++;

    if(mCount == STEP_5)
    {
        emit sig_playSound(SoundManager::E_SOUND::unlock);
    }

    if(mCount == MAX_COUNT)
    {
        stopPowerAnimation();
    }

    update();
}

void Widget_ScreenPowerUnlocked::stopPowerAnimation()
{
    tAnimation->stop();
    mAnimate = false;
    emit sig_startUsingPower(mPower);
}

void Widget_ScreenPowerUnlocked::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap::fromImage(QImage(":/images/background_motif.png")));

    if(mAnimate == true)
    {
        QString styleSheet;

        switch(mPower)
        {
            case E_POWER::kill:
                styleSheet = ":/images/power_kill.png";
                break;

            case E_POWER::checkPile:
                styleSheet =  ":/images/power_pile.png";
                break;

            case E_POWER::checkRole:
                styleSheet =  ":/images/power_foreseen.png";
                break;

            case E_POWER::chooseMinister:
                styleSheet =  ":/images/power_select.png";
                break;

            case E_POWER::voteToKill:
                styleSheet = ":/images/power_kill.png";
                break;

            default:
                qDebug() << "erreur pouvoir non reconnu";
                break;
        }

        QRect central_fig(this->width()/2 - this->height()/4, this->height()/4, this->height()/2, this->height()/2);
        int margins = 5;
        QRect rect_circle_0(central_fig.x()+margins, central_fig.y()+margins, central_fig.width()-2*margins, central_fig.height()-2*margins);
        int circle_0_width = static_cast<int>(0.14*central_fig.width());
        margins = static_cast<int>((0.8*0.07)*central_fig.width());
        QRect rect_circle_1(rect_circle_0.x()+margins, rect_circle_0.y()+margins, rect_circle_0.width()-2*margins, rect_circle_0.height()-2*margins);

        QPen pen_circle_0_black;
        pen_circle_0_black.setCapStyle(Qt::FlatCap);
        pen_circle_0_black.setWidth(circle_0_width);
        pen_circle_0_black.setColor(QColor(0x11,0x11,0x11));

        QPen pen_circle_0_white;
        pen_circle_0_white.setCapStyle(Qt::FlatCap);
        pen_circle_0_white.setWidth(circle_0_width);
        pen_circle_0_white.setColor(QColor(0xFE,0xFE,0xFE));

        QPen pen_circle_grey;
        pen_circle_grey.setCapStyle(Qt::FlatCap);
        pen_circle_grey.setWidth(circle_0_width);
        pen_circle_grey.setColor(QColor(0x2C,0x2C,0x2C));

        QPen pen_none;
        pen_none.setCapStyle(Qt::FlatCap);
        pen_none.setWidth(1);
        pen_none.setColor(QColor(0x69,0x69,0x69));

/*      /// DEBUG
        if(animation == true){

            QBrush fill_red(Qt::SolidPattern);
            fill_red.setColor(QColor("#FF0000"));
            QBrush fill_black(Qt::SolidPattern);
            fill_black.setColor(QColor("#3C3C3C"));
            QBrush fill_3(Qt::SolidPattern);
            fill_3.setColor(QColor(Qt::magenta));

            painter.setPen(pen_none);

            painter.setBrush(fill_red);
            painter.drawRect(central_fig);

            painter.setBrush(fill_3);
            painter.drawRect(rect_circle_0);

            //int w =40;
            pen_circle_0_black.setWidth(circle_0_width);
            painter.setPen(pen_circle_0_black);
            painter.drawArc(getRect(rect_circle_0,circle_0_width), 0, 360*16);

            painter.drawPixmap(central_fig, QPixmap::fromImage(QImage(styleSheet)));
            painter.drawPixmap(central_fig, QPixmap::fromImage(QImage(":/images/unlock.png")));

            //painter.setBrush(fill_red);
            //painter.drawRect(rect_circle_1);

            //pen_circle_0_black.setWidth(width_round);
            //painter.setPen(pen_circle_0_black);
            //painter.drawArc(rect_main_circle, 0, 360*16);

            //pen_circle_0_white.setWidth(20);
            //painter.setPen(pen_circle_0_white);
            //painter.drawArc(circle_1, 0, 360*16);

            return;
        }
*/

        painter.setPen(pen_circle_0_black);
        painter.setOpacity(1);

        int trace = (FULL_CIRCLE/8)*16;

        if(mCount < STEP_1){ // appear

            painter.setOpacity(static_cast<double>(mCount/5.0));
            painter.drawPixmap(central_fig, QPixmap::fromImage(QImage(styleSheet)));
            painter.drawPixmap(central_fig, QPixmap::fromImage(QImage(":/images/lock.png")));

        }else if(mCount < STEP_2){ // slow inc

            //int counter = count - STEP_1;
            int step = ((FULL_CIRCLE/8)*16) / (STEP_2 - STEP_1);
            mCurrentTrace += step;
            painter.drawArc(getRect(rect_circle_0, circle_0_width), START_ANGLE*16, mCurrentTrace);
            if(mCurrentTrace > trace)
            {
                painter.setPen(pen_circle_grey);
                painter.drawArc(getRect(rect_circle_0, circle_0_width), START_ANGLE*16, mCurrentTrace-trace);
                painter.setPen(pen_circle_0_black);
                painter.drawArc(getRect(rect_circle_1, circle_0_width), START_ANGLE*16, mCurrentTrace-trace);
            }
            painter.drawPixmap(central_fig, QPixmap::fromImage(QImage(styleSheet)));
            painter.drawPixmap(central_fig, QPixmap::fromImage(QImage(":/images/lock.png")));

        }else if(mCount < STEP_3){ // slow++ inc

            //int counter = count - STEP_2;
            int step = ((FULL_CIRCLE/4)*16) / (STEP_3 - STEP_2);
            mCurrentTrace += step;
            painter.drawArc(getRect(rect_circle_0, circle_0_width), START_ANGLE*16, mCurrentTrace);
            if(mCurrentTrace > trace)
            {
                painter.setPen(pen_circle_grey);
                painter.drawArc(getRect(rect_circle_0, circle_0_width), START_ANGLE*16, mCurrentTrace-trace);
                painter.setPen(pen_circle_0_black);
                painter.drawArc(getRect(rect_circle_1, circle_0_width), START_ANGLE*16, mCurrentTrace-trace);
            }
            painter.drawPixmap(central_fig, QPixmap::fromImage(QImage(styleSheet)));
            painter.drawPixmap(central_fig, QPixmap::fromImage(QImage(":/images/lock.png")));

        }else if(mCount < STEP_4){ // end inc

            //int counter = count - STEP_3;
            int step = ((FULL_CIRCLE*5/8)*16) / (STEP_4 - STEP_3);
            mCurrentTrace += step;
            painter.drawArc(getRect(rect_circle_0, circle_0_width), START_ANGLE*16, mCurrentTrace);
            if(mCurrentTrace > trace)
            {
                painter.setPen(pen_circle_grey);
                painter.drawArc(getRect(rect_circle_0, circle_0_width), START_ANGLE*16, mCurrentTrace-trace);
                painter.setPen(pen_circle_0_black);
                painter.drawArc(getRect(rect_circle_1, circle_0_width), START_ANGLE*16, mCurrentTrace-trace);
            }
            painter.drawPixmap(central_fig, QPixmap::fromImage(QImage(styleSheet)));
            painter.drawPixmap(central_fig, QPixmap::fromImage(QImage(":/images/lock_3.png")));

        }else if(mCount < STEP_5){ // tempo

            int counter = mCount - STEP_4;
            int step = static_cast<int>(((FULL_CIRCLE*16) / (STEP_5 - STEP_4))*1.1);
            mCurrentTrace += step;
            painter.setPen(pen_circle_grey);
            painter.drawArc(getRect(rect_circle_0, circle_0_width), START_ANGLE*16, FULL_CIRCLE*16);
            painter.setPen(pen_circle_0_black);
            painter.drawArc(getRect(rect_circle_1, circle_0_width), START_ANGLE*16, FULL_CIRCLE*16);
            painter.drawArc(getRect(rect_circle_0, circle_0_width), START_ANGLE*16, counter*step);
            painter.drawPixmap(central_fig, QPixmap::fromImage(QImage(styleSheet)));
            painter.drawPixmap(central_fig, QPixmap::fromImage(QImage(":/images/lock_2.png")));

        }else if(mCount <= STEP_6){ // last screen + evo

            //int counter = count - STEP_5;
            QRect halo(central_fig.x()-central_fig.width()/4, central_fig.y()-central_fig.height()/4, central_fig.width()+central_fig.width()/2, central_fig.height()+central_fig.height()/2);

            painter.drawPixmap(halo, QPixmap::fromImage(QImage(":/images/halo_2.png")));
            painter.setPen(pen_circle_0_black);
            painter.drawArc(getRect(rect_circle_0, circle_0_width), START_ANGLE*16, 360*16);
            painter.drawPixmap(central_fig, QPixmap::fromImage(QImage(styleSheet)));
            painter.drawPixmap(central_fig, QPixmap::fromImage(QImage(":/images/unlock.png")));

        }
    }
}

QRect Widget_ScreenPowerUnlocked::getRect(QRect r, int w)
{
    return QRect(r.x()+w/2, r.y()+w/2, r.width()-w, r.height()-w);
}


Widget_ScreenPowerUnlocked::~Widget_ScreenPowerUnlocked()
{
    delete ui;
}
