#pragma once
#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <SFML/Graphics.hpp>

// The whole game (maps, menus, HUD) is authored against this canvas.
// The map itself is 32 x 18 tiles of 60px = exactly 1920 x 1080.
constexpr float VIRTUAL_WIDTH = 1920.f;
constexpr float VIRTUAL_HEIGHT = 1080.f;

// Builds a view that shows the whole virtual canvas inside the real window,
// keeping the aspect ratio and adding black bars where needed.
sf::View makeLetterboxView(unsigned int windowWidth, unsigned int windowHeight);

// Applies that view to the window. Call once after creating the window and
// again on every Resized event.
void applyLetterboxView(sf::RenderWindow& window);

// Layout helper. Screens lay themselves out against the virtual canvas instead
// of the physical window, so they look identical on every monitor.
// Returns Vector2u so it is a drop-in replacement for window.getSize().
inline sf::Vector2u viewSizeU(const sf::RenderWindow& window) {
    const sf::Vector2f size = window.getView().getSize();
    return sf::Vector2u(static_cast<unsigned int>(size.x), static_cast<unsigned int>(size.y));
}

#endif // GAMEVIEW_H
