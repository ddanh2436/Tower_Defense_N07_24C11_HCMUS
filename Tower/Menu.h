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
    ShowingLeaderboard, // <-- THÊM MỚI
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

// Khai báo các hàm menu
GameState showMenu(sf::RenderWindow& window);
std::string showMapSelectionScreen(sf::RenderWindow& window, const std::vector<MapInfo>& maps);

// <-- THÊM MỚI: Khai báo hàm cho màn hình leaderboard -->
GameState showLeaderboardScreen(sf::RenderWindow& window, Leaderboard& leaderboard);

GameState showSettingsScreen(sf::RenderWindow& window);
GameState showPauseMenu(sf::RenderWindow& window);
GameState showConfirmExitScreen(sf::RenderWindow& window);

#endif // MENU_H