#pragma once
#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Leaderboard.h" // <-- THÊM MỚI: Include file Leaderboard.h

// Enum được cập nhật để thêm trạng thái chọn map và leaderboard
enum class GameState {
    ShowingMenu,
    ShowingMapSelection,
    ShowingLeaderboard, 
    ShowingAboutUs,
    Playing,
    SettingsScreen,
    Exiting,
    Paused,
    Restarting,
    GoToNextLevel,
    ConfirmExit,
    LoadingGame,
    ExitWithSave
};

// Struct để lưu thông tin về một map
struct MapInfo {
    std::string id;
    std::string name;
    std::string dataFile;
};

// Khai báo một hàm quản lí cờ để add thêm ảnh vào.
inline void drawFlag(
    sf::RenderWindow& window,
    sf::Sprite& flagSprite,
    float textX,
    float textY,
    float offsetX
) {
    // Tính kích thước thật của sprite sau khi scale
    sf::FloatRect bounds = flagSprite.getGlobalBounds();
    // Đặt vị trí: dịch sang phải offsetX, căn giữa theo chiều dọc so với textY
    flagSprite.setPosition(
        textX + offsetX,
        textY - bounds.height / 2.f
    );
    window.draw(flagSprite);
}

std::vector<sf::Vector2f> getFlagPosition(const sf::Vector2u& windowSize, int amount);

namespace MenuResources {
    extern sf::Texture mapSelectionBackgroundTexture;
    extern sf::Sprite mapSelectionBackgroundSprite;
    extern bool resourcesInitialized;

    bool initializeResource();
}

// Khai báo các hàm menu
GameState showMenu(sf::RenderWindow& window);
std::string showMapSelectionScreen(sf::RenderWindow& window, const std::vector<MapInfo>& maps);

// <-- THÊM MỚI: Khai báo hàm cho màn hình leaderboard -->
GameState showLeaderboardScreen(sf::RenderWindow& window, Leaderboard& leaderboard);

GameState showSettingsScreen(sf::RenderWindow& window);
GameState showPauseMenu(sf::RenderWindow& window);
GameState showConfirmExitScreen(sf::RenderWindow& window);
GameState showAboutUsScreen(sf::RenderWindow& window);


#endif // MENU_H