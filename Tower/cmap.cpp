#include "cmap.h"
#include <iostream>
#include <vector>
#include <algorithm>

// Initialize the global tile size variable.
int CURRENT_TILE_SIZE = MAP_TILE_SIZE_FROM_MAP1;

bool loadTextureSFML(sf::Texture& texture, const std::string& filename, bool smooth = false) {
    if (!texture.loadFromFile(filename)) {
        std::cerr << "Error: Could not load texture: " << filename << std::endl;
        return false;
    }
    texture.setSmooth(smooth);
    return true;
}

void cmap::loadTileTextures() {
    _texturesLoaded = true;
    // --- Path Tiles ---
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

    // --- Decorations ---
    if (!loadTextureSFML(_bush_Texture, "assets/Bush_10.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_grass_Texture, "assets/Grass.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_bush1_Texture, "assets/BushOverlay_1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_bush2_Texture, "assets/BushOverlay_2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_bush3_Texture, "assets/BushOverlay_3.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_bush4_Texture, "assets/BushOverlay_4.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_bush5_Texture, "assets/BushOverlay_5.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_bush6_Texture, "assets/BushOverlay_6.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_grass1_Texture, "assets/Grass_1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_grass2_Texture, "assets/Grass_2.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_grass3_Texture, "assets/Grass_3.png")) _texturesLoaded = false;
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
    if (!loadTextureSFML(_flower10_Texture, "assets/FlowerOverlay_10.png")) _texturesLoaded = false; // FIX: Was loading into _flower1_Texture
    if (!loadTextureSFML(_grassOverlay1_Texture, "assets/GrassOverlay_1.png")) _texturesLoaded = false;
    if (!loadTextureSFML(_grassOverlay2_Texture, "assets/GrassOverlay_2.png")) _texturesLoaded = false; // FIX: Was loading into _grassOverlay1_Texture
    if (!loadTextureSFML(_grassOverlay3_Texture, "assets/GrassOverlay_3.png")) _texturesLoaded = false; // FIX: Was loading into _grassOverlay1_Texture
    if (!loadTextureSFML(_grassOverlay4_Texture, "assets/GrassOverlay_4.png")) _texturesLoaded = false; // FIX: Was loading into _grassOverlay1_Texture
    if (!loadTextureSFML(_grassOverlay5_Texture, "assets/GrassOverlay_5.png")) _texturesLoaded = false; // FIX: Was loading into _grassOverlay1_Texture
    if (!loadTextureSFML(_grassOverlay6_Texture, "assets/GrassOverlay_6.png")) _texturesLoaded = false; // FIX: Was loading into _grassOverlay1_Texture
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
}


void cmap::initializeGridFromMapData() {
    for (int r = 0; r < MAP_HEIGHT_TILES_FROM_MAP1; ++r) {
        for (int c = 0; c < MAP_WIDTH_TILES_FROM_MAP1; ++c) {
            int tileID = _mapData[r][c];
            _grid[r][c].originalMapDataValue = tileID;

            // Consolidate all path-like tile IDs into a single PATH type for game logic.
            if (tileID >= 1 && tileID < 64) { // ID 1 and various path styles
                _grid[r][c].type = TileType::PATH;
            }
            else if (tileID == 0) {
                _grid[r][c].type = TileType::GRASS;
            }
            else {
                _grid[r][c].type = TileType::EMPTY; // For any other IDs
            }
        }
    }
}

void cmap::addBushAt(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _bushes.emplace_back(_bush_Texture, row, col, scale);
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
    // FIX C4244: Use float literal '0.16f' instead of double '0.16'
    _shadows.emplace_back(_shadow6_Texture, row + 1, col - 1, scale + 0.16f);
}

void cmap::addPlaceForTower1At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    // FIX C4244: Use float literal '0.75f' instead of double '0.75'
    _placesForTowers.emplace_back(_placeForTower1_Texture, row, col - 1, scale - 0.75f);
}

void cmap::addLog1At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _logs.emplace_back(_log1_Texture, row, col, scale);
}

void cmap::addComboBush1GrassFlowerAt(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _bushes.emplace_back(_bush1_Texture, row, col, scale);
    // FIX C4244: Use float literal '0.2f' instead of double '0.2'
    _grasses.emplace_back(_grassOverlay5_Texture, row, col, scale + 0.2f);
    _flowers.emplace_back(_flower5_Texture, row, col + 1, scale);
    _grasses.emplace_back(_grassOverlay4_Texture, row - 1, col - 1, scale + 0.2f);
    _flowers.emplace_back(_flower10_Texture, row, col - 1, scale);
}

void cmap::addComboLogCampStoneFlowerGrassAt(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    // FIX C4244: Use float literals (e.g., '0.5f') to avoid double-to-float conversion.
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
}

void cmap::addPointer1At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _pointers.emplace_back(_pointer1_Texture, row, col, scale);
}

void cmap::addPointer4At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _pointers.emplace_back(_pointer4_Texture, row, col, scale);
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

void cmap::addCamp6At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _camps.emplace_back(_camp6_Texture, row, col, scale);
}

void cmap::addCamp2At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _camps.emplace_back(_camp2_Texture, row, col, scale);
}

void cmap::addDirt2At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _dirts.emplace_back(_dirt2_Texture, row, col, scale);
}

void cmap::addDirt6At(int row, int col) {
    float scale = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;
    _dirts.emplace_back(_dirt6_Texture, row, col, scale);
}


void cmap::assignTileTextures() {
    float scaleFactor = static_cast<float>(CURRENT_TILE_SIZE) / 32.0f;

    for (int r = 0; r < MAP_HEIGHT_TILES_FROM_MAP1; ++r) {
        for (int c = 0; c < MAP_WIDTH_TILES_FROM_MAP1; ++c) {
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

void cmap::calculateEnemyPath() {
    _enemyPath.clear();
    sf::Vector2i startPos = { -1, -1 };

    // Find the starting PATH tile on the left edge (column 0).
    for (int r = 0; r < MAP_HEIGHT_TILES_FROM_MAP1; ++r) {
        if (_grid[r][0].type == TileType::PATH) {
            startPos = { 0, r };
            break;
        }
    }

    if (startPos.x == -1) {
        std::cerr << "Error: No starting point (TileType::PATH) found for enemy path." << std::endl;
        return;
    }

    std::vector<sf::Vector2i> pathGridCoords;
    bool visited[MAP_HEIGHT_TILES_FROM_MAP1][MAP_WIDTH_TILES_FROM_MAP1] = { {false} };
    sf::Vector2i currentPos = startPos;
    sf::Vector2i prevPos = { startPos.x - 1, startPos.y }; // Pretend we came from the left

    while (currentPos.x != -1) {
        pathGridCoords.push_back(currentPos);
        visited[currentPos.y][currentPos.x] = true;

        // Stop if we've reached the right edge of the map.
        if (currentPos.x == MAP_WIDTH_TILES_FROM_MAP1 - 1) {
            break;
        }

        sf::Vector2i nextPos = { -1, -1 };

        // Prioritize moving straight, then turning, then backtracking as a last resort.
        const sf::Vector2i directionsToTry[] = {
            currentPos - prevPos, // Forward
            { (currentPos - prevPos).y, (currentPos - prevPos).x }, // Turn 1 (Right-hand rule)
            { -(currentPos - prevPos).y, -(currentPos - prevPos).x },// Turn 2 (Left-hand rule)
            { -(currentPos - prevPos).x, -(currentPos - prevPos).y },// Backward (should not happen in simple paths)
        };

        for (const auto& dir : directionsToTry) {
            if (dir.x == 0 && dir.y == 0) continue; // Skip zero vector

            sf::Vector2i candidatePos = currentPos + dir;

            if (candidatePos.y >= 0 && candidatePos.y < MAP_HEIGHT_TILES_FROM_MAP1 &&
                candidatePos.x >= 0 && candidatePos.x < MAP_WIDTH_TILES_FROM_MAP1 &&
                _grid[candidatePos.y][candidatePos.x].type == TileType::PATH &&
                !visited[candidatePos.y][candidatePos.x])
            {
                nextPos = candidatePos;
                break;
            }
        }

        prevPos = currentPos;
        currentPos = nextPos;
    }

    if (pathGridCoords.empty()) {
        std::cerr << "Error: Could not generate a valid path!" << std::endl;
        return;
    }

    // Convert grid coordinates to pixel waypoints for enemies.
    _enemyPath.push_back(getPixelPosition(pathGridCoords.front().y, -1));
    for (const auto& gridCoord : pathGridCoords) {
        _enemyPath.push_back(getPixelPosition(gridCoord.y, gridCoord.x));
    }
    _enemyPath.push_back(getPixelPosition(pathGridCoords.back().y, MAP_WIDTH_TILES_FROM_MAP1));

    // Update the tile types for the start and end points.
    _grid[pathGridCoords.front().y][pathGridCoords.front().x].type = TileType::START;
    _grid[pathGridCoords.back().y][pathGridCoords.back().x].type = TileType::END;

    std::cout << "Successfully calculated enemy path with " << _enemyPath.size() << " waypoints." << std::endl;
}

cmap::cmap() : _texturesLoaded(false) {
    CURRENT_TILE_SIZE = MAP_TILE_SIZE_FROM_MAP1;

    // Initialization sequence
    loadTileTextures();
    initializeGridFromMapData(); // Must run before path calculation
    assignTileTextures();
    calculateEnemyPath();       // Must run after grid initialization

    // Add all decorative elements to the map
    addGrassAt(1, 5);
    addBushAt(2, 10);
    addGrass1At(1, 5);
    addBush5At(2, 10);
    addShadow6AndTree1At(14, 15);
    addPlaceForTower1At(5, 11);
    addPlaceForTower1At(4, 15);
    addPlaceForTower1At(12, 16);
    addPlaceForTower1At(5, 21);
    addLog1At(14, 5);
    addComboLogCampStoneFlowerGrassAt(1, 20);
    addComboLogCampStoneFlowerGrassAt(12, 25);
    addCamp2At(4, 28);
    addCamp5At(5, 27);
    addCamp5At(12, 23);
    addCamp6At(12, 8);
    addComboBush1GrassFlowerAt(12, 3);
    addComboBush1GrassFlowerAt(3, 4);
    addFence1At(7, 0);
    addFence1At(10, 0);
    addPointer1At(10, 30);
    addPointer4At(7, 31);
    addPointer4At(10, 1);
    addLamp1At(2, 13);
    addLamp2At(9, 8);
    addLamp3At(7, 2);
    addLamp4At(2, 6);
    addDirt2At(7, 28);
    addDirt6At(6, 28);
}

/**
 * @brief Renders the entire map, including tiles and all decorations, to the window.
 *
 * @param window The sf::RenderWindow to draw on.
 */
void cmap::render(sf::RenderWindow& window) {
    // Draw base tiles first
    for (int r = 0; r < MAP_HEIGHT_TILES_FROM_MAP1; ++r) {
        for (int c = 0; c < MAP_WIDTH_TILES_FROM_MAP1; ++c) {
            const auto& currentTile = _grid[r][c];
            if (currentTile.textureSpecificallyLoaded) {
                window.draw(currentTile.sprite);
            }
        }
    }

    // Draw decorations in a logical order (e.g., shadows under trees)
    for (const auto& grass : _grasses) { window.draw(grass.sprite); }
    for (const auto& dirt : _dirts) { window.draw(dirt.sprite); }
    for (const auto& grassOverlay : _grassesOverlay) { window.draw(grassOverlay.sprite); }
    for (const auto& flower : _flowers) { window.draw(flower.sprite); }
    for (const auto& placesForTower : _placesForTowers) { window.draw(placesForTower.sprite); }
    for (const auto& log : _logs) { window.draw(log.sprite); }
    for (const auto& bush : _bushes) { window.draw(bush.sprite); }
    for (const auto& stone : _stones) { window.draw(stone.sprite); }
    for (const auto& shadow : _shadows) { window.draw(shadow.sprite); } // Draw shadows before things that cast them
    for (const auto& camp : _camps) { window.draw(camp.sprite); }
    for (const auto& fence : _fences) { window.draw(fence.sprite); }
    for (const auto& lamp : _lamps) { window.draw(lamp.sprite); }
    for (const auto& tree : _trees) { window.draw(tree.sprite); } // Draw trees on top of their shadows
    for (const auto& pointer : _pointers) { window.draw(pointer.sprite); }
}

/**
 * @brief Gets the logical type of a tile at a given grid coordinate.
 * @return The TileType, or TileType::EMPTY if out of bounds.
 */
TileType cmap::getTileType(int row, int col) const {
    if (row >= 0 && row < MAP_HEIGHT_TILES_FROM_MAP1 && col >= 0 && col < MAP_WIDTH_TILES_FROM_MAP1) {
        return _grid[row][col].type;
    }
    return TileType::EMPTY;
}

/**
 * @brief Checks if a tile is a valid location to build a tower.
 * @return true if the tile is GRASS, false otherwise.
 */
bool cmap::isBuildable(int row, int col) const {
    if (row < 0 || row >= MAP_HEIGHT_TILES_FROM_MAP1 || col < 0 || col >= MAP_WIDTH_TILES_FROM_MAP1) {
        return false;
    }
    return _grid[row][col].type == TileType::GRASS;
}

cpoint cmap::getPixelPosition(int row, int col, PositionContext context) const {
    float yPos = static_cast<float>(row * CURRENT_TILE_SIZE) + (static_cast<float>(CURRENT_TILE_SIZE) / 2.0f);
    float xPos = static_cast<float>(col * CURRENT_TILE_SIZE) + (static_cast<float>(CURRENT_TILE_SIZE) / 2.0f);

    if (context == PositionContext::EnemyPath) {
        if (col < 0) { // Off-map start position
            xPos = -static_cast<float>(CURRENT_TILE_SIZE) / 2.0f;
        }
        else if (col >= MAP_WIDTH_TILES_FROM_MAP1) { // Off-map end position
            xPos = (static_cast<float>(MAP_WIDTH_TILES_FROM_MAP1) - 0.5f) * CURRENT_TILE_SIZE + CURRENT_TILE_SIZE;
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
    return getPixelPosition(MAP_HEIGHT_TILES_FROM_MAP1 / 2, -1);
}