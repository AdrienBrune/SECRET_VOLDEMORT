#include "widget_gameboard_bot.h"
#include "ui_widget_gameboard_bot.h"

Widget_Gameboard_Bot::Widget_Gameboard_Bot(QWidget* parent, S_MESSAGE* MSG):
    QWidget(parent),
    mMSG(MSG),
    ui(new Ui::Widget_Gameboard_Bot)
{
    ui->setupUi(this);

    wGameState = new Widget_GameState(this, mMSG);
    ui->layout_game_state->addWidget(wGameState);
    wGameState->hide();

    ui->role->hide();
}

Widget_Gameboard_Bot::~Widget_Gameboard_Bot()
{
    delete ui;
}

void Widget_Gameboard_Bot::initWidget()
{
    ui->role->hide();

    if(mMSG->gameStatus.players.size() == 0)
        return;

    switch(mMSG->gameStatus.players[mMSG->identifier].electionRole)
    {
        case E_ELECTION_ROLE::minister:
            ui->role->setStyleSheet(QString("QLabel{border-image:url(:/images/%1);}").arg(RES_MINISTER));
            ui->role->setFixedHeight(ui->role->width()/3);
            ui->role->show();
            break;

        case E_ELECTION_ROLE::director:
            ui->role->setStyleSheet(QString("QLabel{border-image:url(:/images/%1);}").arg(RES_DIRECTOR));
            ui->role->setFixedHeight(ui->role->width()/3);
            ui->role->show();
            break;

        default:
            break;
    }

    update();
}

void Widget_Gameboard_Bot::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap::fromImage(QImage(":/images/border_bot.png")));

    // Draw cancel vote status.

    int penWidth = height()/30;
    QPen penSelected;
    QPen penFocus(QBrush(QColor(43, 72, 50)), penWidth, Qt::SolidLine, Qt::SquareCap, Qt::PenJoinStyle::MPenJoinStyle);
    QPen penUnfocus(QColor(19, 19, 19), penWidth, Qt::SolidLine, Qt::SquareCap, Qt::PenJoinStyle::MPenJoinStyle);
    QPen penWhite(QBrush(QColor(250, 250, 250)), penWidth, Qt::SolidLine, Qt::SquareCap, Qt::PenJoinStyle::MPenJoinStyle);
    QPen penGrey(QBrush(QColor(60, 60, 60)), penWidth, Qt::SolidLine, Qt::SquareCap, Qt::PenJoinStyle::MPenJoinStyle);
    QBrush brushSelected;
    QBrush brushFocus(QColor(250, 250, 250));
    QBrush brushUnfocus(QColor(60, 60, 60));

    QRect boundingArea(width()/ 4, (2.0/3.0)*(height()*3/5), width()/ 2, height()*2/5);
    QRect area1(boundingArea.x(), boundingArea.y(), boundingArea.width()/6, boundingArea.height());
    QRect spacer1(boundingArea.x() + boundingArea.width()/6, boundingArea.y(), boundingArea.width()/6, boundingArea.height());
    QRect area2(boundingArea.x() + 2*boundingArea.width()/6, boundingArea.y(), boundingArea.width()/6, boundingArea.height());
    QRect spacer2(boundingArea.x() + 3*boundingArea.width()/6, boundingArea.y(), boundingArea.width()/6, boundingArea.height());
    QRect area3(boundingArea.x() + 4*boundingArea.width()/6, boundingArea.y(), boundingArea.width()/6, boundingArea.height());
    QRect spacer3(boundingArea.x() + 5*boundingArea.width()/6, boundingArea.y(), boundingArea.width()/6, boundingArea.height());

    int squareX, squareY, squareW, squareH;

    // Shape left.

    squareX = area1.x() + area1.width()/4;
    squareY = area1.y();
    squareW = area1.width()/2;
    squareH = area1.height();
    QVector<QPoint> shape1back = {
        QPoint(squareX + squareW/2, squareY),
        QPoint(squareX, squareY + squareH/2),
        QPoint(squareX + squareW/2, squareY + squareH),
        QPoint(squareX + squareW, squareY + squareH/2)
    };

    squareX = area1.x() + penWidth + area1.width()/4;
    squareY = area1.y() + penWidth;
    squareW = (area1.width()/2 - 2*penWidth);
    squareH = (area1.height() - 2*penWidth);
    QVector<QPoint> shape1fore = {
        QPoint(squareX + squareW/2, squareY),
        QPoint(squareX, squareY + squareH/2),
        QPoint(squareX + squareW/2, squareY + squareH),
        QPoint(squareX + squareW, squareY + squareH/2)
    };

    // Arrow left.
    QLine arrow1[] = {
        QLine(spacer1.x() + spacer1.width()*2/5, spacer1.y() + spacer1.height()/3,
              spacer1.x() + spacer1.width()*3/5, spacer1.y() + spacer1.height()/2),
        QLine(spacer1.x() + spacer1.width()*3/5, spacer1.y() + spacer1.height()/2,
              spacer1.x() + spacer1.width()*2/5, spacer1.y() + spacer1.height()*2/3)
    };

    // Shape middle.

    squareX = area2.x() + area2.width()/4;
    squareY = area2.y();
    squareW = area2.width()/2;
    squareH = area2.height();
    QVector<QPoint> shape2back = {
        QPoint(squareX + squareW/2, squareY),
        QPoint(squareX, squareY + squareH/2),
        QPoint(squareX + squareW/2, squareY + squareH),
        QPoint(squareX + squareW, squareY + squareH/2)
    };

    squareX = area2.x() + penWidth + area2.width()/4;
    squareY = area2.y() + penWidth;
    squareW = (area2.width()/2 - 2*penWidth);
    squareH = (area2.height() - 2*penWidth);
    QVector<QPoint> shape2fore = {
        QPoint(squareX + squareW/2, squareY),
        QPoint(squareX, squareY + squareH/2),
        QPoint(squareX + squareW/2, squareY + squareH),
        QPoint(squareX + squareW, squareY + squareH/2)
    };

    // Arrow right.
    QLine arrow2[] = {
        QLine(spacer2.x() + spacer2.width()*2/5, spacer2.y() + spacer2.height()/3,
              spacer2.x() + spacer2.width()*3/5, spacer2.y() + spacer2.height()/2),
        QLine(spacer2.x() + spacer2.width()*3/5, spacer2.y() + spacer2.height()/2,
              spacer2.x() + spacer2.width()*2/5, spacer2.y() + spacer2.height()*2/3)
    };

    // Shape right.

    squareX = area3.x() + area3.width()/4;
    squareY = area3.y();
    squareW = area3.width()/2;
    squareH = area3.height();
    QVector<QPoint> shape3back = {
        QPoint(squareX + squareW/2, squareY),
        QPoint(squareX, squareY + squareH/2),
        QPoint(squareX + squareW/2, squareY + squareH),
        QPoint(squareX + squareW, squareY + squareH/2)
    };

    squareX = area3.x() + penWidth + area3.width()/4;
    squareY = area3.y() + penWidth;
    squareW = (area3.width()/2 - 2*penWidth);
    squareH = (area3.height() - 2*penWidth);
    QVector<QPoint> shape3fore = {
        QPoint(squareX + squareW/2, squareY),
        QPoint(squareX, squareY + squareH/2),
        QPoint(squareX + squareW/2, squareY + squareH),
        QPoint(squareX + squareW, squareY + squareH/2)
    };

    QPolygon stage1back(shape1back), stage1fore(shape1fore),
             stage2back(shape2back), stage2fore(shape2fore),
             stage3back(shape3back), stage3fore(shape3fore);

    // Setting text fields.

    QFont font("High Tower Text");
    font.setPointSize(height()/10);
    painter.setFont(font);
    QFontMetrics tool(font);
    QRect bounding;

    bounding = tool.boundingRect("rejeté");
    QRect textShape1(area1.x() + area1.width()/2 - bounding.width()/2, area1.y() - 1.5*bounding.height(), bounding.width(), bounding.height());
    QRect textShape2(area2.x() + area2.width()/2 - bounding.width()/2, area2.y() - 1.5*bounding.height(), bounding.width(), bounding.height());
    bounding = tool.boundingRect("Loi votée");
    QRect textShape3(area3.x() + area3.width()/2 - bounding.width()/2, area3.y() - 1.5*bounding.height(), bounding.width(), bounding.height());

    // Drawing of shapes.

    // Shape left.
    brushSelected = (mMSG->gameStatus.electionTracker >= 1) ? brushFocus : brushUnfocus;
    penSelected = (mMSG->gameStatus.electionTracker >= 1) ? penWhite : penGrey;
    painter.setBrush(brushSelected);
    painter.setPen(penSelected);
    painter.drawPolygon(stage1back);

    penSelected = (mMSG->gameStatus.electionTracker >= 1) ? penFocus : penUnfocus;
    painter.setBrush(brushSelected);
    painter.setPen(penSelected);
    painter.drawPolygon(stage1fore);

    // Shape middle.
    brushSelected = (mMSG->gameStatus.electionTracker >= 2) ? brushFocus : brushUnfocus;
    penSelected = (mMSG->gameStatus.electionTracker >= 2) ? penWhite : penGrey;
    painter.setBrush(brushSelected);
    painter.setPen(penSelected);
    painter.drawPolygon(stage2back);

    penSelected = (mMSG->gameStatus.electionTracker >= 2) ? penFocus : penUnfocus;
    painter.setBrush(brushSelected);
    painter.setPen(penSelected);
    painter.drawPolygon(stage2fore);

    //Shape right.
    brushSelected = (mMSG->gameStatus.electionTracker >= 3) ? brushFocus : brushUnfocus;
    penSelected = (mMSG->gameStatus.electionTracker >= 3) ? penWhite : penGrey;
    painter.setBrush(brushSelected);
    painter.setPen(penSelected);
    painter.drawPolygon(stage3back);

    penSelected = (mMSG->gameStatus.electionTracker >= 3) ? penFocus : penUnfocus;
    painter.setBrush(brushSelected);
    painter.setPen(penSelected);
    painter.drawPolygon(stage3fore);

    // Drawing of spacers.

    penSelected = (mMSG->gameStatus.electionTracker >= 1) ? penWhite : penGrey;
    painter.setPen(penSelected);
    painter.drawLines(arrow1, 2);

    penSelected = (mMSG->gameStatus.electionTracker >= 2) ? penWhite : penGrey;
    painter.setPen(penSelected);
    painter.drawLines(arrow2, 2);

    // Drawing of texts.

    // Text shape left.
    penSelected = (mMSG->gameStatus.electionTracker >= 1) ? penWhite : penGrey;
    painter.setPen(penSelected);
    painter.drawText(textShape1, Qt::AlignCenter, "rejeté");

    // Text shape middle.
    penSelected = (mMSG->gameStatus.electionTracker >= 2) ? penWhite : penGrey;
    painter.setPen(penSelected);
    painter.drawText(textShape2, Qt::AlignCenter, "rejeté");

    // Text shape right.
    penSelected = (mMSG->gameStatus.electionTracker >= 3) ? penWhite : penGrey;
    painter.setPen(penSelected);
    painter.drawText(textShape3, Qt::AlignCenter, "loi votée");
}

void Widget_Gameboard_Bot::hideRole()
{
    ui->role->hide();
    ui->role->setFixedHeight(ui->role->width()/3);
}

void Widget_Gameboard_Bot::updateWidget()
{
    wGameState->update();
    ui->role->setFixedHeight(ui->role->width()/3);
}

void Widget_Gameboard_Bot::resizeEvent(QResizeEvent * e)
{
    QWidget::resizeEvent(e);
    ui->role->setFixedHeight(ui->role->width()/3);
}

void Widget_Gameboard_Bot::showRole()
{
    ui->role->show();
    ui->role->setFixedHeight(ui->role->width()/3);
}


