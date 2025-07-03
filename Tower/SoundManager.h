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

    // Hàm mới cho điều chỉnh âm lượng bằng +/-
    void adjustMasterVolume(float delta); // delta có thể là +10 hoặc -10
    float getMasterVolumeSetting();       // Lấy mức âm lượng hiện tại (0-100) để hiển thị

    bool playBackgroundMusic(const std::string& filePath, float baseTrackDefaultVolume = 50.f, bool loop = true);
    void stopBackgroundMusic();
    void pauseBackgroundMusic();
    void resumeBackgroundMusic();
    std::string getCurrentTrackPath();

    bool loadSoundEffect(const std::string& id, const std::string& filePath);
    void playSoundEffect(const std::string& id, float volume = 100.f);
    void playVictoryMusic();
    void update();

} 

#endif // SOUND_MANAGER_H