#pragma once
#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp> 
#include <string>           
#include <vector>     

enum class GameState {
    ShowingMenu,
    Playing,
    SettingsScreen,
    Exiting,
    Paused,
    Restarting
};

GameState showMenu(sf::RenderWindow& window);
GameState showSettingsScreen(sf::RenderWindow& window);
GameState showPauseMenu(sf::RenderWindow& window);

#endif // MENU_H