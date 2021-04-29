#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QWidget>
#include <QMediaPlayer>

class SoundManager : public QWidget
{
    Q_OBJECT

public:
    typedef enum
    {
        putDownCard,
        handleCard,
        pistol,
        select,
        unlock,
        vote,
        winner,
        ready

    }E_SOUND;

    typedef enum
    {
        none,
        waitingGame

    }E_MUSIC;

public:
    explicit SoundManager(QWidget* parent = nullptr, bool* enableSounds = nullptr, bool* enableMusic = nullptr);
    ~SoundManager();

public slots:
    void playSound(SoundManager::E_SOUND);
    void playMusic(SoundManager::E_MUSIC);

private:
    QMediaPlayer * music_waitingGame;
    QMediaPlaylist* playlist_waitingGame;

    QMediaPlayer* mSound_putDownCard;
    QMediaPlayer* mSound_drawCard;
    QMediaPlayer* mSound_pistol;
    QMediaPlayer* mSound_select;
    QMediaPlayer* mSound_unlock;
    QMediaPlayer* mSound_vote;
    QMediaPlayer* mSound_winner;
    QMediaPlayer* mSound_ready;

    bool* mEnableSounds;
    bool* mEnableMusic;
};

#endif // SOUNDMANAGER_H
