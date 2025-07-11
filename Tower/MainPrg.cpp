#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <Windows.h>
#include "cgame.h"
#include "Menu.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "Leaderboard.h"

#pragma comment(lib, "user32.lib")

constexpr unsigned int DEFAULT_WINDOW_WIDTH = 1024;
constexpr unsigned int DEFAULT_WINDOW_HEIGHT = 768;
const std::string MENU_MUSIC_PATH = "assets/menu_music.ogg";
const std::string GAME_MUSIC_PATH = "assets/game_music.ogg";
const std::string LOSE_MUSIC_FILEPATH = "assets/lose_music.ogg";
constexpr float MENU_MUSIC_VOLUME = 50.f;
constexpr float GAME_MUSIC_VOLUME = 70.f;

const std::string SAVE_GAME_FILENAME = "data/savegame.txt";

int determineStars(const cgame& gameManager) {
    int lives = gameManager.getLives();
    int maxLives = gameManager.getMaxLives();
    if (maxLives <= 0 || lives <= 0) return 0;
    if (lives == maxLives) return 3;
    if (lives >= maxLives / 2) return 2;
    return 1;
}

static std::vector<MapInfo> loadMapInfos(const std::string& indexPath) {
    std::vector<MapInfo> maps;
    std::ifstream file(indexPath);
    if (!file.is_open()) {
        std::cerr << "Fatal Error: Could not open maps_index.txt at path: " << indexPath << std::endl;
        return maps;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string id, name, dataFile;
        if (std::getline(ss, id, ',') && std::getline(ss, name, ',') && std::getline(ss, dataFile, ',')) {
            id.erase(id.find_last_not_of(" \n\r\t") + 1);
            name.erase(0, name.find_first_not_of(" \n\r\t"));
            name.erase(name.find_last_not_of(" \n\r\t") + 1);
            dataFile.erase(0, dataFile.find_first_not_of(" \n\r\t"));
            dataFile.erase(dataFile.find_last_not_of(" \n\r\t") + 1);
            if (!id.empty() && !name.empty() && !dataFile.empty()) {
                maps.push_back({ id, name, dataFile });
            }
        }
    }
    file.close();
    return maps;
}

static std::string getPlayerNameInput(sf::RenderWindow& window, sf::Font& font) {
    std::string playerName;
    sf::Text inputText(playerName + "|", font, 30);
    sf::Text promptText("VICTORY! Enter your name:", font, 40);
    sf::RectangleShape background(sf::Vector2f(window.getSize()));
    background.setFillColor(sf::Color(0, 0, 0, 180));
    promptText.setFillColor(sf::Color::Yellow);
    sf::FloatRect promptBounds = promptText.getLocalBounds();
    promptText.setOrigin(promptBounds.width / 2.f, promptBounds.height / 2.f);
    promptText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f - 50);
    inputText.setFillColor(sf::Color::White);
    inputText.setPosition(window.getSize().x / 2.f - 100, window.getSize().y / 2.f + 20);
    sf::Clock cursorClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) return "Player";
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return && !playerName.empty()) {
                return playerName;
            }
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !playerName.empty()) {
                    playerName.pop_back();
                }
                else if (event.text.unicode < 128 && event.text.unicode != '\b' && playerName.length() < 10) {
                    playerName += static_cast<char>(event.text.unicode);
                }
            }
        }
        bool showCursor = static_cast<int>(cursorClock.getElapsedTime().asSeconds() * 2) % 2 == 0;
        inputText.setString(playerName + (showCursor ? "|" : ""));

        window.draw(background);
        window.draw(promptText);
        window.draw(inputText);
        window.display();
    }
    return "Player";
}

static GameState runGame(sf::RenderWindow& window, cgame& gameManager, Leaderboard& leaderboard) {
    sf::Clock clock;
    SoundManager::stopBackgroundMusic();
    if (SoundManager::getGameMusicState()) {
        SoundManager::playBackgroundMusic(GAME_MUSIC_PATH, GAME_MUSIC_VOLUME);
    }

    bool scoreHasBeenSaved = false;

    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        sf::Event event;
        SoundManager::update();

        // --- Xử lý sự kiện ---
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) return GameState::ConfirmExit;
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                gameManager.setPaused(true);
                SoundManager::playSoundEffect("assets/menu_click.ogg");
            }

            // Chỉ xử lý input trong game nếu game không dừng, không kết thúc
            if (!gameManager.isPaused() && !gameManager.isGameOver()) {
                gameManager.handleInput(event, window);
            }
            // Xử lý input cho màn hình thắng/thua
            else if (gameManager.isGameOver()) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Font font; // Cần font để lấy kích thước bounds
                    if (!font.loadFromFile("assets/pixel_font.ttf")) return GameState::ShowingMenu;
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    sf::Vector2f windowCenter(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

                    if (gameManager.hasWon()) {
                        sf::Text nextButton("Next Level", font, 40);
                        nextButton.setOrigin(nextButton.getLocalBounds().width / 2, nextButton.getLocalBounds().height / 2);
                        nextButton.setPosition(windowCenter.x, windowCenter.y + 20);

                        sf::Text restartButton("Restart", font, 40);
                        restartButton.setOrigin(restartButton.getLocalBounds().width / 2, restartButton.getLocalBounds().height / 2);
                        restartButton.setPosition(windowCenter.x, windowCenter.y + 80);

                        sf::Text quitButton("Quit to Menu", font, 40);
                        quitButton.setOrigin(quitButton.getLocalBounds().width / 2, quitButton.getLocalBounds().height / 2);
                        quitButton.setPosition(windowCenter.x, windowCenter.y + 140);

                        if (nextButton.getGlobalBounds().contains(mousePos)) return GameState::GoToNextLevel;
                        if (restartButton.getGlobalBounds().contains(mousePos)) return GameState::Restarting;
                        if (quitButton.getGlobalBounds().contains(mousePos)) return GameState::ShowingMenu;
                    }
                    else { // Khi thua
                        sf::Text restartButton("Restart", font, 40);
                        restartButton.setOrigin(restartButton.getLocalBounds().width / 2, restartButton.getLocalBounds().height / 2);
                        restartButton.setPosition(windowCenter.x, windowCenter.y + 20);

                        sf::Text quitButton("Quit to Menu", font, 40);
                        quitButton.setOrigin(quitButton.getLocalBounds().width / 2, quitButton.getLocalBounds().height / 2);
                        quitButton.setPosition(windowCenter.x, windowCenter.y + 80);

                        if (restartButton.getGlobalBounds().contains(mousePos)) return GameState::Restarting;
                        if (quitButton.getGlobalBounds().contains(mousePos)) return GameState::ShowingMenu;
                    }
                }
            }
        }

        // --- Cập nhật trạng thái game ---
        if (!gameManager.isPaused() && !gameManager.isGameOver()) {
            gameManager.update(deltaTime);
        }

        // --- Xử lý logic khi thắng và lưu điểm (chỉ chạy một lần) ---
        if (gameManager.hasWon() && !scoreHasBeenSaved) {
            SoundManager::playVictoryMusic();

            window.clear();
            gameManager.render(window); // Vẽ cảnh game làm nền

            sf::Font font;
            if (!font.loadFromFile("assets/pixel_font.ttf")) return GameState::ShowingMenu;

            std::string playerName = getPlayerNameInput(window, font);
            long score = gameManager.calculateScore();
            int kills = gameManager.getEnemiesDefeated();
            float time = gameManager.getLevelTime().asSeconds();
            leaderboard.addScore(playerName, score, kills, time);

            scoreHasBeenSaved = true;
        }

        // --- Vẽ mọi thứ ra màn hình ---
        window.clear(sf::Color(25, 25, 25));
        gameManager.render(window); // Luôn vẽ game làm nền

        // Vẽ UI tương ứng với trạng thái
        if (gameManager.isGameOver()) {
            sf::Font font;
            if (!font.loadFromFile("assets/pixel_font.ttf")) return GameState::ShowingMenu;
            sf::Vector2f windowCenter(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
            sf::RectangleShape panel(sf::Vector2f(500, 350));
            panel.setFillColor(sf::Color(0, 0, 0, 200));
            panel.setOutlineColor(sf::Color(255, 255, 255, 100));
            panel.setOutlineThickness(2);
            panel.setOrigin(panel.getSize().x / 2.0f, panel.getSize().y / 2.0f);
            panel.setPosition(windowCenter);
            window.draw(panel);

            if (gameManager.hasWon()) {
                sf::Text victoryText("VICTORY!", font, 50);
                victoryText.setFillColor(sf::Color::Yellow);
                victoryText.setOrigin(victoryText.getLocalBounds().width / 2, victoryText.getLocalBounds().height / 2);
                victoryText.setPosition(windowCenter.x, windowCenter.y - 130);
                window.draw(victoryText);

                int starsEarned = determineStars(gameManager);
                std::vector<sf::Sprite> starSprites;
                sf::Texture& starFilledTexture = TextureManager::getTexture("assets/star.png");
                sf::Texture& starEmptyTexture = TextureManager::getTexture("assets/star_field.png");
                for (int i = 0; i < 3; ++i) {
                    sf::Sprite star;
                    star.setTexture(i < starsEarned ? starFilledTexture : starEmptyTexture);
                    star.setScale(0.5f, 0.5f);
                    sf::FloatRect starBounds = star.getLocalBounds();
                    float totalStarsWidth = 3 * starBounds.width * 0.5f + 2 * 15.f;
                    float startX = windowCenter.x - totalStarsWidth / 2.0f;
                    star.setPosition(startX + i * (starBounds.width * 0.5f + 15.f), windowCenter.y - 70);
                    starSprites.push_back(star);
                }
                for (const auto& star : starSprites) window.draw(star);

                sf::Text nextButton("Next Level", font, 40);
                nextButton.setOrigin(nextButton.getLocalBounds().width / 2, nextButton.getLocalBounds().height / 2);
                nextButton.setPosition(windowCenter.x, windowCenter.y + 20);
                sf::Text restartButton("Restart", font, 40);
                restartButton.setOrigin(restartButton.getLocalBounds().width / 2, restartButton.getLocalBounds().height / 2);
                restartButton.setPosition(windowCenter.x, windowCenter.y + 80);
                sf::Text quitButton("Quit to Menu", font, 40);
                quitButton.setOrigin(quitButton.getLocalBounds().width / 2, quitButton.getLocalBounds().height / 2);
                quitButton.setPosition(windowCenter.x, windowCenter.y + 140);

                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (nextButton.getGlobalBounds().contains(mousePos)) nextButton.setFillColor(sf::Color::Yellow); else nextButton.setFillColor(sf::Color::White);
                if (restartButton.getGlobalBounds().contains(mousePos)) restartButton.setFillColor(sf::Color::Yellow); else restartButton.setFillColor(sf::Color::White);
                if (quitButton.getGlobalBounds().contains(mousePos)) quitButton.setFillColor(sf::Color::Yellow); else quitButton.setFillColor(sf::Color::White);

                window.draw(nextButton);
                window.draw(restartButton);
                window.draw(quitButton);
            }
            else { // Khi thua
                sf::Text titleText("GAME OVER", font, 50);
                titleText.setFillColor(sf::Color::Red);
                titleText.setOrigin(titleText.getLocalBounds().width / 2, titleText.getLocalBounds().height / 2);
                titleText.setPosition(windowCenter.x, windowCenter.y - 70);
                window.draw(titleText);

                sf::Text restartButton("Restart", font, 40);
                restartButton.setOrigin(restartButton.getLocalBounds().width / 2, restartButton.getLocalBounds().height / 2);
                restartButton.setPosition(windowCenter.x, windowCenter.y + 20);
                sf::Text quitButton("Quit to Menu", font, 40);
                quitButton.setOrigin(quitButton.getLocalBounds().width / 2, quitButton.getLocalBounds().height / 2);
                quitButton.setPosition(windowCenter.x, windowCenter.y + 80);

                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (restartButton.getGlobalBounds().contains(mousePos)) restartButton.setFillColor(sf::Color::Yellow); else restartButton.setFillColor(sf::Color::White);
                if (quitButton.getGlobalBounds().contains(mousePos)) quitButton.setFillColor(sf::Color::Yellow); else quitButton.setFillColor(sf::Color::White);

                window.draw(restartButton);
                window.draw(quitButton);
            }
        }
        else if (gameManager.isPaused()) {
            SoundManager::pauseBackgroundMusic();
            GameState pauseResult = showPauseMenu(window);
            if (pauseResult == GameState::Playing) {
                gameManager.setPaused(false);
                SoundManager::resumeBackgroundMusic();
                clock.restart();
            }
            else if (pauseResult == GameState::Restarting) {
                return GameState::Restarting;
            }
            else if (pauseResult == GameState::ConfirmExit) {
                gameManager.render(window);
                GameState choice = showConfirmExitScreen(window);
                if (choice == GameState::ExitWithSave) {
                    gameManager.saveGame(SAVE_GAME_FILENAME);
                    return GameState::ShowingMenu;
                }
                else if (choice == GameState::Exiting) {
                    return GameState::ShowingMenu;
                }
                else {
                    gameManager.setPaused(false);
                    SoundManager::resumeBackgroundMusic();
                    clock.restart();
                }
            }
        }

        window.display();
    }

    return GameState::Exiting;
}

int main() {
    SoundManager::initialize();
    SoundManager::loadSoundEffect("assets/tower_shoot.ogg", "assets/tower_shoot.ogg");
    SoundManager::loadSoundEffect("assets/tower_place.wav", "assets/tower_place.ogg");
    SoundManager::loadSoundEffect("assets/tower_upgrade.ogg", "assets/tower_upgrade.ogg");
    SoundManager::loadSoundEffect("assets/tower_sell.ogg", "assets/tower_sell.ogg");
    SoundManager::loadSoundEffect("assets/menu_click.ogg", "assets/menu_click.ogg");
    SoundManager::loadSoundEffect("assets/life_lost.ogg", "assets/life_lost.ogg");

    std::vector<MapInfo> mapInfos = loadMapInfos("data/maps_index.txt");
    if (mapInfos.empty()) {
        MessageBoxA(NULL, "Could not find or load 'data/maps_index.txt'.\nPlease ensure the file exists and is correctly formatted.", "Fatal Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    auto gameManager = std::make_unique<cgame>();
    Leaderboard leaderboard;

    const auto& fullscreenModes = sf::VideoMode::getFullscreenModes();
    sf::VideoMode selectedMode;
    bool fullscreen = false;
    if (!fullscreenModes.empty()) {
        selectedMode = fullscreenModes[0];
        fullscreen = true;
    }
    else {
        selectedMode = sf::VideoMode(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    }
    auto window = std::make_unique<sf::RenderWindow>(selectedMode, "Tower Defense SFML", fullscreen ? sf::Style::Fullscreen : sf::Style::Close);
    window->setFramerateLimit(60);

    GameState currentState = GameState::ShowingMenu;
    std::string selectedMapId = "";
    SoundManager::playBackgroundMusic(MENU_MUSIC_PATH, MENU_MUSIC_VOLUME);

    while (currentState != GameState::Exiting && window->isOpen()) {
        SoundManager::update();
        switch (currentState) {
        case GameState::ShowingMenu:
            if (SoundManager::getCurrentTrackPath() != MENU_MUSIC_PATH) {
                SoundManager::playBackgroundMusic(MENU_MUSIC_PATH, MENU_MUSIC_VOLUME);
            }
            currentState = showMenu(*window);
            break;
        case GameState::ShowingMapSelection: {
            if (SoundManager::getCurrentTrackPath() != MENU_MUSIC_PATH) {
                SoundManager::playBackgroundMusic(MENU_MUSIC_PATH, MENU_MUSIC_VOLUME);
            }
            std::string choice = showMapSelectionScreen(*window, mapInfos);
            if (!choice.empty()) {
                selectedMapId = choice;
                currentState = GameState::Playing;
            }
            else {
                currentState = GameState::ShowingMenu;
            }
            break;
        }
        case GameState::ShowingLeaderboard: {
            if (SoundManager::getCurrentTrackPath() != MENU_MUSIC_PATH) {
                SoundManager::playBackgroundMusic(MENU_MUSIC_PATH, MENU_MUSIC_VOLUME);
            }
            currentState = showLeaderboardScreen(*window, leaderboard);
            break;
        }
        case GameState::Playing: {
            auto it = std::find_if(mapInfos.begin(), mapInfos.end(), [&](const MapInfo& mi) {
                return mi.id == selectedMapId;
                });
            if (it != mapInfos.end()) {
                gameManager->loadMap(it->id, it->dataFile);
                currentState = runGame(*window, *gameManager, leaderboard);
            }
            else {
                std::cerr << "Error: Could not find map data for ID: " << selectedMapId << std::endl;
                currentState = GameState::ShowingMenu;
            }
            break;
        }
        case GameState::Restarting:
            currentState = GameState::Playing;
            break;
        case GameState::LoadingGame: {
            bool loadSuccess = gameManager->loadGame(SAVE_GAME_FILENAME);
            if (loadSuccess) {
                selectedMapId = gameManager->getCurrentMapId();
                auto it = std::find_if(mapInfos.begin(), mapInfos.end(), [&](const MapInfo& mi) {
                    return mi.id == selectedMapId;
                    });
                if (it != mapInfos.end()) {
                    gameManager->loadMap(it->id, it->dataFile);
                    gameManager->loadGame(SAVE_GAME_FILENAME);
                    currentState = runGame(*window, *gameManager, leaderboard);
                }
                else {
                    std::cerr << "Error: Save file is corrupted. Could not find map ID: " << selectedMapId << std::endl;
                    currentState = GameState::ShowingMenu;
                }
            }
            else {
                std::cout << "No save file found or failed to load. Returning to menu." << std::endl;
                currentState = GameState::ShowingMenu;
            }
            break;
        }
        case GameState::ConfirmExit: {
            gameManager->render(*window);
            GameState choice = showConfirmExitScreen(*window);
            if (choice == GameState::ExitWithSave) {
                gameManager->saveGame(SAVE_GAME_FILENAME);
                currentState = GameState::Exiting;
            }
            else if (choice == GameState::Exiting) {
                currentState = GameState::Exiting;
            }
            else {
                currentState = runGame(*window, *gameManager, leaderboard);
            }
            break;
        }
        case GameState::GoToNextLevel: {
            auto currentMapIt = std::find_if(mapInfos.begin(), mapInfos.end(), [&](const MapInfo& mi) {
                return mi.id == selectedMapId;
                });
            if (currentMapIt != mapInfos.end()) {
                currentMapIt++;
                if (currentMapIt == mapInfos.end()) {
                    currentMapIt = mapInfos.begin();
                    std::cout << "Completed last level. Looping back to first level." << std::endl;
                }
                selectedMapId = currentMapIt->id;
                std::cout << "Preparing next level: " << selectedMapId << std::endl;
                currentState = GameState::Playing;
            }
            else {
                std::cerr << "Error: Could not find current map '" << selectedMapId << "' to determine next level. Returning to menu." << std::endl;
                currentState = GameState::ShowingMenu;
            }
            break;
        }
        case GameState::SettingsScreen:
            if (SoundManager::getCurrentTrackPath() != MENU_MUSIC_PATH) {
                SoundManager::playBackgroundMusic(MENU_MUSIC_PATH, MENU_MUSIC_VOLUME);
            }
            currentState = showSettingsScreen(*window);
            break;
        case GameState::Exiting:
            break;
        default:
            currentState = GameState::ShowingMenu;
            break;
        }
    }

    SoundManager::stopBackgroundMusic();
    if (window->isOpen()) {
        window->close();
    }
    std::cout << "Da thoat Game." << std::endl;
    return 0;
}