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
#include "TextureManager.h" // <-- THÊM MỚI: Cần thiết để tải texture ngôi sao

#pragma comment(lib, "user32.lib")

constexpr unsigned int DEFAULT_WINDOW_WIDTH = 1024;
constexpr unsigned int DEFAULT_WINDOW_HEIGHT = 768;
const std::string MENU_MUSIC_PATH = "assets/menu_music.ogg";
const std::string GAME_MUSIC_PATH = "assets/game_music.ogg";
constexpr float MENU_MUSIC_VOLUME = 50.f;
constexpr float GAME_MUSIC_VOLUME = 70.f;


// <-- THÊM MỚI: HÀM TÍNH TOÁN SỐ SAO DỰA TRÊN MẠNG CÒN LẠI -->
int determineStars(const cgame& gameManager) {
    int lives = gameManager.getLives();
    int maxLives = gameManager.getMaxLives();

    if (lives <= 0) return 0; // Thua cuộc

    // Dùng số thực để phép chia chính xác
    float livesPercent = static_cast<float>(lives) / maxLives;

    if (livesPercent >= 0.99f) { // Gần như hoặc hoàn toàn không mất máu
        return 3;
    }
    else if (livesPercent >= 0.5f) { // Còn lại ít nhất một nửa số mạng
        return 2;
    }
    else { // Còn lại dưới một nửa số mạng
        return 1;
    }
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
        if (line.empty() || line[0] == '#') {
            continue;
        }
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


static GameState runGame(sf::RenderWindow& window, cgame& gameManager) {
    sf::Clock clock;
    SoundManager::stopBackgroundMusic();
    if (SoundManager::getGameMusicState()) {
        SoundManager::playBackgroundMusic(GAME_MUSIC_PATH, GAME_MUSIC_VOLUME);
    }
    bool inGame = true;
    while (inGame && window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        sf::Event event;
        SoundManager::update();
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return GameState::Exiting;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                gameManager.setPaused(true);
                SoundManager::playSoundEffect("assets/menu_click.ogg");
            }
            if (!gameManager.isPaused()) {
                gameManager.handleInput(event, window);
            }
        }
        if (!gameManager.isPaused()) {
            gameManager.update(deltaTime);
        }

        // <-- THAY ĐỔI: Toàn bộ khối logic màn hình kết thúc được cập nhật -->
        if (gameManager.isGameOver()) {
            SoundManager::stopBackgroundMusic();
            sf::Font font;
            if (!font.loadFromFile("assets/pixel_font.ttf")) { return GameState::ShowingMenu; }
            sf::Vector2f windowCenter = sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
            sf::RectangleShape panel(sf::Vector2f(500, 350));
            panel.setFillColor(sf::Color(0, 0, 0, 200));
            panel.setOutlineColor(sf::Color(255, 255, 255, 100));
            panel.setOutlineThickness(2);
            panel.setOrigin(panel.getSize().x / 2.0f, panel.getSize().y / 2.0f);
            panel.setPosition(windowCenter);

            if (gameManager.hasWon()) {
                SoundManager::playVictoryMusic();
                sf::Text victoryText("VICTORY!", font, 50);
                victoryText.setFillColor(sf::Color::Yellow);
                victoryText.setOrigin(victoryText.getLocalBounds().width / 2, victoryText.getLocalBounds().height / 2);
                victoryText.setPosition(windowCenter.x, windowCenter.y - 130);

                // --- THÊM MỚI: LOGIC TẠO NGÔI SAO ---
                int starsEarned = determineStars(gameManager);
                std::vector<sf::Sprite> starSprites;
                sf::Texture& starFilledTexture = TextureManager::getTexture("assets/star.png");
                sf::Texture& starEmptyTexture = TextureManager::getTexture("assets/star_field.png");

                for (int i = 0; i < 3; ++i) {
                    sf::Sprite star;
                    if (i < starsEarned) {
                        star.setTexture(starFilledTexture);
                    }
                    else {
                        star.setTexture(starEmptyTexture);
                    }
                    star.setScale(0.5f, 0.5f);
                    sf::FloatRect starBounds = star.getLocalBounds();
                    float totalStarsWidth = 3 * starBounds.width * 0.5f + 2 * 15.f; // 3 sao + 2 khoảng cách
                    float startX = windowCenter.x - totalStarsWidth / 2.0f;
                    star.setPosition(startX + i * (starBounds.width * 0.5f + 15.f), windowCenter.y - 70);
                    starSprites.push_back(star);
                }
                // --- KẾT THÚC LOGIC TẠO NGÔI SAO ---

                sf::Text nextButton("Next Level", font, 40);
                nextButton.setOrigin(nextButton.getLocalBounds().width / 2, nextButton.getLocalBounds().height / 2);
                nextButton.setPosition(windowCenter.x, windowCenter.y + 20);
                sf::Text restartButton("Restart", font, 40);
                restartButton.setOrigin(restartButton.getLocalBounds().width / 2, restartButton.getLocalBounds().height / 2);
                restartButton.setPosition(windowCenter.x, windowCenter.y + 80);
                sf::Text quitButton("Quit to Menu", font, 40);
                quitButton.setOrigin(quitButton.getLocalBounds().width / 2, quitButton.getLocalBounds().height / 2);
                quitButton.setPosition(windowCenter.x, windowCenter.y + 140);

                while (window.isOpen()) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    nextButton.setFillColor(sf::Color::White);
                    restartButton.setFillColor(sf::Color::White);
                    quitButton.setFillColor(sf::Color::White);
                    if (nextButton.getGlobalBounds().contains(mousePos)) nextButton.setFillColor(sf::Color::Yellow);
                    if (restartButton.getGlobalBounds().contains(mousePos)) restartButton.setFillColor(sf::Color::Yellow);
                    if (quitButton.getGlobalBounds().contains(mousePos)) quitButton.setFillColor(sf::Color::Yellow);
                    sf::Event endEvent;
                    while (window.pollEvent(endEvent)) {
                        if (endEvent.type == sf::Event::Closed) return GameState::Exiting;
                        if (endEvent.type == sf::Event::MouseButtonPressed && endEvent.mouseButton.button == sf::Mouse::Left) {
                            if (nextButton.getGlobalBounds().contains(mousePos)) { SoundManager::playSoundEffect("assets/menu_click.ogg"); return GameState::GoToNextLevel; }
                            if (restartButton.getGlobalBounds().contains(mousePos)) { SoundManager::playSoundEffect("assets/menu_click.ogg"); return GameState::Restarting; }
                            if (quitButton.getGlobalBounds().contains(mousePos)) { SoundManager::playSoundEffect("assets/menu_click.ogg"); return GameState::ShowingMenu; }
                        }
                    }
                    window.clear();
                    gameManager.render(window);
                    window.draw(panel);
                    window.draw(victoryText);
                    // --- THÊM MỚI: VẼ CÁC NGÔI SAO ---
                    for (const auto& star : starSprites) {
                        window.draw(star);
                    }
                    // --- KẾT THÚC VẼ NGÔI SAO ---
                    window.draw(nextButton);
                    window.draw(restartButton);
                    window.draw(quitButton);
                    window.display();
                }
            }
            else {
                // ... (Màn hình thua cuộc giữ nguyên)
                sf::Text titleText("GAME OVER", font, 50);
                titleText.setFillColor(sf::Color::Red);
                titleText.setOrigin(titleText.getLocalBounds().width / 2, titleText.getLocalBounds().height / 2);
                titleText.setPosition(windowCenter.x, windowCenter.y - 70);
                sf::Text restartButton("Restart", font, 40);
                restartButton.setOrigin(restartButton.getLocalBounds().width / 2, restartButton.getLocalBounds().height / 2);
                restartButton.setPosition(windowCenter.x, windowCenter.y + 20);
                sf::Text quitButton("Quit to Menu", font, 40);
                quitButton.setOrigin(quitButton.getLocalBounds().width / 2, quitButton.getLocalBounds().height / 2);
                quitButton.setPosition(windowCenter.x, windowCenter.y + 80);
                while (window.isOpen()) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    restartButton.setFillColor(sf::Color::White);
                    quitButton.setFillColor(sf::Color::White);
                    if (restartButton.getGlobalBounds().contains(mousePos)) restartButton.setFillColor(sf::Color::Yellow);
                    if (quitButton.getGlobalBounds().contains(mousePos)) quitButton.setFillColor(sf::Color::Yellow);
                    sf::Event endEvent;
                    while (window.pollEvent(endEvent)) {
                        if (endEvent.type == sf::Event::Closed) return GameState::Exiting;
                        if (endEvent.type == sf::Event::MouseButtonPressed && endEvent.mouseButton.button == sf::Mouse::Left) {
                            if (restartButton.getGlobalBounds().contains(mousePos)) { SoundManager::playSoundEffect("menu_click"); return GameState::Restarting; }
                            if (quitButton.getGlobalBounds().contains(mousePos)) { SoundManager::playSoundEffect("menu_click"); return GameState::ShowingMenu; }
                        }
                    }
                    window.clear();
                    gameManager.render(window);
                    window.draw(panel);
                    window.draw(titleText);
                    window.draw(restartButton);
                    window.draw(quitButton);
                    window.display();
                }
            }
        }

        window.clear(sf::Color(25, 25, 25));
        gameManager.render(window);
        if (gameManager.isPaused()) {
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
            else {
                return pauseResult;
            }
        }
        else {
            window.display();
        }
    }
    return GameState::ShowingMenu;
}


int main() {
    SoundManager::initialize();
    SoundManager::loadSoundEffect("assets/tower_shoot.ogg", "assets/tower_shoot.ogg");
    SoundManager::loadSoundEffect("assets/tower_place.wav", "assets/tower_place.wav");
    SoundManager::loadSoundEffect("assets/tower_upgrade.ogg", "assets/tower_upgrade.ogg");
    SoundManager::loadSoundEffect("assets/tower_sell.ogg", "assets/tower_sell.ogg");
    SoundManager::loadSoundEffect("assets/menu_click.ogg", "assets/menu_click.ogg");
    std::vector<MapInfo> mapInfos = loadMapInfos("data/maps_index.txt");
    if (mapInfos.empty()) {
        std::cerr << "No maps found or failed to load map index. Exiting." << std::endl;
        MessageBoxA(NULL, "Could not find or load 'data/maps_index.txt'.\nPlease ensure the file exists and is correctly formatted.", "Fatal Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    auto gameManager = std::make_unique<cgame>();
    const auto fullscreenModes = sf::VideoMode::getFullscreenModes();
    sf::VideoMode selectedMode;
    bool fullscreen = false;
    if (!fullscreenModes.empty()) {
        selectedMode = fullscreenModes[0];
        fullscreen = true;
        std::cout << "Chon che do toan man hinh: " << selectedMode.width << "x" << selectedMode.height << " " << selectedMode.bitsPerPixel << "bpp" << std::endl;
    }
    else {
        selectedMode = sf::VideoMode(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
        std::cout << "Khong tim thay che do toan man hinh phu hop, su dung cua so: " << selectedMode.width << "x" << selectedMode.height << std::endl;
    }
    auto window = std::make_unique<sf::RenderWindow>(selectedMode, "Tower Defense SFML", fullscreen ? sf::Style::Fullscreen : sf::Style::Close);
    window->setFramerateLimit(60);
    GameState currentState = GameState::ShowingMenu;
    std::string selectedMapId = "";
    std::cout << "Khoi chay Tower Defense SFML." << std::endl;
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
        } break;
        case GameState::Playing: {
            std::cout << "Chuyen sang trang thai Playing Game cho map: " << selectedMapId << std::endl;
            auto it = std::find_if(mapInfos.begin(), mapInfos.end(), [&](const MapInfo& mi) {
                return mi.id == selectedMapId;
                });
            if (it != mapInfos.end()) {
                gameManager->loadMap(it->id, it->dataFile);
                currentState = runGame(*window, *gameManager);
            }
            else {
                std::cerr << "Error: Could not find map data for ID: " << selectedMapId << std::endl;
                currentState = GameState::ShowingMenu;
            }
        } break;
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
        } break;
        case GameState::Restarting:
            std::cout << "Restarting level: " << selectedMapId << std::endl;
            currentState = GameState::Playing;
            break;
        case GameState::SettingsScreen:
            if (SoundManager::getCurrentTrackPath() != MENU_MUSIC_PATH) {
                SoundManager::playBackgroundMusic(MENU_MUSIC_PATH, MENU_MUSIC_VOLUME);
            }
            currentState = showSettingsScreen(*window);
            break;
        case GameState::Exiting:
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