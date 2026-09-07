#ifndef CGAME_H
#define CGAME_H

#include "cmap.h"
#include "cenemy.h"
#include "cbullet.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include <random>
#include <sstream>
#include "cbasictower.h"

class cbasictower;
class cenemy;
class cbullet;

const std::string FONT_PATH = "assets/pixel_font.ttf";
const std::string TOWER_TEXTURE_PATH = "assets/4.png";
const std::string BULLET_TEXTURE_PATH = "bullet.png";

struct EnemyType {
    std::string name;
    float speed;
    int health;
    float scale;
    int moneyValue;
    int armour;     // Flat damage soaked per hit.
    int livesCost;  // Lives lost if this one gets through.
    std::map<EnemyState, std::map<MovementDirection, std::string>> texturePaths;

    sf::Vector2i frameSize;
    int frameCount;
    int stride;

    EnemyType()
        : name("Enemy"), speed(0.0f), health(0), scale(1.0f), moneyValue(0),
        armour(0), livesCost(1), frameCount(0), stride(0) {
    }
};

// A short-lived expanding ring, drawn where the player calls down the strike.
struct AbilityBlast {
    sf::Vector2f position;
    float radius = 0.f;
    sf::Time elapsed;
    sf::Time duration = sf::seconds(0.45f);
};

// Small rising label used for gold pickups and wave bonuses.
struct FloatingText {
    sf::Text text;
    sf::Time remaining;
    float riseSpeed = 42.f;
};

struct TowerSelectionButton {
    sf::RectangleShape buttonShape; 
    sf::Sprite towerIcon;           
    sf::Text costText;
    sf::Text nameText;
    std::string towerTypeId;
    bool isEnabled = false;         
};


class cgame {
public:
    cgame();
    ~cgame();

    void loadMap(const std::string& mapId, const std::string& dataFilePath);
    void resetGame();
    void handleInput(const sf::Event& event, sf::RenderWindow& window);
    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window);
    void setPaused(bool paused);
    bool isPaused() const;
    bool isGameOver() const;
    bool hasWon() const;
    const TowerLevelData* getTowerNextLevelData(const std::string& typeId, int currentLevel) const;
    sf::Texture& getTexture(const std::string& texturePath);

    int getLives() const;
    int getMaxLives() const;

    void saveGame(const std::string& filename) const;
    bool loadGame(const std::string& filename);
    std::string getCurrentMapId() const;

    // Returns true if it consumed the request (a pending build or an open
    // tower panel), so the caller knows whether ESC should open the pause menu.
    bool cancelPendingAction();

    // Player ability, in the spirit of Kingdom Rush's Rain of Fire: a manually
    // aimed area strike on a cooldown, giving the player something to do while
    // a wave is running instead of only watching.
    bool isAbilityReady() const;
    float getAbilityCooldownRatio() const;

    int getEnemiesDefeated() const;
    sf::Time getLevelTime() const;
    long calculateScore() const;
    void setupTowerSelectionPanel(sf::RenderWindow& window);
private:
    cmap* _map;
    std::string _currentMapId;
    std::vector<cenemy> _enemies;
    // Scratch buffer of depth-sorted enemies, rebuilt each frame for drawing.
    std::vector<cenemy*> _renderOrder;
    std::vector<std::unique_ptr<cbasictower>> _towers;
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
    sf::RectangleShape _sellButton;
    sf::Text _sellText;
    sf::Text _sellValueText;

    int _lives;
    int _maxLives;
    // Enemy types queued for the current wave, in spawn order.
    std::vector<int> _spawnQueue;
    bool _instructionsDismissed;
    int _totalWaves;
    sf::Text _hintText;
    sf::Text _waveProgressText;
    sf::RectangleShape _startWaveButton;
    sf::Text _startWaveText;
    sf::Text _speedLabel;
    int _speedStep;
    // Ability state.
    sf::Time _abilityCooldown;
    sf::Time _abilityMaxCooldown;
    bool _selectingAbilityTarget;
    sf::RectangleShape _abilityButton;
    sf::RectangleShape _abilityCooldownBar;
    sf::Text _abilityText;
    std::vector<AbilityBlast> _abilityBlasts;
    // Feedback and preview.
    std::vector<FloatingText> _floatingTexts;
    std::vector<int> _pendingWaveQueue;
    int _pendingWaveNumber;
    sf::Text _wavePreviewText;
    int _money;
    int _currentWave;
    int _enemiesPerWave;
    sf::Time _timeSinceLastSpawn;
    sf::Time _spawnInterval;
    int _enemiesSpawnedThisWave;
    cbasictower* _selectedTower;
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
    int _enemiesDefeated;
    sf::Time _levelTime;
    bool _levelIsActive;
    sf::RectangleShape _towerPanel;
    std::vector<TowerSelectionButton> _towerSelectionButtons;
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
    void updateUpgradePanel();
    void selectTowerToBuild(const std::string& typeId);
    void renderInstructionPanel(sf::RenderWindow& window);
    void updateTowerSelectionPanel();
    void renderTowerSelectionPanel(sf::RenderWindow& window);
    void renderBuildableTiles(sf::RenderWindow& window);
    void buildWaveComposition();
    int totalWavesForCurrentMap() const;
    void applySplashDamage(const sf::Vector2f& center, float radius, int damage,
        int alreadyHitEnemyId, bool ignoreArmour = false);
    void awardWaveClearBonus();
    void requestNextWave();
    void updateHudText();
    std::vector<int> makeWaveComposition(int waveNumber);
    void ensurePendingWave();
    std::string describeWave(const std::vector<int>& queue) const;
    void castAbilityAt(const sf::Vector2f& target);
    void updateEffects(sf::Time deltaTime);
    void renderEffects(sf::RenderWindow& window);
    void spawnFloatingText(const sf::Vector2f& position, const std::string& label, const sf::Color& color);
    void onEnemyKilled(cenemy& enemy);
};

#endif // CGAME_H