#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "cgame.h"
#include "Menu.h"
#include "SoundManager.h"

constexpr unsigned int DEFAULT_WINDOW_WIDTH = 1024;
constexpr unsigned int DEFAULT_WINDOW_HEIGHT = 768;
const std::string MENU_MUSIC_PATH = "assets/menu_music.ogg";
const std::string GAME_MUSIC_PATH = "assets/game_music.ogg";
constexpr float MENU_MUSIC_VOLUME = 50.f;
constexpr float GAME_MUSIC_VOLUME = 70.f;

GameState runGame(sf::RenderWindow& window, cgame& gameManager) {
    bool wantsToRestart = true;

    // Vòng lặp này cho phép thực hiện chức năng "Restart"
    while (wantsToRestart) {
        wantsToRestart = false; // Mặc định là không restart, trừ khi người chơi chọn
        gameManager.resetGame();
        sf::Clock clock;

        // Dừng nhạc hiện tại và phát nhạc game
        SoundManager::stopBackgroundMusic();
        if (SoundManager::getGameMusicState()) {
            SoundManager::playBackgroundMusic(GAME_MUSIC_PATH, GAME_MUSIC_VOLUME);
        }

        // Vòng lặp cho một màn chơi
        bool inGame = true;
        while (inGame && window.isOpen()) {
            sf::Time deltaTime = clock.restart();
            sf::Event event;
            SoundManager::update();

            // 1. XỬ LÝ SỰ KIỆN
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    SoundManager::stopBackgroundMusic();
                    return GameState::Exiting;
                }

                // Cho phép nhấn Escape để mở/tắt menu pause
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    // Dòng này đã được sửa trong cgame::handleInput, nhưng để ở đây cũng tốt
                    // để xử lý khi người dùng đang không làm gì cả
                    gameManager.setPaused(true);
                    SoundManager::playSoundEffect("menu_click");
                }

                // Chỉ xử lý input game khi không bị dừng
                if (!gameManager.isPaused()) {
                    gameManager.handleInput(event, window);
                }
            }

            // 2. CẬP NHẬT TRẠNG THÁI GAME
            if (!gameManager.isPaused()) {
                gameManager.update(deltaTime);
            }

            // 3. KIỂM TRA GAME OVER - Logic này được tách ra riêng
            if (gameManager.isGameOver()) {
                SoundManager::stopBackgroundMusic();
                SoundManager::playSoundEffect("game_over");

                // --- Thiết lập UI cho màn hình Game Over ---
                sf::Font font;
                if (!font.loadFromFile("assets/pixel_font.ttf")) { return GameState::ShowingMenu; }

                sf::Vector2f windowCenter = sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
                sf::RectangleShape panel(sf::Vector2f(500, 300));
                panel.setFillColor(sf::Color(0, 0, 0, 180));
                panel.setOutlineColor(sf::Color(255, 255, 255, 100));
                panel.setOutlineThickness(2);
                panel.setOrigin(panel.getSize().x / 2.0f, panel.getSize().y / 2.0f);
                panel.setPosition(windowCenter);

                sf::Text restartButton("Restart", font, 40);
                restartButton.setOrigin(restartButton.getLocalBounds().width / 2, restartButton.getLocalBounds().height / 2);
                restartButton.setPosition(windowCenter.x, windowCenter.y - 40);

                sf::Text menuButton("Back to Menu", font, 40);
                menuButton.setOrigin(menuButton.getLocalBounds().width / 2, menuButton.getLocalBounds().height / 2);
                menuButton.setPosition(windowCenter.x, windowCenter.y + 40);

                bool optionChosen = false;
                while (!optionChosen && window.isOpen()) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    restartButton.setFillColor(sf::Color::White);
                    menuButton.setFillColor(sf::Color::White);
                    if (restartButton.getGlobalBounds().contains(mousePos)) { restartButton.setFillColor(sf::Color::Yellow); }
                    if (menuButton.getGlobalBounds().contains(mousePos)) { menuButton.setFillColor(sf::Color::Yellow); }

                    sf::Event gameOverEvent;
                    while (window.pollEvent(gameOverEvent)) {
                        if (gameOverEvent.type == sf::Event::Closed) { return GameState::Exiting; }
                        if (gameOverEvent.type == sf::Event::MouseButtonPressed && gameOverEvent.mouseButton.button == sf::Mouse::Left) {
                            if (restartButton.getGlobalBounds().contains(mousePos)) {
                                SoundManager::playSoundEffect("menu_click");
                                wantsToRestart = true;
                                optionChosen = true;
                            }
                            if (menuButton.getGlobalBounds().contains(mousePos)) {
                                SoundManager::playSoundEffect("menu_click");
                                return GameState::ShowingMenu;
                            }
                        }
                    }

                    window.clear();
                    gameManager.render(window); // Vẽ màn hình game cuối cùng
                    window.draw(panel);
                    window.draw(restartButton);
                    window.draw(menuButton);
                    window.display();
                }

                inGame = false; // Thoát vòng lặp game
                continue; // Chuyển đến vòng lặp "wantsToRestart" tiếp theo
            }

            // 4. VẼ MỌI THỨ
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
                    wantsToRestart = true;
                    inGame = false; 
                }
                else {
                    return pauseResult; 
                }
            }
            else {
                // Chỉ gọi display nếu game không bị dừng (vì menu dừng đã có display riêng)
                window.display();
            }
        } // Kết thúc vòng lặp chơi game (while inGame)
    } // Kết thúc vòng lặp restart (while wantsToRestart)

    return GameState::ShowingMenu; // Mặc định trả về Menu nếu thoát khỏi các vòng lặp
}
int main() {
    SoundManager::initialize();
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
    std::cout << "Khoi chay Tower Defense SFML." << std::endl;

    // Phát nhạc menu khi bắt đầu
    SoundManager::playBackgroundMusic(MENU_MUSIC_PATH, MENU_MUSIC_VOLUME);

    while (currentState != GameState::Exiting && window->isOpen()) {
        SoundManager::update();

        switch (currentState) {
        case GameState::ShowingMenu:
            std::cout << "Dang hien thi Menu." << std::endl;
            // Đảm bảo nhạc menu đang phát nếu nó không phải là track hiện tại hoặc đang không phát
            if (SoundManager::getCurrentTrackPath() != MENU_MUSIC_PATH ||
                SoundManager::backgroundMusic.getStatus() != sf::Music::Playing) {
                SoundManager::playBackgroundMusic(MENU_MUSIC_PATH, MENU_MUSIC_VOLUME);
            }
            currentState = showMenu(*window);
            break;
        case GameState::Playing:
            std::cout << "Chuyen sang trang thai Playing Game." << std::endl;
            // SoundManager::stopBackgroundMusic(); // Đã gọi trong runGame
            gameManager->resetGame();
            currentState = runGame(*window, *gameManager);
            // Khi quay lại từ runGame, vòng lặp sẽ tự xử lý nhạc cho trạng thái mới (ví dụ: ShowingMenu)
            break;
        case GameState::Restarting: 
            std::cout << "Chuyen sang trang thai Playing Game." << std::endl;
            gameManager->resetGame();
            currentState = runGame(*window, *gameManager);
            break;
        case GameState::SettingsScreen:
            std::cout << "Chuyen sang trang thai Settings." << std::endl;
            // Đảm bảo nhạc menu đang phát cho màn hình settings
            if (SoundManager::getCurrentTrackPath() != MENU_MUSIC_PATH ||
                SoundManager::backgroundMusic.getStatus() != sf::Music::Playing) {
                SoundManager::playBackgroundMusic(MENU_MUSIC_PATH, MENU_MUSIC_VOLUME);
            }
            currentState = showSettingsScreen(*window);
            break;
        case GameState::Exiting:
            std::cout << "Dang chuan bi thoat game..." << std::endl;
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