#include "Menu.h"
#include "SoundManager.h"
#include <iostream>
#include <Windows.h>
#include <vector>
#include <string>
#include <iomanip>

#pragma execution_character_set("utf-8")

static void setConsoleToUtf8() {
    SetConsoleOutputCP(CP_UTF8);
}

GameState showMenu(sf::RenderWindow& window) {
    sf::Font pixelFont;
    if (!pixelFont.loadFromFile("assets/pixel_font.ttf")) {
        std::cerr << "Error: Could not load assets/pixel_font.ttf font for menu!" << std::endl;
        if (!pixelFont.loadFromFile("arial.ttf")) {
            std::cerr << "Error: Could not load font Arial.ttf as fallback!" << std::endl;
            return GameState::Exiting;
        }
    }

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("assets/menu_background.png")) {
        std::cerr << "Error: Unable to download menu image: assets/menu_background.png" << std::endl;
    }
    sf::Sprite backgroundSprite(backgroundTexture);
    if (backgroundTexture.getSize().x > 0 && backgroundTexture.getSize().y > 0) {
        backgroundSprite.setScale(
            static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x,
            static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y
        );
    }

    sf::Text gameTitleText;
    gameTitleText.setFont(pixelFont);
    gameTitleText.setString("Tower Defense");
    gameTitleText.setCharacterSize(50);
    gameTitleText.setFillColor(sf::Color::Yellow);
    gameTitleText.setStyle(sf::Text::Bold);

    sf::FloatRect titleRect_showMenu = gameTitleText.getLocalBounds();
    gameTitleText.setOrigin(titleRect_showMenu.left + titleRect_showMenu.width / 2.0f,
        titleRect_showMenu.top + titleRect_showMenu.height / 2.0f);
    gameTitleText.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 5.0f));

    std::vector<sf::Text> menuItems;
    // ================== THÊM MỚI "LEADERBOARD" VÀO MENU ==================
    std::vector<std::string> menuStrings = { "New Game", "Load Game", "Leaderboard", "Settings", "Exit" };
    // ===================================================================

    unsigned int pixelCharSize = 20;
    float itemHeight = static_cast<float>(pixelCharSize) * 1.5f;
    float spacingBetweenItems = static_cast<float>(pixelCharSize) * 1.0f;
    float menuBlockStartY = window.getSize().y / 2.5f;
    float initialY = menuBlockStartY;

    for (size_t i = 0; i < menuStrings.size(); ++i) {
        sf::Text text;
        text.setFont(pixelFont);
        text.setString(menuStrings[i]);
        text.setCharacterSize(pixelCharSize);
        text.setFillColor(sf::Color::White);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
        text.setPosition(sf::Vector2f(window.getSize().x / 2.0f, initialY + i * (itemHeight + spacingBetweenItems)));
        menuItems.push_back(text);
    }

    int selectedItemIndex = 0;
    sf::Texture arrowTexture;
    sf::Sprite arrowSprite;
    bool arrowTextureLoaded = false;
    if (!arrowTexture.loadFromFile("assets/pixel_arrow.png")) {
        std::cerr << "Loi: Khong the tai hinh mui ten: assets/pixel_arrow.png" << std::endl;
    }
    else {
        arrowSprite.setTexture(arrowTexture);
        arrowTextureLoaded = true;
        float desiredArrowHeight = static_cast<float>(pixelCharSize) * 0.8f;
        if (arrowTexture.getSize().y > 0) {
            float scaleFactor = desiredArrowHeight / arrowTexture.getSize().y;
            arrowSprite.setScale(scaleFactor, scaleFactor);
        }
    }
    float arrowOffset = 5.f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return GameState::Exiting;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    if (selectedItemIndex > 0) selectedItemIndex--;
                    else selectedItemIndex = static_cast<int>(menuItems.size() - 1);
                    SoundManager::playSoundEffect("assets/menu_click.ogg");
                }
                else if (event.key.code == sf::Keyboard::Down) {
                    if (selectedItemIndex < static_cast<int>(menuItems.size()) - 1) selectedItemIndex++;
                    else selectedItemIndex = 0;
                    SoundManager::playSoundEffect("assets/menu_click.ogg");
                }
                else if (event.key.code == sf::Keyboard::Return) {
                    SoundManager::playSoundEffect("assets/menu_click.ogg");
                    // ================== XỬ LÝ SỰ KIỆN CHO LEADERBOARD ==================
                    if (menuStrings[selectedItemIndex] == "New Game") return GameState::ShowingMapSelection;
                    if (menuStrings[selectedItemIndex] == "Load Game") return GameState::LoadingGame;
                    if (menuStrings[selectedItemIndex] == "Leaderboard") return GameState::ShowingLeaderboard; // <-- THÊM MỚI
                    if (menuStrings[selectedItemIndex] == "Settings") return GameState::SettingsScreen;
                    if (menuStrings[selectedItemIndex] == "Exit") return GameState::Exiting;
                    // ====================================================================
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    for (size_t i = 0; i < menuItems.size(); ++i) {
                        if (menuItems[i].getGlobalBounds().contains(mousePos)) {
                            selectedItemIndex = static_cast<int>(i);
                            SoundManager::playSoundEffect("assets/menu_click.ogg");
                            // ================== XỬ LÝ SỰ KIỆN CHO LEADERBOARD ==================
                            if (menuStrings[selectedItemIndex] == "New Game") return GameState::ShowingMapSelection;
                            if (menuStrings[selectedItemIndex] == "Load Game") return GameState::LoadingGame;
                            else if (menuStrings[selectedItemIndex] == "Leaderboard") return GameState::ShowingLeaderboard; // <-- THÊM MỚI
                            else if (menuStrings[selectedItemIndex] == "Settings") return GameState::SettingsScreen;
                            else if (menuStrings[selectedItemIndex] == "Exit") return GameState::Exiting;
                            // ====================================================================
                            break;
                        }
                    }
                }
            }
        }

        for (size_t i = 0; i < menuItems.size(); ++i) {
            if (static_cast<int>(i) == selectedItemIndex) {
                menuItems[i].setFillColor(sf::Color::Yellow);
            }
            else {
                menuItems[i].setFillColor(sf::Color::White);
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (menuItems[i].getGlobalBounds().contains(mousePos)) {
                    menuItems[i].setFillColor(sf::Color(220, 220, 180));
                }
            }
        }

        if (arrowTextureLoaded && !menuItems.empty() && selectedItemIndex >= 0 && selectedItemIndex < static_cast<int>(menuItems.size())) {
            const sf::Text& currentItem = menuItems.at(selectedItemIndex);
            sf::FloatRect itemBounds = currentItem.getGlobalBounds();
            sf::FloatRect arrowBounds = arrowSprite.getGlobalBounds();
            arrowSprite.setPosition(
                itemBounds.left - arrowBounds.width - arrowOffset,
                itemBounds.top + (itemBounds.height / 2.f) - (arrowBounds.height / 2.f)
            );
            arrowSprite.setColor(sf::Color::Yellow);
        }

        window.clear();
        if (backgroundTexture.getSize().x > 0) {
            window.draw(backgroundSprite);
        }
        window.draw(gameTitleText);
        for (const auto& item : menuItems) {
            window.draw(item);
        }
        if (arrowTextureLoaded) {
            window.draw(arrowSprite);
        }
        window.display();
    }
    return GameState::Exiting;
}


std::string showMapSelectionScreen(sf::RenderWindow& window, const std::vector<MapInfo>& maps) {
    sf::Font pixelFont;
    if (!pixelFont.loadFromFile("assets/pixel_font.ttf")) {
        return "";
    }

    sf::Text title("Choose a Map", pixelFont, 40);
    title.setFillColor(sf::Color::Yellow);
    title.setOrigin(title.getLocalBounds().width / 2.f, title.getLocalBounds().height / 2.f);
    title.setPosition(window.getSize().x / 2.f, window.getSize().y / 5.f);

    std::vector<sf::Text> mapItems;
    for (const auto& map : maps) {
        sf::Text text(map.name, pixelFont, 25);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        mapItems.push_back(text);
    }

    float startY = window.getSize().y / 2.5f;
    for (size_t i = 0; i < mapItems.size(); ++i) {
        mapItems[i].setPosition(window.getSize().x / 2.f, startY + i * 50.f);
    }

    sf::Text returnHint("Press Esc to go back", pixelFont, 20);
    returnHint.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect hintRect = returnHint.getLocalBounds();
    returnHint.setOrigin(hintRect.left + hintRect.width / 2.f, hintRect.top + hintRect.height / 2.f);
    returnHint.setPosition(window.getSize().x / 2.f, window.getSize().y - 50.f);

    int selectedItemIndex = 0;

    sf::Texture arrowTexture;
    sf::Sprite arrowSprite;
    bool arrowTextureLoaded = false;
    if (!arrowTexture.loadFromFile("assets/pixel_arrow.png")) {
        std::cerr << "Loi: Khong the tai hinh mui ten: assets/pixel_arrow.png" << std::endl;
    }
    else {
        arrowSprite.setTexture(arrowTexture);
        arrowTextureLoaded = true;
        float desiredArrowHeight = 25 * 0.8f;
        if (arrowTexture.getSize().y > 0) {
            float scaleFactor = desiredArrowHeight / arrowTexture.getSize().y;
            arrowSprite.setScale(scaleFactor, scaleFactor);
        }
    }
    float arrowOffset = 10.f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return "";
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    SoundManager::playSoundEffect("assets/menu_click.ogg");
                    return "";
                }
                if (event.key.code == sf::Keyboard::Up) {
                    if (selectedItemIndex > 0) selectedItemIndex--;
                    else selectedItemIndex = static_cast<int>(mapItems.size() - 1);
                    SoundManager::playSoundEffect("assets/menu_click.ogg");
                }
                else if (event.key.code == sf::Keyboard::Down) {
                    if (selectedItemIndex < (int)mapItems.size() - 1) selectedItemIndex++;
                    else selectedItemIndex = 0;
                    SoundManager::playSoundEffect("assets/menu_click.ogg");
                }
                else if (event.key.code == sf::Keyboard::Return) {
                    SoundManager::playSoundEffect("assets/menu_click.ogg");
                    if (!maps.empty()) return maps[selectedItemIndex].id;
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    for (size_t i = 0; i < mapItems.size(); ++i) {
                        if (mapItems[i].getGlobalBounds().contains(mousePos)) {
                            SoundManager::playSoundEffect("assets/menu_click.ogg");
                            return maps[i].id;
                        }
                    }
                }
            }
        }

        sf::Vector2f mousePosView = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        for (size_t i = 0; i < mapItems.size(); ++i) {
            if (mapItems[i].getGlobalBounds().contains(mousePosView)) {
                selectedItemIndex = static_cast<int>(i);
            }
        }

        for (int i = 0; i < (int)mapItems.size(); ++i) {
            if (i == selectedItemIndex) mapItems[i].setFillColor(sf::Color::Yellow);
            else mapItems[i].setFillColor(sf::Color::White);
        }

        if (arrowTextureLoaded && !mapItems.empty()) {
            const sf::Text& currentItem = mapItems[selectedItemIndex];
            sf::FloatRect itemBounds = currentItem.getGlobalBounds();
            sf::FloatRect arrowBounds = arrowSprite.getGlobalBounds();
            arrowSprite.setPosition(
                itemBounds.left - arrowBounds.width - arrowOffset,
                itemBounds.top + (itemBounds.height / 2.f) - (arrowBounds.height / 2.f)
            );
            // <-- SỬA ĐỔI: Tô màu mũi tên thành vàng để đồng bộ -->
            arrowSprite.setColor(sf::Color::Yellow);
        }

        window.clear(sf::Color(15, 30, 50));
        window.draw(title);
        for (const auto& item : mapItems) {
            window.draw(item);
        }
        window.draw(returnHint);
        if (arrowTextureLoaded) {
            window.draw(arrowSprite);
        }
        window.display();
    }

    return "";
}

GameState showSettingsScreen(sf::RenderWindow& window) {
    sf::Font pixelFont;
    if (!pixelFont.loadFromFile("assets/pixel_font.ttf")) {
        std::cerr << "Lỗi: Không thể tải font chữ assets/pixel_font.ttf cho settings!" << std::endl;
        if (!pixelFont.loadFromFile("arial.ttf")) {
            std::cerr << "Lỗi: Không thể tải font Arial.ttf làm fallback!" << std::endl;
            return GameState::ShowingMenu;
        }
    }

    sf::Text settingsTitleText("Settings", pixelFont, 30);
    settingsTitleText.setFillColor(sf::Color::Yellow);
    sf::FloatRect titleRect_showSettings = settingsTitleText.getLocalBounds();
    settingsTitleText.setOrigin(titleRect_showSettings.left + titleRect_showSettings.width / 2.0f, titleRect_showSettings.top + titleRect_showSettings.height / 2.0f);
    settingsTitleText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 7.0f);

    float currentY = window.getSize().y / 3.5f;
    float verticalSpacing = 60.f;
    float labelToControlSpacing = 10.f;
    float controlInternalSpacing = 20.f;
    float buttonSpriteSize = 32.f;
    unsigned int charSizeSettingItems = 20;
    unsigned int charSizeVolumeValue = 22;
    unsigned int charSizePlusMinus = 28;

    float labelRightAlignX = window.getSize().x / 2.f - labelToControlSpacing;
    float controlBlockLeftAlignX = window.getSize().x / 2.f + labelToControlSpacing;

    sf::Text globalSoundLabelText("Global Sound:", pixelFont, charSizeSettingItems);
    globalSoundLabelText.setFillColor(sf::Color::White);
    sf::FloatRect globalLabelRect = globalSoundLabelText.getLocalBounds();
    globalSoundLabelText.setOrigin(globalLabelRect.left + globalLabelRect.width, globalLabelRect.top + globalLabelRect.height / 2.0f);
    globalSoundLabelText.setPosition(labelRightAlignX, currentY);

    sf::Texture soundOnTexture, soundOffTexture;
    bool soundTexturesLoaded = true;
    if (!soundOnTexture.loadFromFile("assets/sound_on.png")) { std::cerr << "Texture error: sound_on.png\n"; soundTexturesLoaded = false; }
    if (!soundOffTexture.loadFromFile("assets/sound_off.png")) { std::cerr << "Texture error: sound_off.png\n"; soundTexturesLoaded = false; }

    sf::Sprite globalSoundButtonSprite;
    if (soundTexturesLoaded) {
        globalSoundButtonSprite.setTexture(SoundManager::getGlobalSoundState() ? soundOnTexture : soundOffTexture);
        if (soundOnTexture.getSize().x > 0 && soundOnTexture.getSize().y > 0) {
            globalSoundButtonSprite.setScale(buttonSpriteSize / soundOnTexture.getSize().x, buttonSpriteSize / soundOnTexture.getSize().y);
        }
        sf::FloatRect globalSpriteBounds = globalSoundButtonSprite.getLocalBounds();
        globalSoundButtonSprite.setOrigin(globalSpriteBounds.left, globalSpriteBounds.top + globalSpriteBounds.height / 2.f);
        globalSoundButtonSprite.setPosition(controlBlockLeftAlignX, currentY);
    }
    currentY += verticalSpacing;

    sf::Text gameMusicLabelText("Music Game:", pixelFont, charSizeSettingItems);
    gameMusicLabelText.setFillColor(sf::Color::White);
    sf::FloatRect gameMusicLabelRect = gameMusicLabelText.getLocalBounds();
    gameMusicLabelText.setOrigin(gameMusicLabelRect.left + gameMusicLabelRect.width, gameMusicLabelRect.top + gameMusicLabelRect.height / 2.0f);
    gameMusicLabelText.setPosition(labelRightAlignX, currentY);

    sf::Sprite gameMusicButtonSprite;
    if (soundTexturesLoaded) {
        gameMusicButtonSprite.setTexture(SoundManager::getGameMusicState() ? soundOnTexture : soundOffTexture);
        if (soundOnTexture.getSize().x > 0 && soundOnTexture.getSize().y > 0) {
            gameMusicButtonSprite.setScale(buttonSpriteSize / soundOnTexture.getSize().x, buttonSpriteSize / soundOnTexture.getSize().y);
        }
        sf::FloatRect gameMusicSpriteBounds = gameMusicButtonSprite.getLocalBounds();
        gameMusicButtonSprite.setOrigin(gameMusicSpriteBounds.left, gameMusicSpriteBounds.top + gameMusicSpriteBounds.height / 2.f);
        gameMusicButtonSprite.setPosition(controlBlockLeftAlignX, currentY);
    }
    currentY += verticalSpacing;

    sf::Text masterVolumeLabelText("Music Volume:", pixelFont, charSizeSettingItems);
    masterVolumeLabelText.setFillColor(sf::Color::White);
    sf::FloatRect masterVolLabelRect = masterVolumeLabelText.getLocalBounds();
    masterVolumeLabelText.setOrigin(masterVolLabelRect.left + masterVolLabelRect.width, masterVolLabelRect.top + masterVolLabelRect.height / 2.0f);
    masterVolumeLabelText.setPosition(labelRightAlignX, currentY);

    sf::Text minusButtonText("-", pixelFont, charSizePlusMinus);
    minusButtonText.setFillColor(sf::Color::White);
    sf::Text volumeValueText(std::to_string(static_cast<int>(SoundManager::getMasterVolumeSetting())), pixelFont, charSizeVolumeValue);
    volumeValueText.setFillColor(sf::Color::Yellow);
    sf::Text plusButtonText("+", pixelFont, charSizePlusMinus);
    plusButtonText.setFillColor(sf::Color::White);

    auto centerTextOrigin = [](sf::Text& text) {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        };

    centerTextOrigin(minusButtonText);
    centerTextOrigin(volumeValueText);
    centerTextOrigin(plusButtonText);

    float currentElementX = controlBlockLeftAlignX;
    minusButtonText.setPosition(currentElementX + minusButtonText.getLocalBounds().width / 2.f, currentY);
    currentElementX += minusButtonText.getLocalBounds().width + controlInternalSpacing;

    volumeValueText.setPosition(currentElementX + volumeValueText.getLocalBounds().width / 2.f, currentY);
    currentElementX += volumeValueText.getLocalBounds().width + controlInternalSpacing;

    plusButtonText.setPosition(currentElementX + plusButtonText.getLocalBounds().width / 2.f, currentY);

    sf::Text returnText("Press Esc to return", pixelFont, 25);
    returnText.setFillColor(sf::Color::Yellow);
    centerTextOrigin(returnText);
    returnText.setPosition(window.getSize().x / 2.0f, window.getSize().y * (4.5f / 5.0f));

    sf::Texture settingsBackgroundTexture;
    sf::Sprite settingsBackgroundSprite;
    bool settingsBgLoaded = false;
    if (settingsBackgroundTexture.loadFromFile("assets/menu_background.png")) {
        settingsBackgroundSprite.setTexture(settingsBackgroundTexture);
        if (settingsBackgroundTexture.getSize().x > 0) {
            settingsBackgroundSprite.setScale(
                static_cast<float>(window.getSize().x) / settingsBackgroundTexture.getSize().x,
                static_cast<float>(window.getSize().y) / settingsBackgroundTexture.getSize().y
            );
            settingsBgLoaded = true;
        }
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return GameState::Exiting;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                SoundManager::playSoundEffect("assets/menu_click.ogg");
                return GameState::ShowingMenu;
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                    if (soundTexturesLoaded && globalSoundButtonSprite.getGlobalBounds().contains(mousePos)) {
                        SoundManager::toggleGlobalSound();
                        globalSoundButtonSprite.setTexture(SoundManager::getGlobalSoundState() ? soundOnTexture : soundOffTexture);
                        SoundManager::playSoundEffect("assets/menu_click.ogg");
                    }
                    else if (soundTexturesLoaded && gameMusicButtonSprite.getGlobalBounds().contains(mousePos)) {
                        SoundManager::toggleGameMusic();
                        gameMusicButtonSprite.setTexture(SoundManager::getGameMusicState() ? soundOnTexture : soundOffTexture);
                        SoundManager::playSoundEffect("assets/menu_click.ogg");
                    }
                    else if (minusButtonText.getGlobalBounds().contains(mousePos)) {
                        SoundManager::adjustMasterVolume(-10.f);
                        volumeValueText.setString(std::to_string(static_cast<int>(SoundManager::getMasterVolumeSetting())));
                        centerTextOrigin(volumeValueText);
                        float newPosX_minus = controlBlockLeftAlignX + minusButtonText.getLocalBounds().width + controlInternalSpacing + volumeValueText.getLocalBounds().width / 2.f;
                        volumeValueText.setPosition(newPosX_minus, minusButtonText.getPosition().y);
                        SoundManager::playSoundEffect("assets/menu_click.ogg");
                    }
                    else if (plusButtonText.getGlobalBounds().contains(mousePos)) {
                        SoundManager::adjustMasterVolume(10.f);
                        volumeValueText.setString(std::to_string(static_cast<int>(SoundManager::getMasterVolumeSetting())));
                        centerTextOrigin(volumeValueText);
                        float newPosX_plus = controlBlockLeftAlignX + minusButtonText.getLocalBounds().width + controlInternalSpacing + volumeValueText.getLocalBounds().width / 2.f;
                        volumeValueText.setPosition(newPosX_plus, plusButtonText.getPosition().y);
                        SoundManager::playSoundEffect("assets/menu_click.ogg");
                    }
                }
            }
        }

        sf::Vector2f mousePosView = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (minusButtonText.getGlobalBounds().contains(mousePosView)) minusButtonText.setFillColor(sf::Color::Yellow);
        else minusButtonText.setFillColor(sf::Color::White);

        if (plusButtonText.getGlobalBounds().contains(mousePosView)) plusButtonText.setFillColor(sf::Color::Yellow);
        else plusButtonText.setFillColor(sf::Color::White);


        window.clear();
        if (settingsBgLoaded) {
            window.draw(settingsBackgroundSprite);
        }
        else {
            window.clear(sf::Color(30, 70, 30));
        }

        window.draw(settingsTitleText);
        window.draw(globalSoundLabelText);
        if (soundTexturesLoaded) window.draw(globalSoundButtonSprite);

        window.draw(gameMusicLabelText);
        if (soundTexturesLoaded) window.draw(gameMusicButtonSprite);

        window.draw(masterVolumeLabelText);
        window.draw(minusButtonText);
        window.draw(volumeValueText);
        window.draw(plusButtonText);

        window.draw(returnText);
        window.display();
    }
    return GameState::ShowingMenu;
}

// ========================================================================================
// HÀM showPauseMenu ĐÃ ĐƯỢC CHỈNH SỬA HOÀN TOÀN
// ========================================================================================
GameState showPauseMenu(sf::RenderWindow& window) {
    sf::Font pixelFont;
    if (!pixelFont.loadFromFile("assets/pixel_font.ttf")) {
        std::cerr << "Loi: Khong the tai font cho menu pause!" << std::endl;
        return GameState::Playing;
    }

    // <-- SỬA ĐỔI: Định nghĩa bộ màu mới để dễ quản lý -->
    const sf::Color panelFillColor(25, 40, 80, 230);
    const sf::Color panelOutlineColor(100, 120, 180, 230);
    const sf::Color textColor = sf::Color::White;
    const sf::Color highlightColor = sf::Color::Yellow;

    const sf::Time fadeInDuration = sf::seconds(0.3f);
    const sf::Uint8 textAlpha = 255;

    sf::RectangleShape backgroundPanel;
    backgroundPanel.setSize(sf::Vector2f(400, 350));
    // <-- SỬA ĐỔI: Bắt đầu với màu trong suốt để có hiệu ứng fade-in -->
    backgroundPanel.setFillColor(sf::Color(panelFillColor.r, panelFillColor.g, panelFillColor.b, 0));
    backgroundPanel.setOutlineColor(sf::Color(panelOutlineColor.r, panelOutlineColor.g, panelOutlineColor.b, 0));
    backgroundPanel.setOutlineThickness(2.f);
    backgroundPanel.setOrigin(backgroundPanel.getSize().x / 2.f, backgroundPanel.getSize().y / 2.f);
    backgroundPanel.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);

    sf::Text pauseTitleText("Game Paused", pixelFont, 50);
    pauseTitleText.setFillColor(sf::Color(highlightColor.r, highlightColor.g, highlightColor.b, 0));
    pauseTitleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleRect = pauseTitleText.getLocalBounds();
    pauseTitleText.setOrigin(titleRect.left + titleRect.width / 2.0f, titleRect.top + titleRect.height / 2.0f);
    pauseTitleText.setPosition(backgroundPanel.getPosition().x, backgroundPanel.getPosition().y - 120);

    std::vector<sf::Text> menuItems;
    std::vector<std::string> menuStrings = { "Resume", "Restart", "Quit Game" };
    for (size_t i = 0; i < menuStrings.size(); ++i) {
        sf::Text text;
        text.setFont(pixelFont);
        text.setString(menuStrings[i]);
        text.setCharacterSize(30);
        text.setFillColor(sf::Color(textColor.r, textColor.g, textColor.b, 0));
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        text.setPosition(backgroundPanel.getPosition().x, backgroundPanel.getPosition().y - 20 + i * 70);
        menuItems.push_back(text);
    }

    int selectedItemIndex = 0;
    sf::Clock animationClock;

    sf::Texture arrowTexture;
    sf::Sprite arrowSprite;
    bool arrowTextureLoaded = false;
    if (!arrowTexture.loadFromFile("assets/pixel_arrow.png")) {
        std::cerr << "Loi: Khong the tai hinh mui ten: assets/pixel_arrow.png" << std::endl;
    }
    else {
        arrowSprite.setTexture(arrowTexture);
        arrowTextureLoaded = true;
        float desiredArrowHeight = 30 * 0.8f;
        if (arrowTexture.getSize().y > 0) {
            float scaleFactor = desiredArrowHeight / arrowTexture.getSize().y;
            arrowSprite.setScale(scaleFactor, scaleFactor);
        }
    }
    float arrowOffset = 10.f;

    while (window.isOpen()) {
        float alphaRatio = animationClock.getElapsedTime().asSeconds() / fadeInDuration.asSeconds();
        if (alphaRatio > 1.f) {
            alphaRatio = 1.f;
        }

        // <-- SỬA ĐỔI: Sử dụng bộ màu mới cho hiệu ứng fade-in -->
        backgroundPanel.setFillColor(sf::Color(panelFillColor.r, panelFillColor.g, panelFillColor.b, static_cast<sf::Uint8>(panelFillColor.a * alphaRatio)));
        backgroundPanel.setOutlineColor(sf::Color(panelOutlineColor.r, panelOutlineColor.g, panelOutlineColor.b, static_cast<sf::Uint8>(panelOutlineColor.a * alphaRatio)));
        pauseTitleText.setFillColor(sf::Color(highlightColor.r, highlightColor.g, highlightColor.b, static_cast<sf::Uint8>(textAlpha * alphaRatio)));

        for (auto& item : menuItems) {
            item.setFillColor(sf::Color(textColor.r, textColor.g, textColor.b, static_cast<sf::Uint8>(textAlpha * alphaRatio)));
        }
        if (arrowTextureLoaded) {
            arrowSprite.setColor(sf::Color(highlightColor.r, highlightColor.g, highlightColor.b, static_cast<sf::Uint8>(textAlpha * alphaRatio)));
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { return GameState::ConfirmExit; }
            if (alphaRatio >= 1.f) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape) { return GameState::Playing; }
                    if (event.key.code == sf::Keyboard::Up) { selectedItemIndex = (selectedItemIndex + menuItems.size() - 1) % static_cast<int>(menuItems.size()); SoundManager::playSoundEffect("assets/menu_click.ogg"); }
                    else if (event.key.code == sf::Keyboard::Down) {
                        selectedItemIndex = (selectedItemIndex + 1) % static_cast<int>(menuItems.size());
                        SoundManager::playSoundEffect("assets/menu_click.ogg");
                    }
                    else if (event.key.code == sf::Keyboard::Return) {
                        if (selectedItemIndex == 0) return GameState::Playing;
                        if (selectedItemIndex == 1) return GameState::Restarting;
                        if (selectedItemIndex == 2) return GameState::ConfirmExit;
                    }
                }
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    for (size_t i = 0; i < menuItems.size(); ++i) {
                        if (menuItems[i].getGlobalBounds().contains(mousePos)) {
                            if (i == 0) return GameState::Playing;
                            if (i == 1) return GameState::Restarting;
                            if (i == 2) return GameState::ConfirmExit;
                        }
                    }
                }
            }
        }

        if (alphaRatio >= 1.f) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            for (size_t i = 0; i < menuItems.size(); ++i) { 
                if (menuItems[i].getGlobalBounds().contains(mousePos)) { 
                    selectedItemIndex = static_cast<int>(i);
                } 
            }
        }

        for (size_t i = 0; i < menuItems.size(); i++) {
            sf::Uint8 currentAlpha = menuItems[i].getFillColor().a;
            // <-- SỬA ĐỔI: Sử dụng highlightColor và textColor -->
            if (i == selectedItemIndex && alphaRatio >= 1.f) {
                menuItems[i].setFillColor(sf::Color(highlightColor.r, highlightColor.g, highlightColor.b, currentAlpha));
            }
            else {
                menuItems[i].setFillColor(sf::Color(textColor.r, textColor.g, textColor.b, currentAlpha));
            }
        }

        if (arrowTextureLoaded && !menuItems.empty()) {
            const sf::Text& currentItem = menuItems[selectedItemIndex];
            sf::FloatRect itemBounds = currentItem.getGlobalBounds();
            sf::FloatRect arrowBounds = arrowSprite.getGlobalBounds();
            arrowSprite.setPosition(
                itemBounds.left - arrowBounds.width - arrowOffset,
                itemBounds.top + (itemBounds.height / 2.f) - (arrowBounds.height / 2.f)
            );
        }

        window.draw(backgroundPanel);
        window.draw(pauseTitleText);
        for (const auto& item : menuItems) {
            window.draw(item);
        }

        if (arrowTextureLoaded && alphaRatio >= 1.f) {
            // <-- SỬA ĐỔI: Đảm bảo mũi tên luôn có màu vàng nổi bật -->
            arrowSprite.setColor(highlightColor);
            window.draw(arrowSprite);
        }
        window.display();
    }

    return GameState::Exiting;
}

// ========================================================================================
// HÀM showConfirmExitScreen ĐÃ ĐƯỢC CHỈNH SỬA HOÀN TOÀN
// ========================================================================================
GameState showConfirmExitScreen(sf::RenderWindow& window) {
    sf::Font pixelFont;
    if (!pixelFont.loadFromFile("assets/pixel_font.ttf")) {
        std::cerr << "Loi: Khong the tai font cho menu xac nhan!" << std::endl;
        return GameState::Exiting;
    }

    // <-- SỬA ĐỔI: Định nghĩa bộ màu mới để dễ quản lý -->
    const sf::Color panelFillColor(25, 40, 80, 230);
    const sf::Color panelOutlineColor(100, 120, 180, 230);
    const sf::Color textColor = sf::Color::White;
    const sf::Color highlightColor = sf::Color::Yellow;

    const sf::Time fadeInDuration = sf::seconds(0.3f);
    const sf::Uint8 textAlpha = 255;

    sf::RectangleShape backgroundPanel;
    backgroundPanel.setSize(sf::Vector2f(700, 350));
    // <-- SỬA ĐỔI: Bắt đầu với màu trong suốt để có hiệu ứng fade-in -->
    backgroundPanel.setFillColor(sf::Color(panelFillColor.r, panelFillColor.g, panelFillColor.b, 0));
    backgroundPanel.setOutlineColor(sf::Color(panelOutlineColor.r, panelOutlineColor.g, panelOutlineColor.b, 0));
    backgroundPanel.setOutlineThickness(2.f);
    backgroundPanel.setOrigin(backgroundPanel.getSize().x / 2.f, backgroundPanel.getSize().y / 2.f);
    backgroundPanel.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);

    sf::Text questionText("Do you want to save your progress?", pixelFont, 35);
    questionText.setFillColor(sf::Color(textColor.r, textColor.g, textColor.b, 0));
    sf::FloatRect qRect = questionText.getLocalBounds();
    questionText.setOrigin(qRect.left + qRect.width / 2.f, qRect.top + qRect.height / 2.f);
    questionText.setPosition(backgroundPanel.getPosition().x, backgroundPanel.getPosition().y - 120);

    std::vector<sf::Text> menuItems;
    std::vector<std::string> menuStrings = { "Save & Quit", "Quit without Saving", "Cancel" };
    for (size_t i = 0; i < menuStrings.size(); ++i) {
        sf::Text text;
        text.setFont(pixelFont);
        text.setString(menuStrings[i]);
        text.setCharacterSize(30);
        text.setFillColor(sf::Color(textColor.r, textColor.g, textColor.b, 0));
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        text.setPosition(backgroundPanel.getPosition().x, backgroundPanel.getPosition().y - 20 + i * 70);
        menuItems.push_back(text);
    }

    int selectedItemIndex = 0;
    sf::Clock animationClock;

    sf::Texture arrowTexture;
    sf::Sprite arrowSprite;
    bool arrowTextureLoaded = false;
    if (!arrowTexture.loadFromFile("assets/pixel_arrow.png")) {
        std::cerr << "Loi: Khong the tai hinh mui ten: assets/pixel_arrow.png" << std::endl;
    }
    else {
        arrowSprite.setTexture(arrowTexture);
        arrowTextureLoaded = true;
        float desiredArrowHeight = 30 * 0.8f;
        if (arrowTexture.getSize().y > 0) {
            float scaleFactor = desiredArrowHeight / arrowTexture.getSize().y;
            arrowSprite.setScale(scaleFactor, scaleFactor);
        }
    }
    float arrowOffset = 10.f;

    while (window.isOpen()) {
        float alphaRatio = animationClock.getElapsedTime().asSeconds() / fadeInDuration.asSeconds();
        if (alphaRatio > 1.f) {
            alphaRatio = 1.f;
        }

        // <-- SỬA ĐỔI: Sử dụng bộ màu mới cho hiệu ứng fade-in -->
        backgroundPanel.setFillColor(sf::Color(panelFillColor.r, panelFillColor.g, panelFillColor.b, static_cast<sf::Uint8>(panelFillColor.a * alphaRatio)));
        backgroundPanel.setOutlineColor(sf::Color(panelOutlineColor.r, panelOutlineColor.g, panelOutlineColor.b, static_cast<sf::Uint8>(panelOutlineColor.a * alphaRatio)));
        questionText.setFillColor(sf::Color(textColor.r, textColor.g, textColor.b, static_cast<sf::Uint8>(textAlpha * alphaRatio)));

        for (auto& item : menuItems) {
            item.setFillColor(sf::Color(textColor.r, textColor.g, textColor.b, static_cast<sf::Uint8>(textAlpha * alphaRatio)));
        }
        if (arrowTextureLoaded) {
            arrowSprite.setColor(sf::Color(highlightColor.r, highlightColor.g, highlightColor.b, static_cast<sf::Uint8>(textAlpha * alphaRatio)));
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return GameState::Exiting;
            }

            if (alphaRatio >= 1.f) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        return GameState::Playing;
                    }
                    if (event.key.code == sf::Keyboard::Up) {
                        selectedItemIndex = (selectedItemIndex + static_cast<int>(menuItems.size()) - 1) % static_cast<int>(menuItems.size());
                        SoundManager::playSoundEffect("assets/menu_click.ogg");
                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        selectedItemIndex = static_cast<int>((static_cast<std::size_t>(selectedItemIndex) + 1) % menuItems.size());

                        SoundManager::playSoundEffect("assets/menu_click.ogg");
                    }
                    else if (event.key.code == sf::Keyboard::Return) {
                        if (selectedItemIndex == 0) return GameState::ExitWithSave;
                        if (selectedItemIndex == 1) return GameState::Exiting;
                        if (selectedItemIndex == 2) return GameState::Playing;
                    }
                }
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    for (size_t i = 0; i < menuItems.size(); ++i) {
                        if (menuItems[i].getGlobalBounds().contains(mousePos)) {
                            if (i == 0) return GameState::ExitWithSave;
                            if (i == 1) return GameState::Exiting;
                            if (i == 2) return GameState::Playing;
                        }
                    }
                }
            }
        }

        if (alphaRatio >= 1.f) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            for (size_t i = 0; i < menuItems.size(); ++i) {
                if (menuItems[i].getGlobalBounds().contains(mousePos)) {
                    selectedItemIndex = static_cast<int>(i);
                }
            }
        }

        for (size_t i = 0; i < menuItems.size(); ++i) {
            sf::Uint8 currentAlpha = menuItems[i].getFillColor().a;
            // <-- SỬA ĐỔI: Sử dụng highlightColor và textColor -->
            if (i == selectedItemIndex && alphaRatio >= 1.f) {
                menuItems[i].setFillColor(sf::Color(highlightColor.r, highlightColor.g, highlightColor.b, currentAlpha));
            }
            else {
                menuItems[i].setFillColor(sf::Color(textColor.r, textColor.g, textColor.b, currentAlpha));
            }
        }

        if (arrowTextureLoaded && !menuItems.empty()) {
            const sf::Text& currentItem = menuItems[selectedItemIndex];
            sf::FloatRect itemBounds = currentItem.getGlobalBounds();
            sf::FloatRect arrowBounds = arrowSprite.getGlobalBounds();
            arrowSprite.setPosition(
                itemBounds.left - arrowBounds.width - arrowOffset,
                itemBounds.top + (itemBounds.height / 2.f) - (arrowBounds.height / 2.f)
            );
        }

        window.draw(backgroundPanel);
        window.draw(questionText);
        for (const auto& item : menuItems) {
            window.draw(item);
        }

        if (arrowTextureLoaded && alphaRatio >= 1.f) {
            // <-- SỬA ĐỔI: Đảm bảo mũi tên luôn có màu vàng nổi bật -->
            arrowSprite.setColor(highlightColor);
            window.draw(arrowSprite);
        }

        window.display();
    }
    return GameState::Exiting;
}

GameState showLeaderboardScreen(sf::RenderWindow& window, Leaderboard& leaderboard) {
    // Vòng lặp chính cho màn hình leaderboard
    while (window.isOpen()) {
        sf::Event event;
        // Xử lý sự kiện
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return GameState::Exiting; // Thoát game nếu người dùng đóng cửa sổ
            }
            // Nếu người dùng nhấn phím Escape, quay trở lại menu chính
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                SoundManager::playSoundEffect("assets/menu_click.ogg");
                return GameState::ShowingMenu;
            }
        }

        // Vẽ màn hình
        window.clear(sf::Color(15, 30, 50)); // Màu nền tối
        leaderboard.draw(window); // Gọi hàm draw của lớp Leaderboard
        window.display();
    }

    return GameState::ShowingMenu; // Mặc định quay về menu
}