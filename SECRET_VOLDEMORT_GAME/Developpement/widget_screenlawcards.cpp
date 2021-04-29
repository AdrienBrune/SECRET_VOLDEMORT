#include "widget_screenlawcards.h"
#include "ui_widget_screenlawcards.h"

Widget_ScreenLawCards::Widget_ScreenLawCards(QWidget* parent, S_MESSAGE* MSG):
    QWidget(parent),
    mMSG(MSG),
    mHover(-1),
    ui(new Ui::Widget_ScreenLawCards)
{
    ui->setupUi(this);
}

Widget_ScreenLawCards::~Widget_ScreenLawCards()
{
    delete ui;
}

void Widget_ScreenLawCards::Setup_PresidentDiscard()
{
    initScreen();

    for(int i = 0; i < 3; i++)
    {
        Widget_Card * law = new Widget_Card(this, i);
        if(mMSG->gameStatus.pile[mMSG->gameStatus.pile.size() - 1 - i] == E_CARD::deathEatersLaw)
        {
            law->setLaw(E_CARD::deathEatersLaw);
        }
        else
        {
            law->setLaw(E_CARD::phenixOrderLAw);
        }
        law->setStyleSheet("Widget_Card{background-color:rgba(0,0,0,0)}");

        connect(law, &Widget_Card::clicked, this, [i, this](bool){ this->onCardClicked(i); });
        connect(law, SIGNAL(sig_hoverIn(int)), this, SLOT(hoverIn(int)));
        connect(law, SIGNAL(sig_hoverOut(int)), this, SLOT(hoverOut(int)));

        mLaws.append(law);
        ui->layoutCards->addWidget(law);
    }
}

void Widget_ScreenLawCards::Setup_ChancelorDiscard()
{
    initScreen();

    for(int i = 0; i < 2; i++)
    {
        Widget_Card * law = new Widget_Card(this, i);
        if(mMSG->gameStatus.pile[mMSG->gameStatus.pile.size() - 1 - i] == E_CARD::deathEatersLaw)
        {
            law->setLaw(E_CARD::deathEatersLaw);
        }
        else
        {
            law->setLaw(E_CARD::phenixOrderLAw);
        }
        law->setStyleSheet("Widget_Card{background-color:rgba(0,0,0,0)}");

        connect(law, &Widget_Card::clicked, this, [i, this](bool){ this->onCardClicked(i); });
        connect(law, SIGNAL(sig_hoverIn(int)), this, SLOT(hoverIn(int)));
        connect(law, SIGNAL(sig_hoverOut(int)), this, SLOT(hoverOut(int)));

        mLaws.append(law);
        ui->layoutCards->addWidget(law);
    }
}

void Widget_ScreenLawCards::Setup_PowerCheckPile()
{
    initScreen();

    for(int i = 0; i < 3; i++)
    {
        Widget_Card * law = new Widget_Card(this, i);
        if(mMSG->gameStatus.pile[mMSG->gameStatus.pile.size() - 1 - i] == E_CARD::deathEatersLaw)
        {
            law->setLaw(E_CARD::deathEatersLaw);
        }
        else
        {
            law->setLaw(E_CARD::phenixOrderLAw);
        }
        law->setStyleSheet("Widget_Card{background-color:rgba(0,0,0,0)}");

        connect(law, &Widget_Card::clicked, this, [i, this](bool){ this->onCardClicked(i); });
        connect(law, SIGNAL(sig_hoverIn(int)), this, SLOT(hoverIn(int)));
        connect(law, SIGNAL(sig_hoverOut(int)), this, SLOT(hoverOut(int)));

        mLaws.append(law);
        ui->layoutCards->addWidget(law);
    }
}

void Widget_ScreenLawCards::initScreen()
{
    while(!mLaws.isEmpty())
        delete mLaws.takeLast();
}

void Widget_ScreenLawCards::onCardClicked(int identifier)
{
    if(mMSG->gameStatus.players[mMSG->identifier].electionRole == E_ELECTION_ROLE::minister
            && mMSG->gameStatus.players[mMSG->identifier].power == E_POWER::checkPile)
        return;

    emit sig_lawCardClicked(mMSG->gameStatus.pile.size() - 1 - identifier);
}

void Widget_ScreenLawCards::hoverIn(int identifier)
{
    mHover = identifier;
    update();
}

void Widget_ScreenLawCards::hoverOut(int)
{
    mHover = -1;
    update();
}

void Widget_ScreenLawCards::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawPixmap(QRect(0, 0, width(), height()), QPixmap(":/images/background_motif.png"));

    // Card ratio : 0.74

    for(Widget_Card * card : mLaws)
    {
        // Check which size need to be resize (width or height).
        QRect cardArea;
        QRect cardBiggerArea;
        int WidthRatioApplied = static_cast<int>(card->width()*1.35);
        if( WidthRatioApplied > card->height())
        {
            cardArea.setRect(card->x() + (card->width() - static_cast<int>(card->height()/1.35))/2, card->y(), static_cast<int>(card->height()/1.35), card->height());
            cardBiggerArea.setRect(cardArea.x() - 10, cardArea.y() - static_cast<int>(10/0.74), cardArea.width() + 20, cardArea.height() + static_cast<int>(20/0.74));
        }
        else
        {
            cardArea.setRect(card->x(), card->y() + (card->height() - static_cast<int>(card->width()/0.74))/2, card->width(), static_cast<int>(card->width()/0.74));
            cardBiggerArea.setRect(cardArea.x() - 10, cardArea.y() - static_cast<int>(10/0.74), cardArea.width() + 20, cardArea.height() + static_cast<int>(20/0.74));
        }

        // Load image.
        QString imageToLoad;
        if(card->getLaw() == E_CARD::deathEatersLaw)
        {
            imageToLoad = QString(":/images/%1").arg(RES_LAW_DEATHEATERS);
        }
        else
        {
            imageToLoad = QString(":/images/%1").arg(RES_LAW_PHENIXORDER);
        }

        // Draw card.
        if(mHover == card->getIdentifier())
        {
            painter.drawPixmap(cardBiggerArea, QPixmap(imageToLoad));
        }
        else
        {
            painter.drawPixmap(cardArea, QPixmap(imageToLoad));
        }
    }

}

