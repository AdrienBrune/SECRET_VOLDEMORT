#include "soundmanager.h"
#include <QMediaPlaylist>
#include <QRandomGenerator>

SoundManager::SoundManager(QWidget* parent, bool* enableSounds, bool* enableMusic):
    QWidget(parent),
    mEnableSounds(enableSounds),
    mEnableMusic(enableMusic)
{
    music_waitingGame = new QMediaPlayer(this);
    music_waitingGame->setVolume(5);
    playlist_waitingGame = new QMediaPlaylist(this);
    playlist_waitingGame->addMedia(QUrl("qrc:/musics/music_0.mp3"));
    playlist_waitingGame->addMedia(QUrl("qrc:/musics/music_1.mp3"));
    playlist_waitingGame->addMedia(QUrl("qrc:/musics/music_2.mp3"));
    playlist_waitingGame->addMedia(QUrl("qrc:/musics/music_3.mp3"));
    playlist_waitingGame->setPlaybackMode(QMediaPlaylist::PlaybackMode::Loop);
    music_waitingGame->setPlaylist(playlist_waitingGame);
    playlist_waitingGame->setCurrentIndex(QRandomGenerator::global()->bounded(playlist_waitingGame->mediaCount()));

    mSound_drawCard = new QMediaPlayer(this);
    mSound_drawCard->setMedia(QUrl("qrc:/sounds/sound_pioche.wav"));
    mSound_drawCard->setVolume(30);

    mSound_pistol = new QMediaPlayer(this);
    mSound_pistol->setMedia(QUrl("qrc:/sounds/sound_kill.mp3"));
    mSound_pistol->setVolume(30);

    mSound_putDownCard = new QMediaPlayer(this);
    mSound_putDownCard->setMedia(QUrl("qrc:/sounds/sound_put_card.wav"));
    mSound_putDownCard->setVolume(30);

    mSound_select = new QMediaPlayer(this);
    mSound_select->setMedia(QUrl("qrc:/sounds/sound_selection.wav"));
    mSound_select->setVolume(50);

    mSound_unlock = new QMediaPlayer(this);
    mSound_unlock->setMedia(QUrl("qrc:/sounds/sound_unlock.wav"));
    mSound_unlock->setVolume(70);

    mSound_vote = new QMediaPlayer(this);
    mSound_vote->setMedia(QUrl("qrc:/sounds/sound_vote.wav"));
    mSound_vote->setVolume(40);

    mSound_winner = new QMediaPlayer(this);
    mSound_winner->setMedia(QUrl("qrc:/sounds/sound_winner.wav"));
    mSound_winner->setVolume(50);

    mSound_ready = new QMediaPlayer(this);
    mSound_ready->setMedia(QUrl("qrc:/sounds/sound_ready.wav"));
    mSound_ready->setVolume(40);
}

SoundManager::~SoundManager()
{

}

void SoundManager::playMusic(SoundManager::E_MUSIC music)
{
    if(!*mEnableMusic)
    {
        music_waitingGame->stop();
        return;
    }

    switch(music)
    {
        case SoundManager::E_MUSIC::none:
            music_waitingGame->stop();
            break;

        case SoundManager::E_MUSIC::waitingGame:
            playlist_waitingGame->setCurrentIndex(QRandomGenerator::global()->bounded(playlist_waitingGame->mediaCount()));
            music_waitingGame->play();
            break;
    }
}

void SoundManager::playSound(SoundManager::E_SOUND sound)
{
    if(!*mEnableSounds)
        return;

    switch(sound)
    {
        case handleCard:
            mSound_drawCard->play();
            break;

        case pistol:
            mSound_pistol->play();
            break;

        case putDownCard:
            mSound_putDownCard->play();
            break;

        case select:
            mSound_select->play();
            break;

        case unlock:
            mSound_unlock->play();
            break;

        case vote:
            mSound_vote->play();
            break;

        case winner:
            mSound_winner->play();
            break;

        case ready:
            mSound_ready->play();
            break;
    }
}
