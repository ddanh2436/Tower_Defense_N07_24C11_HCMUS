#include "TextureManager.h"
#include <iostream>

// Khởi tạo biến static
std::map<std::string, sf::Texture> TextureManager::s_Textures;

sf::Texture& TextureManager::getTexture(const std::string& texturePath) {
    // Tìm kiếm trong map xem texture đã được tải chưa
    auto it = s_Textures.find(texturePath);

    if (it != s_Textures.end()) {
        // Nếu tìm thấy, trả về tham chiếu đến texture đã có
        return it->second;
    }
    else {
        // Nếu không tìm thấy, tải texture mới
        sf::Texture newTexture;
        if (newTexture.loadFromFile(texturePath)) {
            // Lưu texture mới vào map và trả về tham chiếu đến nó
            s_Textures[texturePath] = newTexture;
            return s_Textures.at(texturePath);
        }
        else {
            // In lỗi và trả về một texture trống (gây ra hình vuông trắng)
            // Trong một dự án lớn, bạn có thể muốn tải một texture "lỗi" mặc định
            std::cerr << "Loi: Khong the tai texture: " << texturePath << std::endl;
            // Trả về một texture mới được thêm vào để tránh lỗi tham chiếu
            s_Textures[texturePath] = sf::Texture(); // Thêm texture trống để không bị crash
            return s_Textures.at(texturePath);
        }
    }
}