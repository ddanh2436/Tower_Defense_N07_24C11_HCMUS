#ifndef CGAME_H
#define CGAME_H

#include "cmap.h"
#include "cenemy.h"
#include "ctower.h"
#include "cbullet.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include <random>
#include <sstream>

const std::string FONT_PATH = "assets/pixel_font.ttf";
const std::string TOWER_TEXTURE_PATH = "assets/4.png";
const std::string BULLET_TEXTURE_PATH = "bullet.png";

struct EnemyType {
    float speed;
    int health;
    float scale;
    int moneyValue;
    std::map<EnemyState, std::map<MovementDirection, std::string>> texturePaths;
    sf::Vector2i frameSize;

    EnemyType()
        : speed(0.0f), health(0), scale(1.0f), moneyValue(0) {
    }
};


class cgame {
public:
    cgame();
    ~cgame();
    void resetGame();
    void handleInput(const sf::Event& event, sf::RenderWindow& window);
    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window);
    void setPaused(bool paused);
    bool isPaused() const;
    bool isGameOver() const;
    const TowerLevelData* getTowerNextLevelData(const std::string& typeId, int currentLevel) const;
    sf::Texture& getTexture(const std::string& texturePath);
private:
    cmap* _map;
    std::vector<cenemy> _enemies;
    std::vector<ctower> _towers;
    std::vector<cbullet> _bullets;
    std::map<std::string, std::vector<TowerLevelData>> _towerBlueprints;
    std::map<std::string, sf::Texture> _textureManager;

    sf::Font _gameFont;
    sf::Text _livesText;
    sf::Text _moneyText;
    sf::Text _waveText;
    sf::Text _messageText;
    sf::Text _timerText;
    sf::Text _costText;
    sf::RectangleShape _uiPanel;
    sf::Texture _heartIcon;
    sf::Texture _coinIcon;
    sf::Texture _waveIcon;
    sf::Sprite _livesIconSprite;
    sf::Sprite _moneyIconSprite;
    sf::Sprite _waveIconSprite;
    sf::Texture _pauseButtonTexture;
    sf::Sprite _pauseButtonSprite;
    sf::Texture _ffButtonTexture;
    sf::Sprite _ffButtonSprite;

    // ---- THAY ĐỔI: Chuyển _sellButton từ Sprite sang RectangleShape để nhất quán với Upgrade ----
    sf::RectangleShape _sellButton;
    sf::Text _sellText;
    sf::Text _sellValueText;
    // ---- KẾT THÚC THAY ĐỔI ----

    int _lives;
    int _money;
    int _currentWave;
    int _enemiesPerWave;
    sf::Time _timeSinceLastSpawn;
    sf::Time _spawnInterval;
    int _enemiesSpawnedThisWave;
    ctower* _selectedTower;
    bool _isGameOver;
    bool _levelWon;
    bool _waveInProgress;
    bool _inIntermission;
    sf::Time _intermissionTime;
    sf::Time _intermissionTimer;

    bool _selectingTowerToBuild;
    std::string _selectedTowerTypeTexturePath;
    sf::Sprite _towerPlacementPreview;
    bool _canPlaceTower;
    std::string _selectedTowerType;

    sf::RectangleShape _upgradeButton;
    sf::Text _upgradeText;
    bool _isUpgradePanelVisible;

    std::vector<EnemyType> _availableEnemyTypes;
    int _currentWaveEnemyTypeIndex;
    std::mt19937 _rng;

    bool _isPaused;
    bool _isFastForward;
    float _gameSpeedMultiplier;

    std::string _dataFilePath;
    std::string _currentMapId;

    void loadFont();
    void setupUI();
    void resetGameStats();
    void startNextWave();
    void spawnEnemy();
    void handleCollisions();
    void updateEnemies(sf::Time deltaTime);
    void updateTowers(sf::Time deltaTime);
    void updateBullets(sf::Time deltaTime);
    void cleanupInactiveObjects();
    void updateTowerPlacementPreview(sf::RenderWindow& window);
    void updateInterMission(sf::Time deltaTime);
    void setupEnemyTypes();
    void setupTowerTypes();
    void handleTowerSelection(const sf::Vector2f& mousePos);
    void handleUpgrade();
    void renderTowerUI(sf::RenderWindow& window);
    void handleSell();
};

#endif // CGAME_H
