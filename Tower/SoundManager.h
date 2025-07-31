#pragma once
#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <SFML/Audio.hpp>
#include <string>
#include <map>
#include <vector> 

namespace SoundManager {
    extern sf::Music backgroundMusic;
    extern std::map<std::string, sf::SoundBuffer> soundEffectBuffers;
    extern std::vector<sf::Sound> playingSoundEffects;

    extern bool isGlobalSoundEnabled;
    void initialize();

    void toggleGlobalSound();
    bool getGlobalSoundState();

    bool getGameMusicState();
    void toggleGameMusic();

 
    void adjustMasterVolume(float delta); 
    float getMasterVolumeSetting();       

    bool playBackgroundMusic(const std::string& filePath, float baseTrackDefaultVolume = 50.f, bool loop = true);
    void stopBackgroundMusic();
    void pauseBackgroundMusic();
    void resumeBackgroundMusic();
    std::string getCurrentTrackPath();

    bool loadSoundEffect(const std::string& id, const std::string& filePath);
    void playSoundEffect(const std::string& id, float volume = 100.f);
    void playVictoryMusic();
    void playLoseMusic();
    void update();

} 

#endif // SOUND_MANAGER_H