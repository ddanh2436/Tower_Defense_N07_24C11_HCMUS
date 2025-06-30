#include "SoundManager.h"
#include <iostream>
#include <algorithm> 
#include <iomanip> 

namespace SoundManager {

    sf::Music backgroundMusic;
    std::map<std::string, sf::SoundBuffer> soundEffectBuffers;
    std::vector<sf::Sound> playingSoundEffects;
    bool isGlobalSoundEnabled = true;

    static bool gameMusicOn = true;
    static std::string currentTrackPath = "";
    static float masterVolumeSetting = 50.f; 

    const std::string GAME_MUSIC_FILEPATH = "assets/game_music.ogg"; 
    const std::string MENU_MUSIC_FILEPATH = "assets/menu_music.ogg";


    void applyAllVolumeSettings() {
        if (backgroundMusic.getStatus() == sf::Music::Stopped) return;

        float volumeToApply = masterVolumeSetting;

        if (!isGlobalSoundEnabled) {
            volumeToApply = 0.f;
        }
        else if (currentTrackPath == GAME_MUSIC_FILEPATH && !gameMusicOn) {
            volumeToApply = 0.f;
        }

        backgroundMusic.setVolume(volumeToApply);
    }


    void initialize() {
        std::cout << "SoundManager initializing... Global sound is " << (isGlobalSoundEnabled ? "ON" : "OFF") << std::endl;
        std::cout << "Game music is initially " << (gameMusicOn ? "ON" : "OFF") << std::endl;
        std::cout << "Master volume is initially " << masterVolumeSetting << std::endl;
        loadSoundEffect("menu_click", "assets/menu_click.wav");
        loadSoundEffect("tower_shoot", "assets/tower_shoot.wav");
        loadSoundEffect("enemy_explode", "assets/enemy_explode.wav");
    }

    void toggleGlobalSound() {
        isGlobalSoundEnabled = !isGlobalSoundEnabled;
        std::cout << "Global sound toggled. Now: " << (isGlobalSoundEnabled ? "ON" : "OFF") << std::endl;
        applyAllVolumeSettings(); 
    }

    bool getGlobalSoundState() {
        return isGlobalSoundEnabled;
    }

    bool getGameMusicState() {
        return gameMusicOn;
    }

    std::string getCurrentTrackPath() {
        return currentTrackPath;
    }

    void toggleGameMusic() {
        gameMusicOn = !gameMusicOn;
        std::cout << "Game music flag toggled. Now: " << (gameMusicOn ? "ON" : "OFF") << std::endl;

        if (currentTrackPath == GAME_MUSIC_FILEPATH) {
            applyAllVolumeSettings();
        }
    }

    void adjustMasterVolume(float delta) {
        masterVolumeSetting += delta;
        masterVolumeSetting = std::max(0.f, std::min(100.f, masterVolumeSetting)); 
        std::cout << "Master volume setting changed to: " << masterVolumeSetting << std::endl;
        applyAllVolumeSettings(); 
    }

    float getMasterVolumeSetting() {
        return masterVolumeSetting;
    }
    // ------------------------

    bool playBackgroundMusic(const std::string& filePath, float baseTrackDefaultVolume /*unused for now, masterVolumeSetting is king*/, bool loop) {
        if (backgroundMusic.getStatus() == sf::Music::Playing && currentTrackPath == filePath) {
            applyAllVolumeSettings(); 
            backgroundMusic.setLoop(loop); 
            return true;
        }

        if (!backgroundMusic.openFromFile(filePath)) {
            std::cerr << "Error: Could not load background music from " << filePath << std::endl;
            currentTrackPath = "";
            return false;
        }
        currentTrackPath = filePath;
        backgroundMusic.setLoop(loop);

        applyAllVolumeSettings();

        backgroundMusic.play();
        std::cout << "Playing background music: " << filePath
            << ". Effective volume: " << backgroundMusic.getVolume() << std::endl;
        return true;
    }

    void stopBackgroundMusic() {
        backgroundMusic.stop();
        std::cout << "Background music stopped. Current track was: " << currentTrackPath << std::endl;
        currentTrackPath = "";
    }

    void pauseBackgroundMusic() {
        if (backgroundMusic.getStatus() == sf::Music::Playing) {
            backgroundMusic.pause();
        }
    }

    void resumeBackgroundMusic() {
        if (backgroundMusic.getStatus() == sf::Music::Paused) {
            backgroundMusic.play();
            applyAllVolumeSettings(); 
        }
    }

    bool loadSoundEffect(const std::string& id, const std::string& filePath) {
        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile(filePath)) {
            std::cerr << "Error: Could not load sound effect '" << id << "' from " << filePath << std::endl;
            return false;
        }
        soundEffectBuffers[id] = buffer;
        return true;
    }

    void playSoundEffect(const std::string& id, float volume) {
        if (!isGlobalSoundEnabled) {
            return;
        }
        auto it = soundEffectBuffers.find(id);
        if (it != soundEffectBuffers.end()) {
            playingSoundEffects.emplace_back(it->second);
            playingSoundEffects.back().setVolume(volume);
            playingSoundEffects.back().play();
        }
        else {
            std::cerr << "Error: Sound effect '" << id << "' not found or not loaded." << std::endl;
        }
    }

    void update() {
        playingSoundEffects.erase(
            std::remove_if(playingSoundEffects.begin(), playingSoundEffects.end(),
                [](const sf::Sound& s) { return s.getStatus() == sf::Sound::Stopped; }),
            playingSoundEffects.end()
        );
    }
} 