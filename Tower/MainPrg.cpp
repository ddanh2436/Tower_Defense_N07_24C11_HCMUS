#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <Windows.h>
#include "cgame.h"
#include "Menu.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "Leaderboard.h"
#include "GameView.h"

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

static std::string getPlayerNameInput(sf::RenderWindow& window, sf::Font& font, cgame& gameManager) {
    std::string playerName;
    sf::Text inputText(playerName + "|", font, 30);
    sf::Text promptText("VICTORY! Enter your name:", font, 40);
    sf::RectangleShape background(sf::Vector2f(viewSizeU(window)));
    background.setFillColor(sf::Color(0, 0, 0, 180));
    promptText.setFillColor(sf::Color::Yellow);
    sf::FloatRect promptBounds = promptText.getLocalBounds();
    promptText.setOrigin(promptBounds.width / 2.f, promptBounds.height / 2.f);
    promptText.setPosition(viewSizeU(window).x / 2.f, viewSizeU(window).y / 2.f - 50);
    inputText.setFillColor(sf::Color::White);
    inputText.setPosition(viewSizeU(window).x / 2.f - 100, viewSizeU(window).y / 2.f + 20);
    sf::Clock cursorClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Keep the 1920x1080 design letterboxed into whatever size the
            // window is; otherwise the layout drifts off screen.
            if (event.type == sf::Event::Resized) applyLetterboxView(window);
            if (event.type == sf::Event::Closed) return "Player";
            if (event.type == sf::Event::KeyPressed &&
                (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Escape)) {
                return playerName.empty() ? "Player" : playerName;
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

        window.clear(sf::Color(25, 25, 25));
        gameManager.render(window);
        window.draw(background);
        window.draw(promptText);
        window.draw(inputText);
        window.display();
    }
    return "Player";
}


// Menu shown on the end-of-level overlay. Kept next to runGame so the event
// handling and the drawing code can never disagree about the item list.
static std::vector<std::string> endScreenItems(bool isWin) {
    if (isWin) return { "Next Level", "Restart", "Quit to Menu" };
    return { "Restart", "Quit to Menu" };
}

static sf::Vector2f endScreenItemPos(const sf::RenderWindow& window, size_t index) {
    return sf::Vector2f(viewSizeU(window).x / 2.f, viewSizeU(window).y / 2.f + 60.f + index * 50.f);
}

static GameState runGame(sf::RenderWindow& window, cgame& gameManager, Leaderboard& leaderboard) {
    sf::Clock clock;
    SoundManager::stopBackgroundMusic();
    if (SoundManager::getGameMusicState()) {
        SoundManager::playBackgroundMusic(GAME_MUSIC_PATH, GAME_MUSIC_VOLUME);
    }

    // All end-screen resources are loaded once here. They used to be reloaded
    // from disk on every frame *and* on every event, which stalled the game.
    sf::Font pixelFont;
    if (!pixelFont.loadFromFile("assets/pixel_font.ttf")) {
        std::cerr << "Error: Could not load assets/pixel_font.ttf" << std::endl;
        return GameState::ShowingMenu;
    }
    sf::Texture starAchievedTexture, starEmptyTexture, arrowTexture;
    const bool starTexturesLoaded = starAchievedTexture.loadFromFile("assets/star.png") &&
        starEmptyTexture.loadFromFile("assets/star_field.png");
    const bool arrowTextureLoaded = arrowTexture.loadFromFile("assets/pixel_arrow.png");

    bool scoreHasBeenSaved = false;
    // This index used to be static, so picking "Quit to Menu" on the 3-item win
    // screen left it at 2 for the next 2-item lose screen and indexed out of
    // range. It is per-run now, and clamped below as well.
    int selectedItemIndex = 0;
    bool endScreenWasShown = false;
    sf::Clock fadeClock;

    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        sf::Event event;
        SoundManager::update();

        const bool gameHasEnded = gameManager.isGameOver();
        const bool isWin = gameManager.hasWon();
        const std::vector<std::string> menuStrings = endScreenItems(isWin);
        if (gameHasEnded && !endScreenWasShown) {
            endScreenWasShown = true;
            selectedItemIndex = 0;
            fadeClock.restart();
        }
        if (selectedItemIndex >= static_cast<int>(menuStrings.size())) selectedItemIndex = 0;

        while (window.pollEvent(event)) {
            // Keep the 1920x1080 design letterboxed into whatever size the
            // window is; otherwise the layout drifts off screen.
            if (event.type == sf::Event::Resized) applyLetterboxView(window);
            if (event.type == sf::Event::Closed) return GameState::ConfirmExit;
            if (event.type == sf::Event::Resized) applyLetterboxView(window);

            if (!gameHasEnded && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                // Let the game consume ESC first (it cancels a pending build or
                // an open tower panel); only pause when it has nothing to cancel.
                if (!gameManager.cancelPendingAction()) {
                    gameManager.setPaused(true);
                    SoundManager::playSoundEffect("assets/menu_click.ogg");
                }
                continue;
            }

            if (!gameManager.isPaused() && !gameHasEnded) {
                gameManager.handleInput(event, window);
                continue;
            }

            if (!gameHasEnded) continue;

            const int menuItemCount = static_cast<int>(menuStrings.size());
            auto activate = [&](int index) -> GameState {
                const std::string& choice = menuStrings[index];
                if (choice == "Next Level") return GameState::GoToNextLevel;
                if (choice == "Restart") return GameState::Restarting;
                return GameState::ShowingMenu;
            };

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
                    SoundManager::playSoundEffect("assets/menu_click.ogg");
                    return activate(selectedItemIndex);
                }
            }
            else if (event.type == sf::Event::MouseMoved || event.type == sf::Event::MouseButtonPressed) {
                const bool isClick = (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left);
                if (event.type == sf::Event::MouseButtonPressed && !isClick) continue;

                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                for (size_t i = 0; i < menuStrings.size(); ++i) {
                    sf::Text tempText(menuStrings[i], pixelFont, 30);
                    sf::FloatRect textRect = tempText.getLocalBounds();
                    tempText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                    tempText.setPosition(endScreenItemPos(window, i));
                    if (tempText.getGlobalBounds().contains(mousePos)) {
                        selectedItemIndex = static_cast<int>(i);
                        if (isClick) {
                            SoundManager::playSoundEffect("assets/menu_click.ogg");
                            return activate(static_cast<int>(i));
                        }
                    }
                }
            }
        }

        if (!gameManager.isPaused() && !gameHasEnded) {
            gameManager.update(deltaTime);
        }

        if (gameManager.hasWon() && !scoreHasBeenSaved) {
            SoundManager::playVictoryMusic();
            std::string playerName = getPlayerNameInput(window, pixelFont, gameManager);
            long score = gameManager.calculateScore();
            int kills = gameManager.getEnemiesDefeated();
            float time = gameManager.getLevelTime().asSeconds();
            leaderboard.addScore(playerName, score, kills, time);
            scoreHasBeenSaved = true;
            fadeClock.restart();
        }

        window.clear(sf::Color(25, 25, 25));
        gameManager.render(window);

        if (gameHasEnded) {
            const sf::Color panelFillColor(25, 40, 80, 230);
            const sf::Color panelOutlineColor(100, 120, 180, 230);
            const sf::Color textColor = sf::Color::White;
            const sf::Color highlightColor = sf::Color::Yellow;
            const sf::Time fadeInDuration = sf::seconds(0.3f);
            const sf::Uint8 textAlpha = 255;

            float alphaRatio = fadeClock.getElapsedTime().asSeconds() / fadeInDuration.asSeconds();
            if (alphaRatio > 1.f) alphaRatio = 1.f;

            sf::Vector2f windowCenter(viewSizeU(window).x / 2.0f, viewSizeU(window).y / 2.0f);

            // Dim the battlefield so the panel reads clearly against it.
            sf::RectangleShape dimmer(sf::Vector2f(viewSizeU(window)));
            dimmer.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(140 * alphaRatio)));
            window.draw(dimmer);

            sf::RectangleShape backgroundPanel;
            backgroundPanel.setSize(sf::Vector2f(460, 430));
            backgroundPanel.setFillColor(sf::Color(panelFillColor.r, panelFillColor.g, panelFillColor.b, static_cast<sf::Uint8>(panelFillColor.a * alphaRatio)));
            backgroundPanel.setOutlineColor(sf::Color(panelOutlineColor.r, panelOutlineColor.g, panelOutlineColor.b, static_cast<sf::Uint8>(panelOutlineColor.a * alphaRatio)));
            backgroundPanel.setOutlineThickness(2.f);
            backgroundPanel.setOrigin(backgroundPanel.getSize().x / 2.f, backgroundPanel.getSize().y / 2.f);
            backgroundPanel.setPosition(windowCenter);

            sf::Text titleText(isWin ? "VICTORY!" : "GAME OVER!", pixelFont, 50);
            titleText.setFillColor(sf::Color(highlightColor.r, highlightColor.g, highlightColor.b, static_cast<sf::Uint8>(textAlpha * alphaRatio)));
            titleText.setStyle(sf::Text::Bold);
            sf::FloatRect titleRect = titleText.getLocalBounds();
            titleText.setOrigin(titleRect.left + titleRect.width / 2.0f, titleRect.top + titleRect.height / 2.0f);
            titleText.setPosition(windowCenter.x, windowCenter.y - 160);

            const int starsToShow = isWin ? determineStars(gameManager) : 0;

            // Run summary, so the player can see what the score was made of.
            std::vector<std::string> summaryLines;
            {
                int totalSeconds = static_cast<int>(gameManager.getLevelTime().asSeconds());
                std::stringstream timeStream;
                timeStream << "Time: " << (totalSeconds / 60) << "m "
                    << std::setw(2) << std::setfill('0') << (totalSeconds % 60) << "s";
                summaryLines.push_back("Enemies defeated: " + std::to_string(gameManager.getEnemiesDefeated()));
                summaryLines.push_back(timeStream.str());
                summaryLines.push_back("Score: " + std::to_string(gameManager.calculateScore()));
            }

            std::vector<sf::Text> menuItems;
            for (size_t i = 0; i < menuStrings.size(); ++i) {
                sf::Text text(menuStrings[i], pixelFont, 30);
                sf::Uint8 currentAlpha = static_cast<sf::Uint8>(textAlpha * alphaRatio);
                if (static_cast<int>(i) == selectedItemIndex) {
                    text.setFillColor(sf::Color(highlightColor.r, highlightColor.g, highlightColor.b, currentAlpha));
                }
                else {
                    text.setFillColor(sf::Color(textColor.r, textColor.g, textColor.b, currentAlpha));
                }
                sf::FloatRect textRect = text.getLocalBounds();
                text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                text.setPosition(endScreenItemPos(window, i));
                menuItems.push_back(text);
            }

            sf::Sprite arrowSprite;
            if (arrowTextureLoaded) {
                arrowSprite.setTexture(arrowTexture);
                float desiredArrowHeight = 30 * 0.8f;
                if (arrowTexture.getSize().y > 0) {
                    float scaleFactor = desiredArrowHeight / arrowTexture.getSize().y;
                    arrowSprite.setScale(scaleFactor, scaleFactor);
                }
                arrowSprite.setColor(sf::Color(highlightColor.r, highlightColor.g, highlightColor.b, static_cast<sf::Uint8>(textAlpha * alphaRatio)));
                if (!menuItems.empty()) {
                    const sf::Text& currentItem = menuItems[selectedItemIndex];
                    sf::FloatRect itemBounds = currentItem.getGlobalBounds();
                    sf::FloatRect arrowBounds = arrowSprite.getGlobalBounds();
                    arrowSprite.setPosition(
                        itemBounds.left - arrowBounds.width - 10.f,
                        itemBounds.top + (itemBounds.height / 2.f) - (arrowBounds.height / 2.f)
                    );
                }
            }

            window.draw(backgroundPanel);
            window.draw(titleText);

            float summaryY = windowCenter.y - 50.f;
            if (isWin && starTexturesLoaded) {
                sf::Sprite starSprite;
                const int totalStars = 3;
                float desiredStarHeight = 48.f;
                starSprite.setTexture(starAchievedTexture);
                float scale = desiredStarHeight / starSprite.getLocalBounds().height;
                starSprite.setScale(scale, scale);
                float starWidth = starSprite.getGlobalBounds().width;
                float spacing = 10.f;
                float totalStarsWidth = (totalStars * starWidth) + ((totalStars - 1) * spacing);
                float startX = windowCenter.x - totalStarsWidth / 2.f;
                float starsY = titleText.getPosition().y + titleText.getGlobalBounds().height / 2.f + 25.f;
                for (int i = 0; i < totalStars; ++i) {
                    starSprite.setTexture(i < starsToShow ? starAchievedTexture : starEmptyTexture);
                    starSprite.setPosition(startX + i * (starWidth + spacing), starsY);
                    starSprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(255 * alphaRatio)));
                    window.draw(starSprite);
                }
                summaryY = starsY + desiredStarHeight + 25.f;
            }

            for (size_t i = 0; i < summaryLines.size(); ++i) {
                sf::Text line(summaryLines[i], pixelFont, 18);
                line.setFillColor(sf::Color(200, 210, 230, static_cast<sf::Uint8>(textAlpha * alphaRatio)));
                sf::FloatRect lineBounds = line.getLocalBounds();
                line.setOrigin(lineBounds.left + lineBounds.width / 2.f, lineBounds.top + lineBounds.height / 2.f);
                line.setPosition(windowCenter.x, summaryY + i * 26.f);
                window.draw(line);
            }

            for (const auto& item : menuItems) window.draw(item);
            if (arrowTextureLoaded && alphaRatio >= 1.f) {
                window.draw(arrowSprite);
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
    SoundManager::loadSoundEffect("assets/tower_place.ogg", "assets/tower_place.ogg");
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
    if (!desktopMode.isValid()) {
        desktopMode = sf::VideoMode(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    }
    auto window = std::make_unique<sf::RenderWindow>(desktopMode, "Tower Defense SFML", sf::Style::Fullscreen);
    window->setFramerateLimit(60);
    applyLetterboxView(*window);

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
        case GameState::ShowingAboutUs: {
            if (SoundManager::getCurrentTrackPath() != MENU_MUSIC_PATH) {
                SoundManager::playBackgroundMusic(MENU_MUSIC_PATH, MENU_MUSIC_VOLUME);
            }
            currentState = showAboutUsScreen(*window);
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
                    gameManager->setupTowerSelectionPanel(*window);
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