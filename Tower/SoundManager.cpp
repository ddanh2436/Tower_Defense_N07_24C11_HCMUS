#include "SoundManager.h"
#include <iostream>
#include <algorithm> 
#include <iomanip> 
#include <map>
#include <deque>

namespace SoundManager {

    sf::Music backgroundMusic;
    std::map<std::string, sf::SoundBuffer> soundEffectBuffers;
    std::deque<sf::Sound> playingSoundEffects;
    bool isGlobalSoundEnabled = true;

    static bool gameMusicOn = true;
    static std::string currentTrackPath = "";
    static float masterVolumeSetting = 50.f;
    // A deque is required here: sf::Sound must not be relocated while it plays,
    // and a vector would move every live sound whenever it grew.
    static const size_t MAX_CONCURRENT_EFFECTS = 32;

    const std::string GAME_MUSIC_FILEPATH = "assets/game_music.ogg"; 
    const std::string MENU_MUSIC_FILEPATH = "assets/menu_music.ogg";
    const std::string VICTORY_MUSIC_FILEPATH = "assets/victory_music.ogg";
	const std::string LOSE_MUSIC_FILEPATH = "assets/lose_music.ogg";
        

    void applyAllVolumeSettings() {
        // This used to bail out while the track was stopped. Because
        // playBackgroundMusic applies the volume before calling play(), the
        // volume setting was silently ignored and every track started at 100.
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

        backgroundMusic.play();
        applyAllVolumeSettings();
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

    // Sound effects follow the master volume slider, scaled by the per-call
    // volume so a caller can still make one effect quieter than the rest.
    float getEffectVolume() {
        return isGlobalSoundEnabled ? masterVolumeSetting : 0.f;
    }

    void playSoundEffect(const std::string& id, float volume) {
        if (!isGlobalSoundEnabled) {
            return;
        }
        auto it = soundEffectBuffers.find(id);
        if (it == soundEffectBuffers.end()) {
            std::cerr << "Error: Sound effect '" << id << "' not found or not loaded." << std::endl;
            return;
        }

        // Drop the oldest finished sounds first so a burst of hits cannot grow
        // this list without bound.
        while (playingSoundEffects.size() >= MAX_CONCURRENT_EFFECTS) {
            playingSoundEffects.pop_front();
        }

        playingSoundEffects.emplace_back(it->second);
        playingSoundEffects.back().setVolume(getEffectVolume() * (volume / 100.f));
        playingSoundEffects.back().play();
    }

    void playVictoryMusic() {
        playBackgroundMusic(VICTORY_MUSIC_FILEPATH, 70.f, false);
    }

    void playLoseMusic() {
        playBackgroundMusic(LOSE_MUSIC_FILEPATH, 70.f, false);
    }

    void update() {
        playingSoundEffects.erase(
            std::remove_if(playingSoundEffects.begin(), playingSoundEffects.end(),
                [](const sf::Sound& s) { return s.getStatus() == sf::Sound::Stopped; }),
            playingSoundEffects.end()
        );
    }
} 