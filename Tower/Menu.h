#pragma once
#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Leaderboard.h" 

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

struct MapInfo {
    std::string id;
    std::string name;
    std::string dataFile;
};

inline void drawFlag(
    sf::RenderWindow& window,
    sf::Sprite& flagSprite,
    float textX,
    float textY,
    float offsetX
) {
    sf::FloatRect bounds = flagSprite.getGlobalBounds();
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

GameState showMenu(sf::RenderWindow& window);
std::string showMapSelectionScreen(sf::RenderWindow& window, const std::vector<MapInfo>& maps);
GameState showLeaderboardScreen(sf::RenderWindow& window, Leaderboard& leaderboard);
GameState showSettingsScreen(sf::RenderWindow& window);
GameState showPauseMenu(sf::RenderWindow& window);
GameState showConfirmExitScreen(sf::RenderWindow& window);
GameState showAboutUsScreen(sf::RenderWindow& window);


#endif // MENU_H