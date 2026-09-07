#include "GameView.h"

sf::View makeLetterboxView(unsigned int windowWidth, unsigned int windowHeight) {
    sf::View view(sf::FloatRect(0.f, 0.f, VIRTUAL_WIDTH, VIRTUAL_HEIGHT));

    if (windowWidth == 0 || windowHeight == 0) return view;

    const float windowRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    const float viewRatio = VIRTUAL_WIDTH / VIRTUAL_HEIGHT;

    float sizeX = 1.f, sizeY = 1.f, posX = 0.f, posY = 0.f;
    if (windowRatio > viewRatio) {
        // Window is wider than the canvas: bars on the left and right.
        sizeX = viewRatio / windowRatio;
        posX = (1.f - sizeX) / 2.f;
    }
    else {
        // Window is taller than the canvas: bars on the top and bottom.
        sizeY = windowRatio / viewRatio;
        posY = (1.f - sizeY) / 2.f;
    }

    view.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));
    return view;
}

void applyLetterboxView(sf::RenderWindow& window) {
    window.setView(makeLetterboxView(window.getSize().x, window.getSize().y));
}
