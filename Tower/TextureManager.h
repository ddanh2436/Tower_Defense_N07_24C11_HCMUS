#pragma once
#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <map>

class TextureManager {
public:
    static sf::Texture& getTexture(const std::string& texturePath);

private:
    static std::map<std::string, sf::Texture> s_Textures;
};

#endif // TEXTURE_MANAGER_H