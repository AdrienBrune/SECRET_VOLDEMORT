#ifndef WIDGET_VOTE_LAW_2CARDS_H
#define WIDGET_VOTE_LAW_2CARDS_H

#include <QWidget>
#include <QPainter>
#include "widget_card.h"
#include "data.h"
#include "soundmanager.h"

namespace Ui {
class Widget_ScreenLawCards;
}

class Widget_ScreenLawCards : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_ScreenLawCards(QWidget * parent = nullptr, S_MESSAGE * MSG = nullptr);
    ~Widget_ScreenLawCards();

signals:
    void sig_lawCardClicked(int);
    void sig_vetoAsked();
    void sig_playSound(SoundManager::E_SOUND);

private slots:
    void hoverIn(int);
    void hoverOut(int);
    void onCardClicked(int);
    void onVetoAsked();

public:
    void Setup_MinisterDiscard();
    void Setup_DirectorDiscard();
    void Setup_PowerCheckPile();
    void enableVetoPower(bool toggle);
    void vetoRefused();

private:
    void initScreen();

protected:
    void paintEvent(QPaintEvent *);

public:
    S_MESSAGE* mMSG;
    QList<Widget_Card*> mLaws;
    int mHover;

private:
    Ui::Widget_ScreenLawCards *ui;
};

#endif // WIDGET_VOTE_LAW_2CARDS_H
