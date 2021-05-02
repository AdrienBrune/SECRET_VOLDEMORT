#include "widget_deatheatersteam.h"
#include <QPushButton>
#include <QVBoxLayout>

Widget_FacistTeam::Widget_FacistTeam(QWidget* parent, S_MESSAGE* MSG):
    QWidget(parent),
    mMSG(MSG)
{
    QPushButton* buttonExit = new QPushButton(this);
    buttonExit->setMinimumSize(QSize(50, 50));
    buttonExit->setMaximumSize(QSize(50, 50));
    buttonExit->setText("Ok");
    buttonExit->setStyleSheet("QPushButton{color:rgba(250,250,250,240);;background-color:rgba(0,0,0,60);border:1px solid rgba(250,250,250,50);padding:5px;}"
                              "QPushButton::hover{color:rgba(250,250,250,200);background-color:rgba(0,0,0,80);}");
    connect(buttonExit, SIGNAL(clicked()), this, SIGNAL(sig_hideFacistTeam()));

    QVBoxLayout* layout = new QVBoxLayout(this);
    setLayout(layout);
    layout->addWidget(buttonExit, Qt::AlignVCenter|Qt::AlignRight);
}

Widget_FacistTeam::~Widget_FacistTeam()
{

}

void Widget_FacistTeam::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if(mMSG->identifier == E_IDENTIFIER::ID_none)
        return;

    // Draw background.

    painter.drawPixmap(0, 0, width(), height(), QPixmap(":/images/border_bot.png"));

    if(mMSG->gameStatus.players.size() < 5)
    {
        return;
    }

    // Set area.
    QRect area(width()/10, height()/10, width()*8/10, height()*8/10);

    // Get box side size.
    int littleSide = 0;
    if(area.width() > area.height())
    {
        littleSide = area.height();
    }
    else
    {
        littleSide = area.width();
    }
    QList<QRect> facisteBoxes;

    // Get offset distance to display first player box.
    int playerToDisplay = 0, offset = 0;
    for (int i = 0; i < mMSG->gameStatus.players.size(); i++)
    {
        if(mMSG->gameStatus.players[i].role == E_ROLE::deathEaters || mMSG->gameStatus.players[i].role == E_ROLE::voldemort)
        {
            playerToDisplay++;
        }
    }
    offset = (2*area.x() + area.width() - (playerToDisplay * (littleSide + 10)))/2;

    // Create different boxes to display.
    E_IDENTIFIER hitlerIdentifier = E_IDENTIFIER::ID_none;
    for(int i = 0; i < mMSG->gameStatus.players.size(); i++)
    {
        if(mMSG->gameStatus.players[i].role == E_ROLE::deathEaters)
        {
            // Add a box to display. Start at offset + Hitler box.
            facisteBoxes.append(QRect(offset + (facisteBoxes.size() + 1)*(littleSide + 10), area.y() + (area.height()-littleSide)/2, littleSide, littleSide));
        }

        if(mMSG->gameStatus.players[i].role == E_ROLE::voldemort)
        {
            hitlerIdentifier = static_cast<E_IDENTIFIER>(i);
        }
    }

    // Draw boxes.

    painter.setOpacity(1);
    // Hitler box.
    QRect hitlerBox(offset, area.y() + (area.height()-littleSide)/2, littleSide, littleSide);
    painter.drawPixmap(hitlerBox, QPixmap(QString(":/images/%1").arg(RES_VOLDEMORT)));
    // Faciste boxes.
    quint8 currentPlayer = 0;
    for(int i = 0; i < mMSG->gameStatus.players.size(); i++)
    {
        if(mMSG->gameStatus.players[i].role != E_ROLE::deathEaters)
            continue;

        painter.drawPixmap(facisteBoxes[currentPlayer++], QPixmap(QString(":/images/%1.png").arg(g_RoleNames[mMSG->gameStatus.players[i].roleName])));
    }

    // Draw labels.
    painter.setPen(QColor(0xFF,0xFF,0xFF));
    QFont fontName("Germania");
    fontName.setBold(false);
    fontName.setPointSize(10);

    QFont fontRole("Times New Roman");
    fontRole.setBold(true);
    fontRole.setPointSize(10);

    painter.setFont(fontRole);
    QFontMetrics tool(painter.font());
    QRect bounding;

    // Hitler label.
    bounding = tool.boundingRect("Voldemort");
    painter.drawText(QRect(hitlerBox.x() + (littleSide - bounding.width())/2,
                           hitlerBox.y() + hitlerBox.height() - 2 * bounding.height(),
                           bounding.width(),
                           bounding.height()),
                     Qt::AlignCenter, "Voldemort");
    painter.setFont(fontName);
    QFontMetrics tool2(painter.font());
    bounding = tool2.boundingRect(mMSG->gameStatus.players[hitlerIdentifier].name);
    painter.drawText(QRect(hitlerBox.x() + (littleSide - bounding.width())/2,
                           hitlerBox.y() + hitlerBox.height() - bounding.height(),
                           bounding.width(),
                           bounding.height()),
                     Qt::AlignCenter, mMSG->gameStatus.players[hitlerIdentifier].name);
    // Faciste boxes.
    quint8 index = 0;
    for(QRect box : facisteBoxes)
    {
        for(; index < mMSG->gameStatus.players.size(); index++)
        {
            if(mMSG->gameStatus.players[index].role == E_ROLE::deathEaters)
            {
                painter.setFont(fontRole);
                bounding = tool.boundingRect("Mangemort");
                painter.drawText(QRect(box.x() + (littleSide - bounding.width())/2,
                                       box.y() + box.height() - 2 * bounding.height(),
                                       bounding.width(),
                                       bounding.height()),
                                 Qt::AlignCenter, "Mangemort");
                painter.setFont(fontName);
                QFontMetrics tool2(painter.font());
                bounding = tool.boundingRect(mMSG->gameStatus.players[index].name);
                painter.drawText(QRect(box.x() + (littleSide - bounding.width())/2,
                                       box.y() + box.height() - bounding.height(),
                                       bounding.width(),
                                       bounding.height()),
                                 Qt::AlignCenter, mMSG->gameStatus.players[index].name);
                index++;
                break;
            }
        }
    }
}
