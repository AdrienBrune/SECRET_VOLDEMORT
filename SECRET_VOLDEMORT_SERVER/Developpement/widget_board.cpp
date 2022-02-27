#include "widget_board.h"
#include "ui_widget_board.h"

Widget_Board::Widget_Board(QWidget *parent, S_GAME_STATUS* game, E_BOARD_TYPE type) :
    QWidget(parent),
    mGame(game),
    mType(type),
    ui(new Ui::Widget_Board)
{
    ui->setupUi(this);
}

Widget_Board::~Widget_Board()
{
    delete ui;
}

void Widget_Board::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    double horizontalMargin, verticalMargin, sizeWidth, sizeHeight, spacing;

    /* Get canvas */
    double ratio = 3.40;
    QRect board;
    if(height() > width()/ratio) // Oversize from height -> Reduce height
    {
        board.setRect(0, (height()-width()/ratio)/2, width(), width()/ratio);
    }
    else // Oversize from width -> Reduce width
    {
        board.setRect((width()-height()*ratio)/2, 0, height()*ratio, height());
    }
    if(mType == E_BOARD_TYPE::Mangemort)
    {
        horizontalMargin = board.x() + 0.070 * board.width(),
        verticalMargin = board.y() + 0.2254 * board.height(),
        sizeWidth = 0.119 * board.width(),
        sizeHeight = 0.553 * board.height(),
        spacing = 0.03125 * board.width();
    }
    else
    {
        horizontalMargin = board.x() +  0.146 * board.width();
        verticalMargin = board.y() + 0.225 * board.height();
        sizeWidth = 0.119 * board.width();
        sizeHeight = 0.556 * board.height();
        spacing = 0.0332 * board.width();
    }

    /* Get number of players */
    uint8_t numberPlayerInGame = 0;
    for(S_PLAYER player : mGame->players)
    {
        if(player.status != E_PLAYER_STATUS::notPlaying)
        {
            numberPlayerInGame++;
        }
    }

    /* Draw board */
    if(mType == E_BOARD_TYPE::Mangemort)
    {
        switch(numberPlayerInGame)
        {
            default:
            case 0 ... 6:
                painter.drawPixmap(board, QPixmap(":/images/Plateau_Mangemort_56.png"));
                break;

            case 7 ... 8:
                painter.drawPixmap(board, QPixmap(":/images/Plateau_Mangemort_78.png"));
                break;

            case 9 ... 10:
                painter.drawPixmap(board, QPixmap(":/images/Plateau_Mangemort_910.png"));
                break;
        }
    }
    else
    {
        painter.drawPixmap(board, QPixmap(":/images/Plateau_Phenix.png"));
    }

    /* Draw card put */
    if(mType == E_BOARD_TYPE::Mangemort)
    {
        if(mGame->board.boardFaciste > 0)
        {
            painter.drawPixmap(horizontalMargin, verticalMargin, sizeWidth, sizeHeight, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_DEATHEATERS))));
        }
        if(mGame->board.boardFaciste > 1)
        {
            painter.drawPixmap(horizontalMargin + sizeWidth + spacing, verticalMargin, sizeWidth, sizeHeight, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_DEATHEATERS))));
        }
        if(mGame->board.boardFaciste > 2)
        {
            painter.drawPixmap(horizontalMargin + 2 * (sizeWidth + spacing), verticalMargin, sizeWidth, sizeHeight, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_DEATHEATERS))));
        }
        if(mGame->board.boardFaciste > 3)
        {
            painter.drawPixmap(horizontalMargin + 3 * (sizeWidth + spacing), verticalMargin, sizeWidth, sizeHeight, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_DEATHEATERS))));
        }
        if(mGame->board.boardFaciste > 4)
        {
            painter.drawPixmap(horizontalMargin + 4 * (sizeWidth + spacing), verticalMargin, sizeWidth, sizeHeight, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_DEATHEATERS))));
        }
        if(mGame->board.boardFaciste > 5)
        {
            painter.drawPixmap(horizontalMargin + 5 * (sizeWidth + spacing), verticalMargin, sizeWidth, sizeHeight, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_DEATHEATERS))));
        }
    }
    else
    {
        if(mGame->board.boardLiberal > 0)
        {
            painter.drawPixmap(horizontalMargin, verticalMargin, sizeWidth, sizeHeight, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_PHENIXORDER))));
        }
        if(mGame->board.boardLiberal > 1)
        {
            painter.drawPixmap(horizontalMargin + sizeWidth + spacing, verticalMargin, sizeWidth, sizeHeight, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_PHENIXORDER))));
        }
        if(mGame->board.boardLiberal > 2)
        {
            painter.drawPixmap(horizontalMargin + 2 * (sizeWidth + spacing), verticalMargin, sizeWidth, sizeHeight, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_PHENIXORDER))));
        }
        if(mGame->board.boardLiberal > 3)
        {
            painter.drawPixmap(horizontalMargin + 3 * (sizeWidth + spacing), verticalMargin, sizeWidth, sizeHeight, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_PHENIXORDER))));
        }
        if(mGame->board.boardLiberal > 4)
        {
            painter.drawPixmap(horizontalMargin + 4 * (sizeWidth + spacing), verticalMargin, sizeWidth, sizeHeight, QPixmap::fromImage(QImage(QString(":/images/%1").arg(RES_LAW_PHENIXORDER))));
        }
    }
}
