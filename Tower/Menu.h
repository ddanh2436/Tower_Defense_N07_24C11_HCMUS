#pragma once
#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// Enum được cập nhật để thêm trạng thái chọn map
enum class GameState {
    ShowingMenu,
    ShowingMapSelection, // <-- THÊM MỚI
    Playing,
    SettingsScreen,
    Exiting,
    Paused,
    Restarting
};

// Struct mới để lưu thông tin về một map
struct MapInfo {
    std::string id;
    std::string name;
    std::string dataFile;
};

// Khai báo các hàm menu
GameState showMenu(sf::RenderWindow& window);
// Hàm chọn map mới, trả về ID của map được chọn hoặc chuỗi rỗng
std::string showMapSelectionScreen(sf::RenderWindow& window, const std::vector<MapInfo>& maps);
GameState showSettingsScreen(sf::RenderWindow& window);
GameState showPauseMenu(sf::RenderWindow& window);

#endif // MENU_H