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
std::vector<MapInfo> loadMapInfos(const std::string& indexPath) {
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


GameState runGame(sf::RenderWindow& window, cgame& gameManager) {
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

        if (gameManager.isGameOver()) {
            SoundManager::stopBackgroundMusic();
            SoundManager::playVictoryMusic();

            sf::Font font;
            if (!font.loadFromFile("assets/pixel_font.ttf")) { return GameState::ShowingMenu; }

            sf::Vector2f windowCenter = sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
            sf::RectangleShape panel(sf::Vector2f(500, 350));
            panel.setFillColor(sf::Color(0, 0, 0, 200));
            panel.setOutlineColor(sf::Color(255, 255, 255, 100));
            panel.setOutlineThickness(2);
            panel.setOrigin(panel.getSize().x / 2.0f, panel.getSize().y / 2.0f);
            panel.setPosition(windowCenter);

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

            sf::Text quitButton("Quit", font, 40);
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

                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) return GameState::Exiting;
                    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                        if (nextButton.getGlobalBounds().contains(mousePos)) {
                            SoundManager::playSoundEffect("menu_click");
                            // TODO: Xử lý chuyển sang màn tiếp theo (tăng selectedMapId hoặc chọn map mới)
                            return GameState::ShowingMapSelection;
                        }
                        if (restartButton.getGlobalBounds().contains(mousePos)) {
                            SoundManager::playSoundEffect("menu_click");
                            return GameState::Playing;
                        }
                        if (quitButton.getGlobalBounds().contains(mousePos)) {
                            SoundManager::playSoundEffect("menu_click");
                            return GameState::Exiting;
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
            // SỬA ĐỔI: Restart từ menu pause sẽ quay về màn hình chọn map
            else if (pauseResult == GameState::Restarting) {
                return GameState::Playing;
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

    // THÊM MỚI: Tải thông tin các map trước khi tạo cửa sổ
    std::vector<MapInfo> mapInfos = loadMapInfos("data/maps_index.txt");
    if (mapInfos.empty()) {
        std::cerr << "No maps found or failed to load map index. Exiting." << std::endl;
        // Hiển thị lỗi cho người dùng Windows thấy trước khi tắt
        MessageBoxA(NULL, "Could not find or load 'data/maps_index.txt'.\nPlease ensure the file exists and is correctly formatted.", "Fatal Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    auto gameManager = std::make_unique<cgame>();

    // GIỮ NGUYÊN: Toàn bộ logic fullscreen của bạn được giữ lại
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
    std::string selectedMapId = ""; // THÊM MỚI: Lưu ID map được chọn

    std::cout << "Khoi chay Tower Defense SFML." << std::endl;
    SoundManager::playBackgroundMusic(MENU_MUSIC_PATH, MENU_MUSIC_VOLUME);

    // SỬA ĐỔI: Vòng lặp chính với state machine được cập nhật
    while (currentState != GameState::Exiting && window->isOpen()) {
        SoundManager::update();

        switch (currentState) {
        case GameState::ShowingMenu:
            if (SoundManager::getCurrentTrackPath() != MENU_MUSIC_PATH) {
                SoundManager::playBackgroundMusic(MENU_MUSIC_PATH, MENU_MUSIC_VOLUME);
            }
            currentState = showMenu(*window);
            break;

            // THÊM MỚI: Case xử lý màn hình chọn map
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

        // SỬA ĐỔI: Case playing để tải map trước khi chạy
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

        // Case này không còn cần thiết vì đã được xử lý bởi logic trả về của runGame
        case GameState::Restarting:
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