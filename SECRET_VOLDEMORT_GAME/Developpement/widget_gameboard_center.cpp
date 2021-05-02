#include "widget_gameboard_center.h"
#include "ui_widget_gameboard_center.h"

#define NB_STEP_PILE_ANIMATION 15

Widget_Gameboard_Center::Widget_Gameboard_Center(QWidget* parent, S_MESSAGE* MSG):
    QWidget(parent),
    mMSG(MSG),
    mIsPileAnimated(false),
    mHover(false),
    mPileAnimationStep(NB_STEP_PILE_ANIMATION),
    ui(new Ui::Widget_Gameboard_Center)
{
    ui->setupUi(this);

    wFacisteBoard = new Widget_BoardDeathEaters(this, mMSG);
    ui->layoutFacisteBoard->addWidget(wFacisteBoard, 1);

    wLiberalBoard = new Widget_BoardPhenixOrder(this, mMSG);
    ui->layoutLiberalBoard->addWidget(wLiberalBoard, 1);

    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(animate()));

    wPile = new Widget_Card(this);
    connect(wPile, SIGNAL(sig_hoverIn(int)), this, SLOT(hoverIn(int)));
    connect(wPile, SIGNAL(sig_hoverOut(int)), this, SLOT(hoverOut(int)));
    connect(wPile, SIGNAL(clicked(bool)), this, SLOT(onDraw()));
    ui->layout_pile->addWidget(wPile, Qt::AlignCenter);
    wPile->setEnabled(false);

    wPile->setStyleSheet("Widget_Card{background-color:rgba(0,0,0,0);}");
}

Widget_Gameboard_Center::~Widget_Gameboard_Center()
{
    delete ui;
}

void Widget_Gameboard_Center::initWidget()
{
    stopPileAnimation();
    wPile->setEnabled(false);
}

void Widget_Gameboard_Center::updateWidget()
{
    wPile->update();
    wLiberalBoard->update();
    wFacisteBoard->update();
}

void Widget_Gameboard_Center::startPileAnimation()
{
    wPile->setEnabled(true);
    mIsPileAnimated = true;
    mPileAnimationStep = NB_STEP_PILE_ANIMATION;
    ui->layout_pile->update();
    mTimer->start(15);
}

void Widget_Gameboard_Center::animate()
{
    mPileAnimationStep--;

    if(mPileAnimationStep == -1)
    {
        mPileAnimationStep = NB_STEP_PILE_ANIMATION;
    }

    update();
}

void Widget_Gameboard_Center::stopPileAnimation()
{
    wPile->setEnabled(false);
    mTimer->stop();
    mIsPileAnimated = false;
}

void Widget_Gameboard_Center::hoverIn(int)
{
    mHover = true;
    update();
}

void Widget_Gameboard_Center::hoverOut(int)
{
    mHover = false;
    update();
}

void Widget_Gameboard_Center::onDraw()
{
    stopPileAnimation();
    emit sig_draw();
}

void Widget_Gameboard_Center::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap::fromImage(QImage(":/images/background.png")));

    // Draw draw card (ratio 1.5).

    QRect roleArea, roleAreaReduced;
    roleAreaReduced.setRect(wPile->x() + 10, wPile->y() + 5, wPile->width() - 20, wPile->height() - 10);
    if(wPile->height() / wPile->width() < 1.5)
    {
        // The size will scale on height.
        roleArea.setRect(roleAreaReduced.x(),
                         roleAreaReduced.y(),
                         static_cast<int>(roleAreaReduced.height()/1.5),
                         roleAreaReduced.height());
    }
    else
    {
        // The size will scale on width.
        roleArea.setRect(roleAreaReduced.x(),
                         roleAreaReduced.y() + (roleAreaReduced.height()-static_cast<int>(roleAreaReduced.width()*1.5))/2,
                         roleAreaReduced.width(),
                         static_cast<int>(roleAreaReduced.width()*1.5));
    }

    // Animation in progress.
    if((mIsPileAnimated == true) && (mHover == false))
    {
        qreal scaleReduction = static_cast<qreal>(mPileAnimationStep)/static_cast<qreal>(NB_STEP_PILE_ANIMATION);
        roleArea.setRect(roleArea.x() + ((roleArea.width()/3)*scaleReduction)/2,
                         roleArea.y() + ((roleArea.height()/3)*scaleReduction)/2,
                         roleArea.width() - (roleArea.width()/3)*scaleReduction,
                         roleArea.height() - (roleArea.height()/3)*scaleReduction);
    }

    // Pile Drawing.
    painter.drawPixmap(roleArea, QPixmap(QString(":/images/%1").arg(RES_LAW_BACK)));

    // When hover is triggered and player is allowed to draw.
    if(mHover == true && wPile->isEnabled())
    {
        painter.drawPixmap(QRect(roleArea.x() + roleArea.width()/10, roleArea.y() - roleArea.width()/10, roleArea.width(), roleArea.height()),
                           QPixmap(QString(":/images/%1").arg(RES_LAW_BACK)));
        painter.drawPixmap(QRect(roleArea.x() + 2*roleArea.width()/10, roleArea.y() - 2*roleArea.width()/10, roleArea.width(), roleArea.height()),
                           QPixmap(QString(":/images/%1").arg(RES_LAW_BACK)));
    }

    if(mMSG == nullptr)
        return;

    if(mHover == false || !wPile->isEnabled())
    {
        QRect textField(roleArea.x() + roleArea.width()/3, roleArea.y() + roleArea.height()*10/12, roleArea.width()/3, roleArea.height()/12);
        painter.setFont(QFont("", roleArea.height()/30));

        painter.setOpacity(0.05);
        painter.setBrush(QBrush(QColor(250, 250, 250)));
        painter.drawRect(textField);

        painter.setOpacity(0.6);
        painter.setPen(QPen(QBrush(QColor(250, 250, 250)), 1));
        painter.drawText(textField, Qt::AlignCenter, QString("%1").arg(mMSG->gameStatus.pile.size()));
        mMSG->gameStatus.pile.size();
    }
}
