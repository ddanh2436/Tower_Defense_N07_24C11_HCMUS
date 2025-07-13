#ifndef CMAP_H
#define CMAP_H

#include "cpoint.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

const int MAP_TILE_SIZE_FROM_MAP1 = 60;
const int MAP_WIDTH_TILES_FROM_MAP1 = 32;
const int MAP_HEIGHT_TILES_FROM_MAP1 = 18;

extern int CURRENT_TILE_SIZE;


enum class PositionContext {
    TowerPlacement,
    EnemyPath
};

enum class TileType {
    GRASS,  
    PATH,   
    START,  
    END,    
    EMPTY   
};



struct Decoration {
    sf::Sprite sprite;
    int row, col;

    Decoration(const sf::Texture& texture, int r, int c, float scale) : row(r), col(c) {
        sprite.setTexture(texture);
        sprite.setScale(scale, scale);
        sprite.setPosition(static_cast<float>(c * CURRENT_TILE_SIZE), static_cast<float>(r * CURRENT_TILE_SIZE));
    }
};

struct Bush : public Decoration { using Decoration::Decoration; };
struct Grass : public Decoration { using Decoration::Decoration; };
struct Log : public Decoration { using Decoration::Decoration; };
struct Fence : public Decoration { using Decoration::Decoration; };
struct GrassOverlay : public Decoration { using Decoration::Decoration; };
struct Stone : public Decoration { using Decoration::Decoration; };
struct Tree : public Decoration { using Decoration::Decoration; };
struct Flower : public Decoration { using Decoration::Decoration; };
struct Shadow : public Decoration { using Decoration::Decoration; };
struct Pointer : public Decoration { using Decoration::Decoration; };
struct Lamp : public Decoration { using Decoration::Decoration; };
struct Dirt : public Decoration { using Decoration::Decoration; };
struct Camp : public Decoration { using Decoration::Decoration; };
struct Box : public Decoration { using Decoration::Decoration; };
struct PlaceForTower : public Decoration { using Decoration::Decoration; };


struct MapTile {
    sf::Sprite sprite;
    TileType type;
    bool textureSpecificallyLoaded;
    int originalMapDataValue;

    MapTile() : type(TileType::EMPTY), textureSpecificallyLoaded(false), originalMapDataValue(-1) {}
};


class cmap {
private:
    std::vector<std::vector<MapTile>> _grid;

    std::vector<Bush> _bushes;
    std::vector<Grass> _grasses;
    std::vector<Tree> _trees;
    std::vector<Shadow> _shadows;
    std::vector<Camp> _camps;
    std::vector<Stone> _stones;
    std::vector<Lamp> _lamps;
    std::vector<Pointer> _pointers;
    std::vector<Box> _boxes;
    std::vector<Flower> _flowers;
    std::vector<Dirt> _dirts;
    std::vector<PlaceForTower> _placesForTowers;
    std::vector<Log> _logs;
    std::vector<GrassOverlay> _grassesOverlay;
    std::vector<Fence> _fences;
    std::vector<std::vector<int>> _mapData;


    sf::Texture _grassTexture, _wallTexture, _pathDefaultTexture, _pathStyle2_Texture,
        _pathStyle3_Texture, _pathStyle5_Texture, _pathStyle6_Texture, _pathStyle20_Texture,
        _pathStyle21_Texture, _pathStyle24_Texture, _pathStyle28_Texture, _pathStyle56_Texture,
        _pathStyle60_Texture, _pathStyle50_Texture, _pathStyle13_Texture, _pathStyle29_Texture,
        _pathStyle16_Texture, _pathStyle17_Texture, _pathStyle8_Texture, _pathStyle22_Texture,
        _bush_Texture, _grass_Texture, _bush1_Texture, _bush2_Texture, _bush3_Texture,
        _bush4_Texture, _bush5_Texture, _bush6_Texture, _grass1_Texture, _grass2_Texture,
        _grass3_Texture, _tree1_Texture, _tree2_Texture, _placeForTower1_Texture,
        _shadow1_Texture, _shadow2_Texture, _shadow3_Texture, _shadow4_Texture,
        _shadow5_Texture, _shadow6_Texture, _lamp1_Texture, _lamp2_Texture, _lamp3_Texture,
        _lamp4_Texture, _lamp5_Texture, _lamp6_Texture, _log1_Texture, _log2_Texture,
        _log3_Texture, _log4_Texture, _stone1_Texture, _stone2_Texture, _stone3_Texture,
        _stone4_Texture, _stone5_Texture, _stone6_Texture, _stone7_Texture, _stone8_Texture,
        _stone9_Texture, _stone10_Texture, _stone11_Texture, _stone12_Texture,
        _stone13_Texture, _stone14_Texture, _stone15_Texture, _stone16_Texture,
        _camp1_Texture, _camp2_Texture, _camp3_Texture, _camp4_Texture, _camp5_Texture,
        _camp6_Texture, _flower1_Texture, _flower2_Texture, _flower3_Texture,
        _flower4_Texture, _flower5_Texture, _flower6_Texture, _flower7_Texture,
        _flower8_Texture, _flower9_Texture, _flower10_Texture, _grassOverlay1_Texture,
        _grassOverlay2_Texture, _grassOverlay3_Texture, _grassOverlay4_Texture,
        _grassOverlay5_Texture, _grassOverlay6_Texture, _fence1_Texture, _fence2_Texture,
        _fence3_Texture, _fence4_Texture, _fence5_Texture, _fence6_Texture,
        _fence7_Texture, _fence8_Texture, _fence9_Texture, _fence10_Texture,
        _pointer1_Texture, _pointer2_Texture, _pointer3_Texture, _pointer4_Texture,
        _pointer5_Texture, _pointer6_Texture, _dirt1_Texture, _dirt2_Texture,
        _dirt3_Texture, _dirt4_Texture, _dirt5_Texture, _dirt6_Texture,
        _box1_Texture, _box2_Texture, _box3_Texture, _box4_Texture;

    bool _texturesLoaded;
    std::vector<cpoint> _enemyPath; 

    // --- Private Methods ---
    void loadTileTextures();
    void initializeGridFromMapData();
    void assignTileTextures();
    void loadMapFromTxtFile(const std::string& filePath, const std::string& mapId);

    // Helper methods to add decorations
    void addBushAt(int row, int col);
    void addGrassAt(int row, int col);
    void addBush3At(int row, int col);
    void addBush4At(int row, int col);
    void addBush5At(int row, int col);
    void addGrass1At(int row, int col);
    void addShadow6AndTree1At(int row, int col);
    void addPlaceForTower1At(int row, int col);
    void addLog1At(int row, int col);
    void addComboLogCampStoneFlowerGrassAt(int row, int col);
    void addCamp2At(int row, int col);
    void addCamp4At(int row, int col);
    void addCamp5At(int row, int col);
    void addCamp6At(int row, int col);
    void addComboBush1GrassFlowerAt(int row, int col);
    void addFence1At(int row, int col);
    void addFence5At(int row, int col);
    void addFence6At(int row, int col);
    void addFence7At(int row, int col);
    void addFence9At(int row, int col);
    void addFence10At(int row, int col);
    void addPointer1At(int row, int col);
    void addPointer4At(int row, int col);
    void addPointer5At(int row, int col);
    void addLamp1At(int row, int col);
    void addLamp2At(int row, int col);
    void addLamp3At(int row, int col);
    void addLamp4At(int row, int col);
    void addLamp5At(int row, int col);
    void addLamp6At(int row, int col);
    void addDirt2At(int row, int col);
    void addDirt6At(int row, int col);
    void addComboFlowerGrassAt(int row, int col);
    void addBox1At(int row, int col);
    void addBox3At(int row, int col);
    void addStone7At(int row, int col);
    void addStone9At(int row, int col);
    void addStone12At(int row, int col);
    void addTree2At(int row, int col);
public:
    cmap(const std::string& dataFilePath, const std::string& mapId);
    void calculateEnemyPath(const std::string& mapId);
    void render(sf::RenderWindow& window);
    TileType getTileType(int row, int col) const;
    bool isBuildable(int row, int col) const;
    cpoint getPixelPosition(float row, float col, PositionContext context = PositionContext::EnemyPath) const;
    sf::Vector2i getGridCoordinates(const sf::Vector2f& pixelPosition) const;
    const std::vector<cpoint>& getEnemyPath() const;
    cpoint getEnemyStartLocation() const;
    int getMapWidthTiles() const { return _grid.empty() ? 0 : static_cast<int>(_grid[0].size()); }
    int getMapHeightTiles() const { return static_cast<int>(_grid.size()); }
    bool isDecorated(int row, int col) const;
};

#endif // CMAP_H