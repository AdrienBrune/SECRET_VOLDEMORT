#include "widget_gamestate.h"
#include "ui_widget_gamestate.h"

static QString states[] = {
    "Partie en attente",
    "Election du chancelier",
    "Vote pour le chancelier",
    "Vote d'une loi en cours",
    "Pouvoir en cours",
};

Widget_GameState::Widget_GameState(QWidget *parent, S_MESSAGE* MSG) :
    QWidget(parent),
    mMSG(MSG),
    ui(new Ui::Widget_GameState)
{
    ui->setupUi(this);
}

Widget_GameState::~Widget_GameState()
{
    delete ui;
}

int Widget_GameState::getIndexTextFromCommand(int command)
{
    switch(command)
    {
        case CMD_TO_PLAYER_START_GAME:
        case CMD_TO_PLAYER_ELECT_CHANCELOR:
            return 1;
        case CMD_TO_PLAYER_START_VOTE:
        case CMD_TO_PLAYER_PLAYER_VOTED:
            return 2;

        case CMD_TO_PLAYER_PRESIDENT_DRAW:
        case CMD_TO_PLAYER_CHANCELOR_DISCARD:
            return 3;

        case CMD_TO_PLAYER_PUT_LAW_ON_BOARD:
        case CMD_TO_PLAYER_VOTE_TO_KILL:
        case CMD_TO_PLAYER_INIT_COMMUNICATION:
            return 4;

        case CMD_TO_PLAYER_END_GAME:
        default:
            return 0;
    }
}

void Widget_GameState::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen_grey;
    pen_grey.setCapStyle(Qt::FlatCap);
    pen_grey.setWidth(4);
    pen_grey.setColor(QColor(0x3C,0x3C,0x3C));

    QPen pen_grey2;
    pen_grey2.setCapStyle(Qt::FlatCap);
    pen_grey2.setWidth(4);
    pen_grey2.setColor(QColor(0xC3,0xC3,0xC3));

    QPen pen_black;
    pen_black.setCapStyle(Qt::FlatCap);
    pen_black.setWidth(16);
    pen_black.setColor(QColor(0x00,0x00,0x00));
    pen_black.setDashPattern(QVector<qreal>{0.2, 0.2});

    QPen pen_black_2;
    pen_black_2.setCapStyle(Qt::FlatCap);
    pen_black_2.setWidth(4);
    pen_black_2.setColor(QColor(0x00,0x00,0x00));

    QPen pen_white;
    pen_white.setCapStyle(Qt::FlatCap);
    pen_white.setWidth(10);
    pen_white.setColor(QColor(0xFF,0xFF,0xFF));


    /// VARIABLES
    int margin_left = this->width()/12, margin_right = this->width()/12;
    int step_width = (this->width() - margin_left - margin_right)/4;
    int level = this->height()*7/10;
    int x0 = margin_left,
        x1 = margin_left +     step_width,
        x2 = margin_left + 2 * step_width,
        x3 = margin_left + 3 * step_width,
        x4 = margin_left + 4 * step_width;

    /// DRAW PROGRESSBAR

    /// LINE
    painter.setPen(pen_black);
    painter.drawLine(margin_left, level, 4*step_width + margin_left, level);
    painter.setPen(pen_white);
    painter.drawLine(margin_left, level, 4*step_width + margin_left, level);
    painter.setPen(pen_black_2);
    painter.drawLine(margin_left, level, 4*step_width + margin_left, level);


    /// CIRCLE 0
    int rayon = this->height()/8;
    pen_grey2.setWidth(rayon + 15);
    pen_black_2.setWidth(rayon + 15);
    pen_grey.setWidth(rayon);
    pen_white.setWidth(rayon + 10);

    painter.setPen(pen_black_2);
    QRectF rectangle(x0, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle);

    painter.setPen(pen_white);
    QRectF rectangle1(x0, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle1);

    painter.setPen(pen_grey);
    QRectF rectangle2(x0, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle2);

    /// CIRCLE 1
    painter.setPen(pen_black_2);
    rectangle.setRect(x1, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle);

    painter.setPen(pen_white);
    rectangle1.setRect(x1, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle1);

    painter.setPen(pen_grey);
    rectangle2.setRect(x1, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle2);

    /// CIRCLE 2
    painter.setPen(pen_black_2);
    rectangle.setRect(x2, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle);

    painter.setPen(pen_white);
    rectangle1.setRect(x2, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle1);

    pen_grey.setWidth(rayon);
    painter.setPen(pen_grey);
    rectangle2.setRect(x2, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle2);

    /// CIRCLE 3
    painter.setPen(pen_black_2);
    rectangle.setRect(x3, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle);

    painter.setPen(pen_white);
    rectangle1.setRect(x3, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle1);

    pen_grey.setWidth(rayon);
    painter.setPen(pen_grey);
    rectangle2.setRect(x3, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle2);

    /// CIRCLE 4
    painter.setPen(pen_black_2);
    rectangle.setRect(x4, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle);

    painter.setPen(pen_white);
    rectangle1.setRect(x4, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle1);

    pen_grey.setWidth(rayon);
    painter.setPen(pen_grey);
    rectangle2.setRect(x4, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle2);

    /// Current Step
    int step = getIndexTextFromCommand(mMSG->command);

    int x = margin_left + step_width * step;
    pen_grey2.setWidth(rayon + 30);
    pen_grey.setWidth(rayon + 15);
    pen_white.setWidth(rayon + 20);
    pen_black_2.setWidth(rayon + 30);
    pen_black_2.setColor(QColor(0x3C,0x3C,0x3C));

    painter.setPen(pen_black_2);
    rectangle.setRect(x, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle);

    painter.setPen(pen_white);
    rectangle1.setRect(x, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle1);

    pen_black_2.setWidth(rayon + 15);

    painter.setPen(pen_black_2);
    rectangle2.setRect(x, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle2);

    pen_white.setWidth(rayon + 8);

    painter.setPen(pen_white);
    QRect rectangle3(x, level - rayon/2, rayon, rayon);
    painter.drawEllipse(rectangle3);

    painter.drawPoint(QPoint(x + rayon/2, level));

    /// SET TEXT
    QFont font("High Tower Text");
    font.setPointSize(8);
    painter.setFont(font);
    QFontMetrics tool(painter.font());
    QRect bounding = tool.boundingRect(states[step]);
    QRect text(x - bounding.width()/2, this->height()/8, bounding.width(), bounding.height());
    painter.setPen(pen_white);
    /*
    QRect text = QRect(x - step_width/2,
                       this->height()/8,
                       step_width,
                       this->height()/3);
    */

    painter.drawText(text, Qt::AlignCenter, states[step]);
}
