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
#pragma comment(lib, "user32.lib")

constexpr unsigned int DEFAULT_WINDOW_WIDTH = 1024;
constexpr unsigned int DEFAULT_WINDOW_HEIGHT = 768;
const std::string MENU_MUSIC_PATH = "assets/menu_music.ogg";
const std::string GAME_MUSIC_PATH = "assets/game_music.ogg";
constexpr float MENU_MUSIC_VOLUME = 50.f;
constexpr float GAME_MUSIC_VOLUME = 70.f;

// HÀM MỚI: Đọc danh sách các map từ file index
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


// --- HÀM runGame ĐÃ ĐƯỢC CẬP NHẬT HOÀN CHỈNH ---
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
                SoundManager::playSoundEffect("menu_click");
            }
            if (!gameManager.isPaused()) {
                gameManager.handleInput(event, window);
            }
        }

        if (!gameManager.isPaused()) {
            gameManager.update(deltaTime);
        }

        // --- BẮT ĐẦU KHỐI LOGIC SỬA ĐỔI ---
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

            // --- SỬA ĐỔI: Kiểm tra xem người chơi thắng hay thua ---
            if (gameManager.hasWon()) {
                // --- MÀN HÌNH CHIẾN THẮNG (Code gốc của bạn) ---
                SoundManager::playVictoryMusic();

                sf::Text victoryText("VICTORY!", font, 50);
                victoryText.setFillColor(sf::Color::Yellow);
                victoryText.setOrigin(victoryText.getLocalBounds().width / 2, victoryText.getLocalBounds().height / 2);
                victoryText.setPosition(windowCenter.x, windowCenter.y - 90);

                sf::Text nextButton("Next Level", font, 40);
                nextButton.setOrigin(nextButton.getLocalBounds().width / 2, nextButton.getLocalBounds().height / 2);
                nextButton.setPosition(windowCenter.x, windowCenter.y - 20);

                sf::Text restartButton("Restart", font, 40);
                restartButton.setOrigin(restartButton.getLocalBounds().width / 2, restartButton.getLocalBounds().height / 2);
                restartButton.setPosition(windowCenter.x, windowCenter.y + 40);

                sf::Text quitButton("Quit to Menu", font, 40);
                quitButton.setOrigin(quitButton.getLocalBounds().width / 2, quitButton.getLocalBounds().height / 2);
                quitButton.setPosition(windowCenter.x, windowCenter.y + 100);

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
                            if (nextButton.getGlobalBounds().contains(mousePos)) {
                                SoundManager::playSoundEffect("menu_click");
                                return GameState::GoToNextLevel;
                            }
                            if (restartButton.getGlobalBounds().contains(mousePos)) {
                                SoundManager::playSoundEffect("menu_click");
                                return GameState::Restarting;
                            }
                            if (quitButton.getGlobalBounds().contains(mousePos)) {
                                SoundManager::playSoundEffect("menu_click");
                                return GameState::ShowingMenu;
                            }
                        }
                    }

                    window.clear();
                    gameManager.render(window);
                    window.draw(panel);
                    window.draw(victoryText);
                    window.draw(nextButton);
                    window.draw(restartButton);
                    window.draw(quitButton);
                    window.display();
                }
            }
            else {
                // --- THÊM MỚI: Màn hình thua cuộc ---
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
                            if (restartButton.getGlobalBounds().contains(mousePos)) {
                                SoundManager::playSoundEffect("menu_click");
                                return GameState::Restarting;
                            }
                            if (quitButton.getGlobalBounds().contains(mousePos)) {
                                SoundManager::playSoundEffect("menu_click");
                                return GameState::ShowingMenu;
                            }
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
        // --- KẾT THÚC KHỐI LOGIC SỬA ĐỔI ---

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
                return pauseResult; // Exiting hoặc ShowingMenu
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
    auto window = std::make_unique<sf::RenderWindow>(
        selectedMode,
        "Tower Defense SFML",
        fullscreen ? sf::Style::Fullscreen : sf::Style::Close
    );
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

        case GameState::ShowingMapSelection:
        {
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
        }
        break;

        case GameState::Playing:
        {
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
        }
        break;

        // THÊM MỚI: Case xử lý logic chuyển sang màn chơi tiếp theo
        case GameState::GoToNextLevel:
        {
            // Tìm iterator của map hiện tại trong vector mapInfos
            auto currentMapIt = std::find_if(mapInfos.begin(), mapInfos.end(), [&](const MapInfo& mi) {
                return mi.id == selectedMapId;
                });

            if (currentMapIt != mapInfos.end()) {
                // Di chuyển đến map tiếp theo
                currentMapIt++;

                // Nếu là map cuối cùng, quay lại map đầu tiên
                if (currentMapIt == mapInfos.end()) {
                    currentMapIt = mapInfos.begin();
                    std::cout << "Completed last level. Looping back to first level." << std::endl;
                }

                // Cập nhật ID map được chọn để chơi ở vòng lặp tiếp theo
                selectedMapId = currentMapIt->id;
                std::cout << "Preparing next level: " << selectedMapId << std::endl;

                // Đặt trạng thái thành Playing để bắt đầu màn mới
                currentState = GameState::Playing;
            }
            else {
                // Trường hợp dự phòng nếu không tìm thấy map hiện tại
                std::cerr << "Error: Could not find current map '" << selectedMapId << "' to determine next level. Returning to menu." << std::endl;
                currentState = GameState::ShowingMenu;
            }
        }
        break;

        case GameState::Restarting:
            // Khi Restarting, chúng ta chỉ cần đặt lại trạng thái là Playing.
            // ID map hiện tại (selectedMapId) không đổi.
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