#include "TextureManager.h"
#include <iostream>

std::map<std::string, sf::Texture> TextureManager::s_Textures;

sf::Texture& TextureManager::getTexture(const std::string& texturePath) {
    auto it = s_Textures.find(texturePath);

    if (it != s_Textures.end()) {
        return it->second;
    }
    else {
        sf::Texture newTexture;
        if (newTexture.loadFromFile(texturePath)) {
            s_Textures[texturePath] = newTexture;
            return s_Textures.at(texturePath);
        }
        else {
            std::cerr << "Loi: Khong the tai texture: " << texturePath << std::endl;
            s_Textures[texturePath] = sf::Texture(); 
            return s_Textures.at(texturePath);
        }
    }
}