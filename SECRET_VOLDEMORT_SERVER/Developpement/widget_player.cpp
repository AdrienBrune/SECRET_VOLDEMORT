#include "widget_player.h"
#include "ui_widget_player.h"

Widget_Player::Widget_Player(QWidget* parent, S_PLAYER player) :
    QWidget(parent),
    mPlayer(player),
    ui(new Ui::Widget_Player)
{
    ui->setupUi(this);

    initWidget();
}

Widget_Player::~Widget_Player()
{
    delete ui;
}

void Widget_Player::initWidget()
{
    ui->edit_name->setText(mPlayer.name);
    ui->edit_id->setText(QString("%1").arg(mPlayer.identifier));
    //ui->edit_socket->setText(static_cast<long>(mPlayer.socket))
    ui->edit_electionRole->setCurrentIndex(mPlayer.electionRole - E_ELECTION_ROLE::none);
    ui->edit_power->setCurrentIndex(mPlayer.power - E_POWER::noPower);
    ui->edit_vote->setCurrentIndex(mPlayer.vote - E_VOTE::blank);
    ui->edit_role->setCurrentIndex(mPlayer.role - E_ROLE::notAssigned);
}

void Widget_Player::setPlayer(S_PLAYER player)
{
    mPlayer = player;
}

S_PLAYER Widget_Player::getPlayer()
{
    // Get all inputs that can be changed by moderator.
    applyPlayerInputs();

    return S_PLAYER(mPlayer);
}

void Widget_Player::applyPlayerInputs()
{
    // Identifier, socket can't be changed.

    // Get player name.
    mPlayer.name = ui->edit_name->text();

    // Get player election role.
    mPlayer.electionRole = static_cast<E_ELECTION_ROLE>(ui->edit_electionRole->currentIndex() + E_ELECTION_ROLE::none);

    // Get player power.
    mPlayer.power = static_cast<E_POWER>(ui->edit_power->currentIndex() + E_POWER::noPower);

    // Get player vote.
    mPlayer.vote = static_cast<E_VOTE>(ui->edit_vote->currentIndex() + E_VOTE::blank);

    // Get player role.
    mPlayer.role = static_cast<E_ROLE>(ui->edit_role->currentIndex() + E_ROLE::notAssigned);
}
