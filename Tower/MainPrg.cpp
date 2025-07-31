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

static int determineStars(const cgame& gameManager) {
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

// THAY THẾ TOÀN BỘ HÀM runGame CŨ BẰNG HÀM NÀY

static GameState runGame(sf::RenderWindow& window, cgame& gameManager, Leaderboard& leaderboard) {
    sf::Clock clock;
    SoundManager::stopBackgroundMusic();
    if (SoundManager::getGameMusicState()) {
        SoundManager::playBackgroundMusic(GAME_MUSIC_PATH, GAME_MUSIC_VOLUME);
    }

    bool scoreHasBeenSaved = false;
    static int selectedItemIndex = 0; // Đưa ra ngoài vòng lặp để giữ trạng thái
    static sf::Clock fadeClock;     // Đưa ra ngoài vòng lặp để giữ trạng thái
    fadeClock.restart();            // Restart clock khi vào game

    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        sf::Event event;
        SoundManager::update();

        // --- HỢP NHẤT XỬ LÝ SỰ KIỆN VÀO MỘT VÒNG LẶP DUY NHẤT ---
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) return GameState::ConfirmExit;

            // 1. Xử lý Escape để vào menu Pause
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                gameManager.setPaused(true);
                SoundManager::playSoundEffect("assets/menu_click.ogg");
            }

            // 2. Xử lý input khi game đang chạy
            if (!gameManager.isPaused() && !gameManager.isGameOver()) {
                gameManager.handleInput(event, window);
            }
            // 3. Xử lý input cho màn hình THẮNG/THUA
            else if (gameManager.isGameOver()) {
                bool isWin = gameManager.hasWon();
                int menuItemCount = isWin ? 3 : 2;

                // 3.1. Input bằng bàn phím
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up) {
                        selectedItemIndex = (selectedItemIndex + menuItemCount - 1) % menuItemCount;
                        SoundManager::playSoundEffect("assets/menu_click.ogg");
                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        selectedItemIndex = (selectedItemIndex + 1) % menuItemCount;
                        SoundManager::playSoundEffect("assets/menu_click.ogg");
                    }
                    else if (event.key.code == sf::Keyboard::Return) {
                        if (isWin) {
                            if (selectedItemIndex == 0) return GameState::GoToNextLevel;
                            if (selectedItemIndex == 1) return GameState::Restarting;
                            if (selectedItemIndex == 2) return GameState::ShowingMenu;
                        }
                        else {
                            if (selectedItemIndex == 0) return GameState::Restarting;
                            if (selectedItemIndex == 1) return GameState::ShowingMenu;
                        }
                    }
                }

                // 3.2. Input bằng chuột
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    // Logic click chuột của bạn đã đúng, nhưng để nó ở đây để xử lý tập trung hơn.
                    // Bạn có thể giữ nguyên hoặc di chuyển vào đây. Đoạn code dưới đây chỉ để minh họa.
                    // Để tránh lặp code, ta có thể tạo các sf::Text và kiểm tra bounds ở đây
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
            gameManager.render(window);

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
            sf::Font pixelFont;
            if (!pixelFont.loadFromFile("assets/pixel_font.ttf")) return GameState::ShowingMenu;

            const sf::Color panelFillColor(25, 40, 80, 230);
            const sf::Color panelOutlineColor(100, 120, 180, 230);
            const sf::Color textColor = sf::Color::White;
            const sf::Color highlightColor = sf::Color::Yellow;
            const sf::Time fadeInDuration = sf::seconds(0.3f);
            const sf::Uint8 textAlpha = 255;

            float alphaRatio = fadeClock.getElapsedTime().asSeconds() / fadeInDuration.asSeconds();
            if (alphaRatio > 1.f) alphaRatio = 1.f;

            sf::Vector2f windowCenter(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

            sf::RectangleShape backgroundPanel;
            backgroundPanel.setSize(sf::Vector2f(400, 350));
            backgroundPanel.setFillColor(sf::Color(panelFillColor.r, panelFillColor.g, panelFillColor.b, static_cast<sf::Uint8>(panelFillColor.a * alphaRatio)));
            backgroundPanel.setOutlineColor(sf::Color(panelOutlineColor.r, panelOutlineColor.g, panelOutlineColor.b, static_cast<sf::Uint8>(panelOutlineColor.a * alphaRatio)));
            backgroundPanel.setOutlineThickness(2.f);
            backgroundPanel.setOrigin(backgroundPanel.getSize().x / 2.f, backgroundPanel.getSize().y / 2.f);
            backgroundPanel.setPosition(windowCenter);

            bool isWin = gameManager.hasWon();
            sf::Text titleText(isWin ? "VICTORY!" : "GAME OVER!", pixelFont, 50);
            titleText.setFillColor(sf::Color(highlightColor.r, highlightColor.g, highlightColor.b, static_cast<sf::Uint8>(textAlpha * alphaRatio)));
            titleText.setStyle(sf::Text::Bold);
            sf::FloatRect titleRect = titleText.getLocalBounds();
            titleText.setOrigin(titleRect.left + titleRect.width / 2.0f, titleRect.top + titleRect.height / 2.0f);
            titleText.setPosition(windowCenter.x, windowCenter.y - 120);

            std::vector<std::string> menuStrings;
            if (isWin) {
                menuStrings = { "Next Level", "Restart", "Quit to Menu" };
            }
            else {
                menuStrings = { "Restart", "Quit to Menu" };
            }
            std::vector<sf::Text> menuItems;
            for (size_t i = 0; i < menuStrings.size(); ++i) {
                sf::Text text;
                text.setFont(pixelFont);
                text.setString(menuStrings[i]);
                text.setCharacterSize(30);
                sf::Uint8 currentAlpha = static_cast<sf::Uint8>(textAlpha * alphaRatio);
                if (i == selectedItemIndex && alphaRatio >= 1.f) {
                    text.setFillColor(sf::Color(highlightColor.r, highlightColor.g, highlightColor.b, currentAlpha));
                }
                else {
                    text.setFillColor(sf::Color(textColor.r, textColor.g, textColor.b, currentAlpha));
                }
                sf::FloatRect textRect = text.getLocalBounds();
                text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                text.setPosition(windowCenter.x, windowCenter.y - 20 + i * 70);
                menuItems.push_back(text);
            }

            sf::Texture arrowTexture;
            sf::Sprite arrowSprite;
            bool arrowTextureLoaded = false;
            if (arrowTexture.loadFromFile("assets/pixel_arrow.png")) {
                arrowTextureLoaded = true;
                arrowSprite.setTexture(arrowTexture);
                float desiredArrowHeight = 30 * 0.8f;
                if (arrowTexture.getSize().y > 0) {
                    float scaleFactor = desiredArrowHeight / arrowTexture.getSize().y;
                    arrowSprite.setScale(scaleFactor, scaleFactor);
                }
                arrowSprite.setColor(sf::Color(highlightColor.r, highlightColor.g, highlightColor.b, static_cast<sf::Uint8>(textAlpha * alphaRatio)));
            }

            if (arrowTextureLoaded && !menuItems.empty() && alphaRatio >= 1.f) {
                const sf::Text& currentItem = menuItems[selectedItemIndex];
                sf::FloatRect itemBounds = currentItem.getGlobalBounds();
                sf::FloatRect arrowBounds = arrowSprite.getGlobalBounds();
                arrowSprite.setPosition(
                    itemBounds.left - arrowBounds.width - 10.f,
                    itemBounds.top + (itemBounds.height / 2.f) - (arrowBounds.height / 2.f)
                );
            }

            window.draw(backgroundPanel);
            window.draw(titleText);
            for (const auto& item : menuItems) window.draw(item);
            if (arrowTextureLoaded && alphaRatio >= 1.f) window.draw(arrowSprite);
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
    SoundManager::loadSoundEffect("assets/enemy_explode.ogg", "assets/enemy_explode.ogg");

    std::vector<MapInfo> mapInfos = loadMapInfos("data/maps_index.txt");
    if (mapInfos.empty()) {
        MessageBoxA(NULL, "Could not find or load 'data/maps_index.txt'.\nPlease ensure the file exists and is correctly formatted.", "Fatal Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    auto gameManager = std::make_unique<cgame>();
    Leaderboard leaderboard;

    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

    // Kiểm tra xem chế độ lấy được có hợp lệ không, nếu không thì dùng độ phân giải mặc định
    if (!desktopMode.isValid()) {
        desktopMode = sf::VideoMode(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    }

    // Tạo cửa sổ ở chế độ Fullscreen với độ phân giải đã lấy được
    auto window = std::make_unique<sf::RenderWindow>(desktopMode, "Tower Defense SFML", sf::Style::Fullscreen);
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
                gameManager->setupTowerSelectionPanel(*window);
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