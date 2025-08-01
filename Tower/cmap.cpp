﻿#include "cmap.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <fstream>
#include <sstream>
#include <map>
#include <cmath>
#include <functional>
#include <random>
    
int CURRENT_TILE_SIZE = 60; 

static bool isPathTile(const MapTile& tile) {
    return tile.type == TileType::PATH || tile.type == TileType::START || tile.type == TileType::END;
}

// Hàm helper để tải texture
static bool loadTextureSFML(sf::Texture& texture, const std::string& filename, bool smooth = false) {
    if (!texture.loadFromFile(filename)) {
        std::cerr << "Error: Could not load texture: " << filename << std::endl;
        return false;
    }
    texture.setSmooth(smooth);
    return true;
}

cmap::cmap(const std::string& dataFilePath, const std::string& mapId) : _texturesLoaded(false) {
    CURRENT_TILE_SIZE = 60; 

    loadTileTextures();
    loadMapFromTxtFile(dataFilePath, mapId);
    initializeGridFromMapData();
    assignTileTextures();
    calculateEnemyPath(mapId);
}


void cmap::loadMapFromTxtFile(const std::string& filePath, const std::string& mapId) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open data file: " << filePath << std::endl;
        return;
    }

    std::map<std::string, std::function<void(int, int)>> decorationFuncs;
    decorationFuncs["addGrassAt"] = [this](int r, int c) { this->addGrassAt(r, c); };
    decorationFuncs["addBushAt"] = [this](int r, int c) { this->addBushAt(r, c); };
    decorationFuncs["addGrass1At"] = [this](int r, int c) { this->addGrass1At(r, c); };
    decorationFuncs["addBush3At"] = [this](int r, int c) { this->addBush3At(r, c); };
    decorationFuncs["addBush4At"] = [this](int r, int c) { this->addBush4At(r, c); };
    decorationFuncs["addBush5At"] = [this](int r, int c) { this->addBush5At(r, c); };
    decorationFuncs["addShadow6AndTree1At"] = [this](int r, int c) { this->addShadow6AndTree1At(r, c); };
    decorationFuncs["addPlaceForTower1At"] = [this](int r, int c) { this->addPlaceForTower1At(r, c); };
    decorationFuncs["addLog1At"] = [this](int r, int c) { this->addLog1At(r, c); };
    decorationFuncs["addLog2At"] = [this](int r, int c) { this->addLog2At(r, c); };
    decorationFuncs["addLog3At"] = [this](int r, int c) { this->addLog3At(r, c); };
    decorationFuncs["addLog4At"] = [this](int r, int c) { this->addLog4At(r, c); };
    decorationFuncs["addComboLogCampStoneFlowerGrassAt"] = [this](int r, int c) { this->addComboLogCampStoneFlowerGrassAt(r, c); };
    decorationFuncs["addCamp2At"] = [this](int r, int c) { this->addCamp2At(r, c); };
    decorationFuncs["addCamp4At"] = [this](int r, int c) { this->addCamp4At(r, c); };
    decorationFuncs["addCamp5At"] = [this](int r, int c) { this->addCamp5At(r, c); };
    decorationFuncs["addCamp6At"] = [this](int r, int c) { this->addCamp6At(r, c); };
    decorationFuncs["addComboBush1GrassFlowerAt"] = [this](int r, int c) { this->addComboBush1GrassFlowerAt(r, c); };
    decorationFuncs["addFence1At"] = [this](int r, int c) { this->addFence1At(r, c); };
    decorationFuncs["addFence3At"] = [this](int r, int c) { this->addFence1At(r, c); };
    decorationFuncs["addFence5At"] = [this](int r, int c) { this->addFence5At(r, c); };
    decorationFuncs["addFence6At"] = [this](int r, int c) { this->addFence6At(r, c); };
    decorationFuncs["addFence7At"] = [this](int r, int c) { this->addFence7At(r, c); };
    decorationFuncs["addFence9At"] = [this](int r, int c) { this->addFence9At(r, c); };
    decorationFuncs["addFence10At"] = [this](int r, int c) { this->addFence10At(r, c); };
    decorationFuncs["addPointer1At"] = [this](int r, int c) { this->addPointer1At(r, c); };
    decorationFuncs["addPointer4At"] = [this](int r, int c) { this->addPointer4At(r, c); };
    decorationFuncs["addPointer5At"] = [this](int r, int c) { this->addPointer5At(r, c); };
    decorationFuncs["addLamp1At"] = [this](int r, int c) { this->addLamp1At(r, c); };
    decorationFuncs["addLamp2At"] = [this](int r, int c) { this->addLamp2At(r, c); };
    decorationFuncs["addLamp3At"] = [this](int r, int c) { this->addLamp3At(r, c); };
    decorationFuncs["addLamp4At"] = [this](int r, int c) { this->addLamp4At(r, c); };
    decorationFuncs["addLamp5At"] = [this](int r, int c) { this->addLamp5At(r, c); };
    decorationFuncs["addLamp6At"] = [this](int r, int c) { this->addLamp6At(r, c); };
    decorationFuncs["addDirt2At"] = [this](int r, int c) { this->addDirt2At(r, c); };
    decorationFuncs["addDirt6At"] = [this](int r, int c) { this->addDirt6At(r, c); };
    decorationFuncs["addComboFlowerGrassAt"] = [this](int r, int c) { this->addComboFlowerGrassAt(r, c); };
    decorationFuncs["addBox1At"] = [this](int r, int c) { this->addBox1At(r, c); };
    decorationFuncs["addBox3At"] = [this](int r, int c) { this->addBox3At(r, c); };
    decorationFuncs["addStone7At"] = [this](int r, int c) { this->addStone7At(r, c); };
    decorationFuncs["addStone9At"] = [this](int r, int c) { this->addStone9At(r, c); };
    decorationFuncs["addStone12At"] = [this](int r, int c) { this->addStone12At(r, c); };
    decorationFuncs["addStone15At"] = [this](int r, int c) { this->addStone15At(r, c); };
    decorationFuncs["addTree2At"] = [this](int r, int c) { this->addTree2At(r, c); };
    decorationFuncs["addTent1At"] = [this](int r, int c) { this->addTent1At(r, c); };
    decorationFuncs["addTent2At"] = [this](int r, int c) { this->addTent2At(r, c); };
    decorationFuncs["addTent3At"] = [this](int r, int c) { this->addTent3At(r, c); };
    decorationFuncs["addTent4At"] = [this](int r, int c) { this->addTent4At(r, c); };
    decorationFuncs["addHouse1At"] = [this](int r, int c) { this->addHouse1At(r, c); };
    decorationFuncs["addHouse2At"] = [this](int r, int c) { this->addHouse2At(r, c); };
    decorationFuncs["addHouse3At"] = [this](int r, int c) { this->addHouse3At(r, c); };
    decorationFuncs["addHouse4At"] = [this](int r, int c) { this->addHouse4At(r, c); };
    decorationFuncs["addDecor1At"] = [this](int r, int c) { this->addDecor1At(r, c); };
    decorationFuncs["addDecor2At"] = [this](int r, int c) { this->addDecor2At(r, c); };
    decorationFuncs["addDecor3At"] = [this](int r, int c) { this->addDecor3At(r, c); };
    decorationFuncs["addDecor4At"] = [this](int r, int c) { this->addDecor4At(r, c); };
    decorationFuncs["addDecor5At"] = [this](int r, int c) { this->addDecor5At(r, c); };
    decorationFuncs["addDecor6At"] = [this](int r, int c) { this->addDecor6At(r, c); };
    decorationFuncs["addDecor7At"] = [this](int r, int c) { this->addDecor7At(r, c); };
    decorationFuncs["addDecor8At"] = [this](int r, int c) { this->addDecor8At(r, c); };
    decorationFuncs["addDecor9At"] = [this](int r, int c) { this->addDecor9At(r, c); };
    decorationFuncs["addDecor10At"] = [this](int r, int c) { this->addDecor10At(r, c); };
    decorationFuncs["addDecor11At"] = [this](int r, int c) { this->addDecor11At(r, c); };
    decorationFuncs["addDecor12At"] = [this](int r, int c) { this->addDecor12At(r, c); };
    decorationFuncs["addDecor13At"] = [this](int r, int c) { this->addDecor13At(r, c); };
    decorationFuncs["addDecor14At"] = [this](int r, int c) { this->addDecor14At(r, c); };
    decorationFuncs["addDecor15At"] = [this](int r, int c) { this->addDecor15At(r, c); };
    decorationFuncs["addDecor16At"] = [this](int r, int c) { this->addDecor16At(r, c); };
    decorationFuncs["addDecor17At"] = [this](int r, int c) { this->addDecor17At(r, c); };
    decorationFuncs["addMainCharacterHouseAt"] = [this](int r, int c) { this->addMainCharacterHouseAt(r, c); };

    std::string line;
    bool inMapBlock = false, inLayoutSection = false, inDecorationsSection = false;
    std::string startTag = "[" + mapId + "]";
    std::string endTag = "[/" + mapId + "]";

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (line == startTag) { inMapBlock = true; continue; }
        if (line == endTag) break;

        if (inMapBlock) {
            if (line == "layout") { inLayoutSection = true; inDecorationsSection = false; continue; }
            if (line == "endlayout") { inLayoutSection = false; continue; }
            if (line == "decorations") { inDecorationsSection = true; inLayoutSection = false; continue; }
            if (line == "enddecorations") { inDecorationsSection = false; continue; }

            if (inLayoutSection) {
                std::vector<int> row;
                std::stringstream ss(line);
                int tileId;
                while (ss >> tileId) row.push_back(tileId);
                if (!row.empty()) _mapData.push_back(row);
            }
            else if (inDecorationsSection) {
                std::stringstream ss(line);
                std::string decoName;
                int r, c;
                if (ss >> decoName >> r >> c) {
                    if (decorationFuncs.count(decoName)) {
                        decorationFuncs[decoName](r, c);
                    }
                    else {
                        std::cerr << "Warning: Unknown decoration '" << decoName << "' in data file." << std::endl;
                    }
                }
            }
        }
    }
}

void cmap::initializeGridFromMapData() {
    if (_mapData.empty()) {
        std::cerr << "Error: Map data is empty. Cannot initialize grid." << std::endl;
        return;
    }

    int H= static_cast<int>(_mapData.size());
    int W = static_cast<int>(_mapData[0].size());

    _grid.assign(H, std::vector<MapTile>(W));

    bool startSet = false, endSet = false;
    for (int r = 0; r < H; ++r) {
        for (int c = 0; c < W; ++c) {
            int tileID = _mapData[r][c];
            _grid[r][c].originalMapDataValue = tileID;

            if (tileID == 0) {
                _grid[r][c].type = TileType::GRASS;
            }
            else if (tileID >= 1 && tileID < 64) {
                if (c == 0 && !startSet && tileID != 0) {
                    _grid[r][c].type = TileType::START;
                    startSet = true;
                }
                else if (c == W - 1 && !endSet && tileID != 0) {
                    _grid[r][c].type = TileType::END;
                    endSet = true;
                }
                else {
                    _grid[r][c].type = TileType::PATH;
                }
            }
            else {
                _grid[r][c].type = TileType::EMPTY;
            }
        }
    }

    if (!startSet || !endSet) {
        std::cerr << "Warning: Start or End tile not found or set in map data!" << std::endl;
    }
}

void cmap::assignTileTextures() {
    float scaleFactor = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;

    // Sửa vòng lặp để dùng kích thước động
    for (int r = 0; r < getMapHeightTiles(); ++r) {
        for (int c = 0; c < getMapWidthTiles(); ++c) {
            MapTile& currentTile = _grid[r][c];
            currentTile.sprite.setPosition(static_cast<float>(c * CURRENT_TILE_SIZE), static_cast<float>(r * CURRENT_TILE_SIZE));
            currentTile.sprite.setScale(scaleFactor, scaleFactor);

            int tileID = currentTile.originalMapDataValue;
            sf::Texture* texToUse = nullptr;

            switch (tileID) {
            case 0:  texToUse = &_grassTexture; break;
            case 1:  texToUse = &_pathDefaultTexture; break;
            case 2:  texToUse = &_pathStyle2_Texture; break;
            case 3:  texToUse = &_pathStyle3_Texture; break;
            case 5:  texToUse = &_pathStyle5_Texture; break;
            case 6:  texToUse = &_pathStyle6_Texture; break;
            case 8:  texToUse = &_pathStyle8_Texture; break;
            case 13: texToUse = &_pathStyle13_Texture; break;
            case 16: texToUse = &_pathStyle16_Texture; break;
            case 17: texToUse = &_pathStyle17_Texture; break;
            case 20: texToUse = &_pathStyle20_Texture; break;
            case 21: texToUse = &_pathStyle21_Texture; break;
            case 22: texToUse = &_pathStyle22_Texture; break;
            case 24: texToUse = &_pathStyle24_Texture; break;
            case 28: texToUse = &_pathStyle28_Texture; break;
            case 29: texToUse = &_pathStyle29_Texture; break;
            case 50: texToUse = &_pathStyle50_Texture; break;
            case 56: texToUse = &_pathStyle56_Texture; break;
            case 60: texToUse = &_pathStyle60_Texture; break;
            default: texToUse = &_grassTexture; break;
            }

            if (texToUse && texToUse->getSize().x > 0) {
                currentTile.sprite.setTexture(*texToUse);
                currentTile.textureSpecificallyLoaded = true;
            }
        }
    }
}

namespace std {
    template <>
    struct hash<sf::Vector2i> {
        std::size_t operator()(const sf::Vector2i& v) const {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
        }
    };
}
    
void cmap::calculateEnemyPath(const std::string& mapId) {
    if (strcmp(mapId.c_str(), "MAP_1") == 0) {
        _enemyPath = {
             getPixelPosition(8.5f, -1.5f, PositionContext::EnemyPath), 

             getPixelPosition(8.5f, 6.5f),

             getPixelPosition(3.5f, 6.5f),

             getPixelPosition(3.5f, 12.5f),

             getPixelPosition(13.5f, 12.5f),

             getPixelPosition(13.5f, 18.5f),

             getPixelPosition(3.5f, 18.5f),

             getPixelPosition(3.5f, 24.5f),

			 getPixelPosition(8.5f, 24.5f),

             getPixelPosition(8.5f, 25.5f), getPixelPosition(8.5f, 32.5f, PositionContext::EnemyPath)
        };
    }
    else if (strcmp(mapId.c_str(), "MAP_2") == 0) {
        _enemyPath = {
             getPixelPosition(3.5f, -1.5f, PositionContext::EnemyPath), 

             getPixelPosition(3.5f, 26.5f),

             getPixelPosition(8.5f, 26.5f),

             getPixelPosition(8.5f, 4.5f),

             getPixelPosition(13.5f, 4.5f),

             getPixelPosition(13.5f, 32.5f, PositionContext::EnemyPath)
        };
    }
    else if (strcmp(mapId.c_str(), "MAP_3") == 0) {
        _enemyPath = {
             getPixelPosition(3.5f, -1.5f, PositionContext::EnemyPath), 

             getPixelPosition(3.5f, 5.5f), 

			 getPixelPosition(5.5f, 5.5f),

			 getPixelPosition(5.5f, 9.5f),

			 getPixelPosition(7.5f, 9.5f),

			 getPixelPosition(7.5f, 13.5f),

			 getPixelPosition(9.5f, 13.5f),

			 getPixelPosition(9.5f, 17.5f),

			 getPixelPosition(11.5f, 17.5f),

			 getPixelPosition(11.5f, 21.5f),

			 getPixelPosition(13.5f, 21.5f),

             getPixelPosition(13.5f, 25.5f),

			 getPixelPosition(15.5f, 25.5f),

			 getPixelPosition(15.5f, 32.5f, PositionContext::EnemyPath)
        };
    }
    else if (strcmp(mapId.c_str(), "MAP_4") == 0) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::bernoulli_distribution boolRandom(0.5);
        bool randomBool = boolRandom(gen);
        if (randomBool == 1) {
            _enemyPath = {
                 getPixelPosition(8.5f, -1.5f, PositionContext::EnemyPath),

                 getPixelPosition(8.5f, 3.5f),

                 getPixelPosition(2.5f, 3.5f),

                 getPixelPosition(2.5f, 27.5f),

                 getPixelPosition(8.5f, 27.5f),

                 getPixelPosition(8.5f, 32.5f, PositionContext::EnemyPath)
            };
        }
        else {
            _enemyPath = {
                 getPixelPosition(8.5f, -1.5f, PositionContext::EnemyPath),

                 getPixelPosition(8.5f, 3.5f),

                 getPixelPosition(14.5f, 3.5f),

                 getPixelPosition(14.5f, 27.5f),

                 getPixelPosition(8.5f, 27.5f),

                 getPixelPosition(8.5f, 32.5f, PositionContext::EnemyPath)
            };
        }
    }

    std::cout << "TAO DUONG DI THEO DIEM RE. So diem tham chieu: " << _enemyPath.size() << std::endl;
}

void cmap::render(sf::RenderWindow& window) {
    for (int r = 0; r < getMapHeightTiles(); ++r) {
        for (int c = 0; c < getMapWidthTiles(); ++c) {
            const auto& currentTile = _grid[r][c];
            if (currentTile.textureSpecificallyLoaded) {
                window.draw(currentTile.sprite);
            }
        }
    }
    for (const auto& grass : _grasses) { window.draw(grass.sprite); }
    for (const auto& dirt : _dirts) { window.draw(dirt.sprite); }
    for (const auto& grassOverlay : _grassesOverlay) { window.draw(grassOverlay.sprite); }
    for (const auto& flower : _flowers) { window.draw(flower.sprite); }
    for (const auto& placesForTower : _placesForTowers) { window.draw(placesForTower.sprite); }
    for (const auto& log : _logs) { window.draw(log.sprite); }
    for (const auto& bush : _bushes) { window.draw(bush.sprite); }
    for (const auto& stone : _stones) { window.draw(stone.sprite); }
    for (const auto& shadow : _shadows) { window.draw(shadow.sprite); }
    for (const auto& camp : _camps) { window.draw(camp.sprite); }
    for (const auto& fence : _fences) { window.draw(fence.sprite); }
    for (const auto& lamp : _lamps) { window.draw(lamp.sprite); }
    for (const auto& tree : _trees) { window.draw(tree.sprite); }
    for (const auto& pointer : _pointers) { window.draw(pointer.sprite); }
    for (const auto& box : _boxes) { window.draw(box.sprite); }
    for (const auto& tent : _tents) { window.draw(tent.sprite); }
    for (const auto& house : _houses) { window.draw(house.sprite); }
    for (const auto& decor : _decors) { window.draw(decor.sprite); }
}

TileType cmap::getTileType(int row, int col) const {
    if (row >= 0 && row < getMapHeightTiles() && col >= 0 && col < getMapWidthTiles()) {
        return _grid[row][col].type;
    }
    return TileType::EMPTY;
}

bool cmap::isBuildable(int row, int col) const {
    if (row < 0 || row >= getMapHeightTiles() || col < 0 || col >= getMapWidthTiles()) {
        return false;
    }
    return (_grid[row][col].type == TileType::GRASS && !isDecorated(row, col));
}

cpoint cmap::getPixelPosition(float row, float col, PositionContext context) const {
    float yPos = static_cast<float>(row * CURRENT_TILE_SIZE) + (static_cast<float>(CURRENT_TILE_SIZE) / 2.0f);
    float xPos = static_cast<float>(col * CURRENT_TILE_SIZE) + (static_cast<float>(CURRENT_TILE_SIZE) / 2.0f);

    if (context == PositionContext::EnemyPath) {
        if (col < 0) {
            xPos = -static_cast<float>(CURRENT_TILE_SIZE) / 2.0f;
        }
        else if (col >= getMapWidthTiles()) {
            xPos = (static_cast<float>(getMapWidthTiles()) - 0.5f) * CURRENT_TILE_SIZE + CURRENT_TILE_SIZE;
        }
    }
    return cpoint(xPos, yPos);
}

sf::Vector2i cmap::getGridCoordinates(const sf::Vector2f& pixelPosition) const {
    if (CURRENT_TILE_SIZE == 0) return sf::Vector2i(-1, -1);
    int c = static_cast<int>(pixelPosition.x / CURRENT_TILE_SIZE);
    int r = static_cast<int>(pixelPosition.y / CURRENT_TILE_SIZE);
    return sf::Vector2i(c, r);
}

const std::vector<cpoint>& cmap::getEnemyPath() const {
    return _enemyPath;
}

cpoint cmap::getEnemyStartLocation() const {
    if (!_enemyPath.empty()) {
        return _enemyPath[0];
    }
    return getPixelPosition(static_cast<float>(getMapHeightTiles()) / 2.0f, -1.0f);
}

bool cmap::isDecorated(int row, int col) const {
    for (const auto& deco : _bushes) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _grasses) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _trees) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _shadows) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _camps) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _stones) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _lamps) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _pointers) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _boxes) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _flowers) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _dirts) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _placesForTowers) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _logs) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _grassesOverlay) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _fences) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _decors) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _houses) if (deco.col == col && deco.row == row) return true;
    for (const auto& deco : _tents) if (deco.col == col && deco.row == row) return true;
    return false;
}

// Chép toàn bộ các hàm add...At và loadTileTextures của bạn vào đây.

void cmap::loadTileTextures() {
    _texturesLoaded = true;
    if (!loadTextureSFML(_grassTexture, "assets/FieldsTile_38.jpg")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathDefaultTexture, "assets/FieldsTile_01.jpg")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle2_Texture, "assets/FieldsTile_02.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle3_Texture, "assets/FieldsTile_03.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle5_Texture, "assets/FieldsTile_05.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle6_Texture, "assets/FieldsTile_06.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle8_Texture, "assets/FieldsTile_08.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle13_Texture, "assets/FieldsTile_13.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle16_Texture, "assets/FieldsTile_16.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle17_Texture, "assets/FieldsTile_17.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle20_Texture, "assets/FieldsTile_20.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle21_Texture, "assets/FieldsTile_21.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle22_Texture, "assets/FieldsTile_22.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle24_Texture, "assets/FieldsTile_24.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle28_Texture, "assets/FieldsTile_28.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle29_Texture, "assets/FieldsTile_29.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle50_Texture, "assets/FieldsTile_50.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle56_Texture, "assets/FieldsTile_56.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pathStyle60_Texture, "assets/FieldsTile_60.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_bush_Texture, "assets/Bush_10.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_grass_Texture, "assets/Grass.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_bush1_Texture, "assets/BushOverlay_1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_bush2_Texture, "assets/BushOverlay_2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_bush3_Texture, "assets/BushOverlay_3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_bush4_Texture, "assets/BushOverlay_4.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_bush5_Texture, "assets/BushOverlay_5.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_bush6_Texture, "assets/BushOverlay_6.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_grass1_Texture, "assets/GrassOverlay_1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_grass2_Texture, "assets/GrassOverlay_2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_grass3_Texture, "assets/GrassOverlay_3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_tree1_Texture, "assets/Tree1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_tree2_Texture, "assets/Tree2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_shadow1_Texture, "assets/ShadowOverlay_1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_shadow2_Texture, "assets/ShadowOverlay_2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_shadow3_Texture, "assets/ShadowOverlay_3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_shadow4_Texture, "assets/ShadowOverlay_4.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_shadow5_Texture, "assets/ShadowOverlay_5.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_shadow6_Texture, "assets/ShadowOverlay_6.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_lamp1_Texture, "assets/Lamp1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_lamp2_Texture, "assets/Lamp2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_lamp3_Texture, "assets/Lamp3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_lamp4_Texture, "assets/Lamp4.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_lamp5_Texture, "assets/Lamp5.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_lamp6_Texture, "assets/Lamp6.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_log1_Texture, "assets/Log1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_log2_Texture, "assets/Log2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_log3_Texture, "assets/Log3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_log4_Texture, "assets/Log4.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_placeForTower1_Texture, "assets/StoneOverlay_8.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_stone1_Texture, "assets/StoneOverlay_1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_stone2_Texture, "assets/StoneOverlay_2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_stone3_Texture, "assets/StoneOverlay_3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_stone4_Texture, "assets/StoneOverlay_4.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_stone5_Texture, "assets/StoneOverlay_5.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_stone6_Texture, "assets/StoneOverlay_6.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_stone7_Texture, "assets/StoneOverlay_7.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_stone9_Texture, "assets/StoneOverlay_9.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_stone10_Texture, "assets/StoneOverlay_10.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_stone11_Texture, "assets/StoneOverlay_11.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_stone12_Texture, "assets/StoneOverlay_12.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_stone13_Texture, "assets/StoneOverlay_13.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_stone14_Texture, "assets/StoneOverlay_14.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_stone15_Texture, "assets/StoneOverlay_15.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_stone16_Texture, "assets/StoneOverlay_16.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_camp1_Texture, "assets/CampOverlay_1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_camp2_Texture, "assets/CampOverlay_2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_camp3_Texture, "assets/CampOverlay_3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_camp4_Texture, "assets/CampOverlay_4.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_camp5_Texture, "assets/CampOverlay_5.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_camp6_Texture, "assets/CampOverlay_6.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_flower1_Texture, "assets/FlowerOverlay_1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_flower2_Texture, "assets/FlowerOverlay_2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_flower3_Texture, "assets/FlowerOverlay_3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_flower4_Texture, "assets/FlowerOverlay_4.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_flower5_Texture, "assets/FlowerOverlay_5.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_flower6_Texture, "assets/FlowerOverlay_6.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_flower7_Texture, "assets/FlowerOverlay_7.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_flower8_Texture, "assets/FlowerOverlay_8.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_flower9_Texture, "assets/FlowerOverlay_9.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_flower10_Texture, "assets/FlowerOverlay_10.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_grassOverlay1_Texture, "assets/GrassOverlay_1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_grassOverlay2_Texture, "assets/GrassOverlay_2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_grassOverlay3_Texture, "assets/GrassOverlay_3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_grassOverlay4_Texture, "assets/GrassOverlay_4.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_grassOverlay5_Texture, "assets/GrassOverlay_5.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_grassOverlay6_Texture, "assets/GrassOverlay_6.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_fence1_Texture, "assets/FenceOverlay_1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_fence2_Texture, "assets/FenceOverlay_2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_fence3_Texture, "assets/FenceOverlay_3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_fence4_Texture, "assets/FenceOverlay_4.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_fence5_Texture, "assets/FenceOverlay_5.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_fence6_Texture, "assets/FenceOverlay_6.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_fence7_Texture, "assets/FenceOverlay_7.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_fence8_Texture, "assets/FenceOverlay_8.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_fence9_Texture, "assets/FenceOverlay_9.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_fence10_Texture, "assets/FenceOverlay_10.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pointer1_Texture, "assets/PointerOverlay_1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pointer2_Texture, "assets/PointerOverlay_2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pointer3_Texture, "assets/PointerOverlay_3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pointer4_Texture, "assets/PointerOverlay_4.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pointer5_Texture, "assets/PointerOverlay_5.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_pointer6_Texture, "assets/PointerOverlay_6.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_dirt1_Texture, "assets/Dirt1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_dirt2_Texture, "assets/Dirt2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_dirt3_Texture, "assets/Dirt3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_dirt4_Texture, "assets/Dirt4.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_dirt5_Texture, "assets/Dirt5.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_dirt6_Texture, "assets/Dirt6.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_box1_Texture, "assets/Box1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_box2_Texture, "assets/Box2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_box3_Texture, "assets/Box3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_box4_Texture, "assets/Box4.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_tent1_Texture, "assets/Tent1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_tent2_Texture, "assets/Tent2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_tent3_Texture, "assets/Tent3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_tent4_Texture, "assets/Tent4.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_house1_Texture, "assets/House1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_house2_Texture, "assets/House2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_house3_Texture, "assets/House3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_house4_Texture, "assets/House4.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor1_Texture, "assets/Decor1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor2_Texture, "assets/Decor2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor3_Texture, "assets/Decor3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor4_Texture, "assets/Decor4.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor5_Texture, "assets/Decor5.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor6_Texture, "assets/Decor6.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor7_Texture, "assets/Decor7.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor8_Texture, "assets/Decor8.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor9_Texture, "assets/Decor9.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor10_Texture, "assets/Decor10.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor11_Texture, "assets/Decor11.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor12_Texture, "assets/Decor12.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor13_Texture, "assets/Decor13.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor14_Texture, "assets/Decor14.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor15_Texture, "assets/Decor15.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor16_Texture, "assets/Decor16.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_decor17_Texture, "assets/Decor17.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_mainCharacterHouse_Texture, "assets/MainCharacter_House.png")) _texturesLoaded = false;
}

void cmap::addBushAt(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _bushes.emplace_back(_bush_Texture, row, col, scale);
}
void cmap::addBush3At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _bushes.emplace_back(_bush3_Texture, row, col, scale);
}
void cmap::addBush4At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _bushes.emplace_back(_bush4_Texture, row, col, scale);
}
void cmap::addBush5At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _bushes.emplace_back(_bush5_Texture, row, col, scale);
}
void cmap::addGrassAt(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _grasses.emplace_back(_grass_Texture, row, col, scale);
}

void cmap::addGrass1At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _grasses.emplace_back(_grass1_Texture, row, col, scale);
}

void cmap::addShadow6AndTree1At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _trees.emplace_back(_tree1_Texture, row, col, scale);
    _shadows.emplace_back(_shadow6_Texture, row + 1, col - 1, scale + 0.16f);
}

void cmap::addPlaceForTower1At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _placesForTowers.emplace_back(_placeForTower1_Texture, row, col - 1, scale - 0.75f);
}

void cmap::addLog1At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _logs.emplace_back(_log1_Texture, row, col, scale);
}

void cmap::addLog2At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _logs.emplace_back(_log2_Texture, row, col, scale);
}

void cmap::addLog3At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _logs.emplace_back(_log3_Texture, row, col, scale);
}

void cmap::addLog4At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _logs.emplace_back(_log4_Texture, row, col, scale);
}
void cmap::addComboBush1GrassFlowerAt(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _bushes.emplace_back(_bush1_Texture, row, col, scale);
    _grasses.emplace_back(_grassOverlay5_Texture, row, col, scale + 0.2f);
    _flowers.emplace_back(_flower5_Texture, row, col + 1, scale);
    _grasses.emplace_back(_grassOverlay4_Texture, row - 1, col - 1, scale + 0.2f);
    _flowers.emplace_back(_flower10_Texture, row, col - 1, scale);
}

void cmap::addComboLogCampStoneFlowerGrassAt(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _logs.emplace_back(_log3_Texture, row, col - 1, scale - 0.5f);
    _camps.emplace_back(_camp1_Texture, row, col, scale - 0.25f);
    _stones.emplace_back(_stone1_Texture, row, col + 2, scale + 0.2f);
    _grassesOverlay.emplace_back(_grassOverlay1_Texture, row - 1, col - 1, scale);
    _grassesOverlay.emplace_back(_grassOverlay2_Texture, row + 1, col, scale);
    _flowers.emplace_back(_flower1_Texture, row + 1, col + 1, scale);
}

void cmap::addCamp5At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _camps.emplace_back(_camp5_Texture, row, col, scale);
}

void cmap::addFence1At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _fences.emplace_back(_fence1_Texture, row, col, scale);
    _fences.back().sprite.move(0, -25);
}

void cmap::addFence3At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _fences.emplace_back(_fence3_Texture, row, col, scale);
}

void cmap::addFence5At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _fences.emplace_back(_fence5_Texture, row, col, scale);
}

void cmap::addFence6At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _fences.emplace_back(_fence6_Texture, row, col, scale);
}

void cmap::addFence7At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _fences.emplace_back(_fence7_Texture, row, col, scale);
}

void cmap::addFence9At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _fences.emplace_back(_fence9_Texture, row, col, scale);
}

void cmap::addFence10At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _fences.emplace_back(_fence10_Texture, row, col, scale);
}

void cmap::addPointer1At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _pointers.emplace_back(_pointer1_Texture, row, col, scale);
}

void cmap::addPointer4At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _pointers.emplace_back(_pointer4_Texture, row, col, scale);
}

void cmap::addPointer5At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _pointers.emplace_back(_pointer5_Texture, row, col, scale);
}

void cmap::addLamp1At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _lamps.emplace_back(_lamp1_Texture, row, col, scale);
}

void cmap::addLamp2At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _lamps.emplace_back(_lamp2_Texture, row, col, scale);
}

void cmap::addLamp3At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _lamps.emplace_back(_lamp3_Texture, row, col, scale);
}

void cmap::addLamp4At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _lamps.emplace_back(_lamp4_Texture, row, col, scale);
}

void cmap::addLamp5At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _lamps.emplace_back(_lamp5_Texture, row, col, scale);
}

void cmap::addLamp6At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _lamps.emplace_back(_lamp6_Texture, row, col, scale);
}

void cmap::addCamp6At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _camps.emplace_back(_camp6_Texture, row, col, scale);
}

void cmap::addCamp2At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _camps.emplace_back(_camp2_Texture, row, col, scale);
}

void cmap::addCamp4At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _camps.emplace_back(_camp4_Texture, row, col, scale);
}

void cmap::addDirt2At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _dirts.emplace_back(_dirt2_Texture, row, col, scale);
}

void cmap::addDirt6At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _dirts.emplace_back(_dirt6_Texture, row, col, scale);
}

void cmap::addBox1At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _boxes.emplace_back(_box1_Texture, row, col, scale);
}

void cmap::addBox3At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _boxes.emplace_back(_box3_Texture, row, col, scale);
}

void cmap::addStone7At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _stones.emplace_back(_stone7_Texture, row, col, scale);
}

void cmap::addStone9At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _stones.emplace_back(_stone9_Texture, row, col, scale);
}

void cmap::addStone12At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _stones.emplace_back(_stone12_Texture, row, col, scale);
}

void cmap::addStone15At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _stones.emplace_back(_stone15_Texture, row, col, scale);
}

void cmap::addTree2At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _trees.emplace_back(_tree2_Texture, row, col, scale);
}

void cmap::addHouse1At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _houses.emplace_back(_house1_Texture, row, col, scale - 0.5f);
}

void cmap::addHouse2At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _houses.emplace_back(_house2_Texture, row, col, scale - 0.56f);
    _houses.back().sprite.move(0, -20);
}

void cmap::addHouse3At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _houses.emplace_back(_house3_Texture, row, col, scale - 0.6f);
    _houses.back().sprite.move(0, 24);
}

void cmap::addHouse4At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _houses.emplace_back(_house4_Texture, row, col, scale - 0.64f);
}

void cmap::addTent1At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _tents.emplace_back(_tent1_Texture, row, col, scale - 0.5f);
}

void cmap::addTent2At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _tents.emplace_back(_tent2_Texture, row, col, scale - 0.5f);
}

void cmap::addTent3At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _tents.emplace_back(_tent3_Texture, row, col, scale - 0.5f);
}

void cmap::addTent4At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _tents.emplace_back(_tent4_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor1At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _decors.emplace_back(_decor1_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor2At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _decors.emplace_back(_decor2_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor3At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _decors.emplace_back(_decor3_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor4At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _decors.emplace_back(_decor4_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor5At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _decors.emplace_back(_decor5_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor6At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _decors.emplace_back(_decor6_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor7At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _decors.emplace_back(_decor7_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor8At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _decors.emplace_back(_decor8_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor9At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _decors.emplace_back(_decor9_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor10At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _decors.emplace_back(_decor10_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor11At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _decors.emplace_back(_decor11_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor12At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _decors.emplace_back(_decor12_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor13At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _decors.emplace_back(_decor13_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor14At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _trees.emplace_back(_decor14_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor15At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _decors.emplace_back(_decor15_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor16At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _decors.emplace_back(_decor16_Texture, row, col, scale - 0.5f);
}

void cmap::addDecor17At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _decors.emplace_back(_decor17_Texture, row, col, scale - 0.5f);
}
void cmap::addComboFlowerGrassAt(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _flowers.emplace_back(_flower4_Texture, row, col, scale + 0.25f);
    _flowers.emplace_back(_flower1_Texture, row + 1, col - 1, scale + 0.25f);
    _flowers.emplace_back(_flower8_Texture, row - 1, col - 1, scale + 0.25f);
    _flowers.emplace_back(_flower5_Texture, row - 1, col, scale + 0.25f);
    _flowers.emplace_back(_flower10_Texture, row, col + 1, scale + 0.25f);
    _grassesOverlay.emplace_back(_grassOverlay3_Texture, row, col - 1, scale + 0.25f);
    _grassesOverlay.emplace_back(_grassOverlay1_Texture, row + 1, col + 1, scale + 0.25f);
    _grassesOverlay.emplace_back(_grassOverlay4_Texture, row - 1, col + 1, scale + 0.25f);
    _grassesOverlay.emplace_back(_grassOverlay5_Texture, row + 1, col, scale + 0.25f);
}
void cmap::addMainCharacterHouseAt(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _houses.emplace_back(_mainCharacterHouse_Texture, row, col, scale + 0.54f);
}