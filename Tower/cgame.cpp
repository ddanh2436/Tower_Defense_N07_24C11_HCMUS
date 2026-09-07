#include "cgame.h"
#include "GameView.h"
#include "SoundManager.h"
#include <iostream>
#include <sstream>
#include <random>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <cmath>
#include "carchertower.h"
#include "ccannontower.h"

static int nextTowerId = 0;

cgame::cgame() : _rng(std::random_device{}()) {
    _map = nullptr;
    loadFont();
    setupUI();
    setupEnemyTypes();
    setupTowerTypes();
    _isPaused = false;
}

cgame::~cgame() {
    if (_map) {
        delete _map;
        _map = nullptr;
    }
}

void cgame::selectTowerToBuild(const std::string& typeId) {
    auto it = _towerBlueprints.find(typeId);
    if (it == _towerBlueprints.end() || it->second.empty()) {
        std::cerr << "Loi: Khong tim thay thong tin cho tru loai: " << typeId << std::endl;
        return;
    }

    const auto& blueprint = it->second;
    int buildCost = blueprint[0].cost;

    if (buildCost > 0 && _money >= buildCost) {
        _selectingAbilityTarget = false;
        _selectingTowerToBuild = true;
        _selectedTowerType = typeId;
        for (auto& button : _towerSelectionButtons) {
            if (button.towerTypeId == typeId) {
                button.buttonShape.setOutlineColor(sf::Color::Yellow);
            }
            else {
                button.buttonShape.setOutlineColor(sf::Color(60, 60, 80, 255));
            }
        }

        static sf::Texture previewTexture;
        if (previewTexture.loadFromFile(blueprint[0].idle_texturePath)) {
            _towerPlacementPreview.setTexture(previewTexture);
            _towerPlacementPreview.setTextureRect(sf::IntRect(
                blueprint[0].idle_startFrame * blueprint[0].frameSize.x, 0,
                blueprint[0].frameSize.x, blueprint[0].frameSize.y
            ));
        }
        else {
            std::cerr << "Loi tai texture preview thap!" << std::endl;
            _selectingTowerToBuild = false;
            return;
        }
        sf::FloatRect bounds = _towerPlacementPreview.getLocalBounds();
        _towerPlacementPreview.setOrigin(bounds.width / 2.f, bounds.height - blueprint[0].frameOffsetY);
    }
    else {
        std::cout << "Khong du tien de mua tru " << typeId << "!" << std::endl;
    }
}


void cgame::loadMap(const std::string& mapId, const std::string& dataFilePath) {
    std::cout << "Loading map: " << mapId << " from " << dataFilePath << std::endl;
    _currentMapId = mapId;
    if (_map) {
        delete _map;
        _map = nullptr;
    }
    _map = new cmap(dataFilePath, mapId);
    resetGame();
}

// The two towers used to be near-identical: same range, same fire rate, same
// damage, and the cannon simply added a flat +10. They now fill different
// roles - the archer is the cheap, fast single-target option, the cannon is
// the expensive, slow one that hits a whole group at once.
void cgame::setupTowerTypes() {
    _towerBlueprints.clear();

    // --- ARCHER TOWER: cheap, long range, fast, single target ---
    sf::Vector2i archerFrameSize = { 70, 115 };
    int archerFrameOffsetY = 15;
    std::vector<TowerLevelData> archerLevels;
    archerLevels.push_back(TowerLevelData{
        /*level*/ 1, /*cost*/ 60,
        /*frameSize*/ archerFrameSize, /*frameOffsetY*/ archerFrameOffsetY,
        /*build*/ "assets/2.png", 0, 3, 0.5f,
        /*attack*/ "assets/2.png", 2, 2, 0.5f,
        /*idle*/ "assets/2_idle.png", 0, 4, 2.0f,
        /*range*/ 160.f, /*fireRate*/ 0.80f, /*damage*/ 18, /*bulletSpeed*/ 420.f,
        /*bulletTexturePath*/ "assets/bullet.png", /*splashRadius*/ 0.f
        });
    archerLevels.push_back(TowerLevelData{
        /*level*/ 2, /*cost*/ 90,
        archerFrameSize, archerFrameOffsetY,
        "assets/3.png", 0, 3, 0.4f,
        "assets/3.png", 2, 2, 0.4f,
        "assets/3_idle.png", 0, 4, 1.8f,
        190.f, 0.65f, 30, 460.f,
        "assets/bullet.png", 0.f
        });
    archerLevels.push_back(TowerLevelData{
        /*level*/ 3, /*cost*/ 140,
        archerFrameSize, archerFrameOffsetY,
        "assets/4.png", 0, 3, 0.4f,
        "assets/4.png", 2, 2, 0.4f,
        "assets/4_idle.png", 0, 6, 1.8f,
        225.f, 0.50f, 46, 500.f,
        "assets/bullet.png", 0.f
        });
    _towerBlueprints["ArcherTower"] = archerLevels;

    // --- CANNON TOWER: expensive, short range, slow, splash damage ---
    sf::Vector2i cannonFrameSize = { 70, 115 };
    int cannonFrameOffsetY = 15;
    std::vector<TowerLevelData> cannonLevels;
    cannonLevels.push_back(TowerLevelData{
        /*level*/ 1, /*cost*/ 110,
        cannonFrameSize, cannonFrameOffsetY,
        "assets/5.png", 0, 3, 0.5f,
        "assets/5.png", 2, 2, 0.5f,
        "assets/5_idle.png", 0, 6, 2.0f,
        /*range*/ 135.f, /*fireRate*/ 1.80f, /*damage*/ 46, /*bulletSpeed*/ 300.f,
        "assets/fire.png", /*splashRadius*/ 55.f
        });
    cannonLevels.push_back(TowerLevelData{
        /*level*/ 2, /*cost*/ 165,
        cannonFrameSize, cannonFrameOffsetY,
        "assets/6.png", 0, 3, 0.4f,
        "assets/6.png", 2, 2, 0.4f,
        "assets/6_idle.png", 0, 6, 1.8f,
        155.f, 1.60f, 78, 320.f,
        "assets/fire.png", 66.f
        });
    cannonLevels.push_back(TowerLevelData{
        /*level*/ 3, /*cost*/ 240,
        cannonFrameSize, cannonFrameOffsetY,
        "assets/7.png", 0, 3, 0.4f,
        "assets/7.png", 2, 2, 0.4f,
        "assets/7_idle.png", 0, 6, 1.8f,
        175.f, 1.40f, 120, 340.f,
        "assets/fire.png", 78.f
        });
    _towerBlueprints["CannonTower"] = cannonLevels;
}

const TowerLevelData* cgame::getTowerNextLevelData(const std::string& typeId, int currentLevel) const {
    auto it = _towerBlueprints.find(typeId);
    if (it != _towerBlueprints.end()) {
        const auto& levels = it->second;
        if (static_cast<size_t>(currentLevel) < levels.size()) {
            return &levels[currentLevel];
        }
    }
    return nullptr;
}

void cgame::setupEnemyTypes() {
    _availableEnemyTypes.clear();
    // --- GOBLIN ---
    EnemyType goblin;
    goblin.frameSize = { 48, 48 }; goblin.frameCount = 6; goblin.stride = 48;
    goblin.name = "Goblin"; goblin.health = 120; goblin.speed = 20.f; goblin.scale = 2.0f;
    goblin.moneyValue = 30; goblin.armour = 0; goblin.livesCost = 1;
    goblin.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U_Walk.png";
    goblin.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D_Walk.png";
    goblin.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S_Walk.png";
    goblin.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U_Death.png";
    goblin.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D_Death.png";
    goblin.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S_Death.png";
    _availableEnemyTypes.push_back(goblin);
    // --- WOLF ---
    EnemyType wolf = goblin;
    wolf.name = "Wolf"; wolf.health = 60; wolf.speed = 35.f; wolf.scale = 1.2f;
    wolf.moneyValue = 20; wolf.armour = 0; wolf.livesCost = 1;
    wolf.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U1_Walk.png";
    wolf.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D1_Walk.png";
    wolf.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S1_Walk.png";
    wolf.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U1_Death.png";
    wolf.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D1_Death.png";
    wolf.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S1_Death.png";
    _availableEnemyTypes.push_back(wolf);
    // --- BEE ---
    EnemyType bee = goblin;
    bee.name = "Bee"; bee.health = 250; bee.speed = 15.f; bee.scale = 1.5f;
    bee.moneyValue = 22; bee.armour = 4; bee.livesCost = 1;
    bee.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U2_Walk.png";
    bee.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D2_Walk.png";
    bee.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S2_Walk.png";
    bee.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U2_Death.png";
    bee.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D2_Death.png";
    bee.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S2_Death.png";
    _availableEnemyTypes.push_back(bee);
    // --- SLIME ---
    EnemyType slime = goblin;
    slime.name = "Slime"; slime.health = 150; slime.speed = 20.f; slime.scale = 2.0f;
    slime.moneyValue = 20; slime.armour = 2; slime.livesCost = 1;
    slime.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U3_Walk.png";
    slime.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D3_Walk.png";
    slime.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S3_Walk.png";
    slime.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U3_Death.png";
    slime.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D3_Death.png";
    slime.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S3_Death.png";
    _availableEnemyTypes.push_back(slime);
    // --- SLIME 1 ---
    EnemyType slime1 = goblin;
    slime1.name = "King Slime"; slime1.health = 150; slime1.speed = 20.f; slime1.scale = 2.0f;
    slime1.moneyValue = 24; slime1.armour = 3; slime1.livesCost = 1;
    slime1.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U3_Special.png";
    slime1.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D3_Special.png";
    slime1.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S3_Special.png";
    slime1.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U3_Death2.png";
    slime1.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D3_Death2.png";
    slime1.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S3_Death2.png";
    _availableEnemyTypes.push_back(slime1);
    // --- HORSERIDER ---
    EnemyType horserider;
    horserider.frameSize = { 96, 96 }; horserider.frameCount = 6; horserider.stride = 96;
    horserider.name = "Rider"; horserider.health = 200; horserider.speed = 30.f; horserider.scale = 1.5f;
    horserider.moneyValue = 40; horserider.armour = 9; horserider.livesCost = 2;
    horserider.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U4_Walk.png";
    horserider.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D4_Walk.png";
    horserider.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S4_Walk.png";
    horserider.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U4_Death.png";
    horserider.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D4_Death.png";
    horserider.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S4_Death.png";
    _availableEnemyTypes.push_back(horserider);
    // --- RAT ---
    EnemyType rat = horserider;
    rat.name = "Rat Brute"; rat.health = 400; rat.speed = 20.f; rat.scale = 2.0f;
    rat.moneyValue = 80; rat.armour = 7; rat.livesCost = 2;
    rat.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U5_Walk.png";
    rat.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D5_Walk.png";
    rat.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S5_Walk.png";
    rat.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U5_Death.png";
    rat.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D5_Death.png";
    rat.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S5_Death.png";
    _availableEnemyTypes.push_back(rat);
    // --- WIZARD ---
    EnemyType wizard = horserider;
    wizard.name = "Wizard Boss"; wizard.health = 1400; wizard.speed = 15.f; wizard.scale = 2.0f;
    wizard.moneyValue = 250; wizard.armour = 12; wizard.livesCost = 5;
    wizard.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U6_Walk.png";
    wizard.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D6_Walk.png";
    wizard.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S6_Walk.png";
    wizard.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U6_Death.png";
    wizard.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D6_Death.png";
    wizard.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S6_Death.png";
    _availableEnemyTypes.push_back(wizard);
}

void cgame::resetGameStats() {
    _isPaused = false;
    _lives = 15;
    _maxLives = 15;
    // Was 1000 with 50-gold towers, which meant the whole map could be filled
    // before the first enemy appeared. The player now has to earn the board.
    _money = 260;
    _currentWave = 0;
    _totalWaves = totalWavesForCurrentMap();
    _enemiesPerWave = 0;
    _spawnQueue.clear();
    _spawnInterval = sf::seconds(1.2f);
    _timeSinceLastSpawn = sf::Time::Zero;
    _enemiesSpawnedThisWave = 0;
    _isGameOver = false;
    _inIntermission = false;
    _intermissionTime = sf::seconds(12.f);
    _intermissionTimer = sf::Time::Zero;
    _levelWon = false;
    _waveInProgress = false;
    _currentWaveEnemyTypeIndex = -1;
    _isFastForward = false;
    _speedStep = 0;
    _gameSpeedMultiplier = 1.0f;
    _ffButtonSprite.setColor(sf::Color::White);
    _enemies.clear();
    _towers.clear();
    _bullets.clear();
    _selectingTowerToBuild = false;
    _selectedTower = nullptr;
    _isUpgradePanelVisible = false;
    _instructionsDismissed = false;
    _abilityMaxCooldown = sf::seconds(25.f);
    _abilityCooldown = sf::Time::Zero;
    _selectingAbilityTarget = false;
    _abilityBlasts.clear();
    _floatingTexts.clear();
    _pendingWaveQueue.clear();
    _pendingWaveNumber = -1;
    _renderOrder.clear();
    nextTowerId = 0;
    _enemiesDefeated = 0;
    _levelTime = sf::Time::Zero;
    _levelIsActive = false;
    _messageText.setString("");
    _timerText.setString("");
}

int cgame::totalWavesForCurrentMap() const {
    // The last map is the long one; the rest share a shorter campaign.
    return (getCurrentMapId() == "MAP_4") ? 10 : 8;
}

// Builds the spawn order for a given wave number.
//
// The old version picked one random enemy type and spawned N copies of it, so
// every wave was a single wall of identical monsters and the tower you needed
// was pure luck. Waves are now mixed: the roster widens as the level goes on,
// the toughest unlocked type leads the second half, and the final wave is a
// boss with an escort instead of a lone wizard.
//
// This is a pure function of the wave number and the RNG, so the result can be
// generated ahead of time and shown to the player as a preview.
std::vector<int> cgame::makeWaveComposition(int waveNumber) {
    std::vector<int> queue;
    if (_availableEnemyTypes.empty()) return queue;

    const int BOSS_INDEX = 7;
    const bool isFinalWave = (waveNumber >= totalWavesForCurrentMap());

    // Enemy types unlocked so far, in roughly increasing order of threat.
    static const int unlockOrder[] = { 0, 1, 3, 2, 4, 5, 6 };
    const int unlockCount = static_cast<int>(sizeof(unlockOrder) / sizeof(unlockOrder[0]));

    int unlocked = waveNumber;
    if (unlocked < 1) unlocked = 1;
    if (unlocked > unlockCount) unlocked = unlockCount;

    std::vector<int> pool;
    for (int i = 0; i < unlocked; ++i) {
        if (static_cast<size_t>(unlockOrder[i]) < _availableEnemyTypes.size()) {
            pool.push_back(unlockOrder[i]);
        }
    }
    if (pool.empty()) pool.push_back(0);

    std::uniform_int_distribution<int> dist(0, static_cast<int>(pool.size()) - 1);

    if (isFinalWave) {
        // Escort first, boss last, so the player has to hold the line and then
        // still have something left for the boss.
        const int escortCount = 6 + waveNumber;
        for (int i = 0; i < escortCount; ++i) queue.push_back(pool[dist(_rng)]);
        if (static_cast<size_t>(BOSS_INDEX) < _availableEnemyTypes.size()) {
            queue.push_back(BOSS_INDEX);
        }
    }
    else {
        const int total = 6 + waveNumber * 2;
        for (int i = 0; i < total; ++i) queue.push_back(pool[dist(_rng)]);
        // Put the toughest unlocked type at the front of the second half so the
        // wave escalates rather than arriving all at once.
        if (queue.size() > 4) queue[queue.size() / 2] = pool.back();
    }
    return queue;
}

// Generates the next wave ahead of time so it can be previewed. Once made, it
// is cached, otherwise the preview would show a different wave than the one
// that actually arrives.
void cgame::ensurePendingWave() {
    const int nextWave = _currentWave + 1;
    if (_pendingWaveNumber == nextWave && !_pendingWaveQueue.empty()) return;
    if (nextWave > totalWavesForCurrentMap()) {
        _pendingWaveQueue.clear();
        _pendingWaveNumber = -1;
        return;
    }
    _pendingWaveQueue = makeWaveComposition(nextWave);
    _pendingWaveNumber = nextWave;
}

// "6x Goblin, 4x Rider (armour 9)" - lets the player pick towers for what is
// actually coming instead of guessing, the way Kingdom Rush previews a wave.
std::string cgame::describeWave(const std::vector<int>& queue) const {
    if (queue.empty()) return "";

    // Counts in first-appearance order.
    std::vector<std::pair<int, int>> counts;
    for (int typeIndex : queue) {
        auto it = std::find_if(counts.begin(), counts.end(),
            [typeIndex](const std::pair<int, int>& entry) { return entry.first == typeIndex; });
        if (it == counts.end()) counts.emplace_back(typeIndex, 1);
        else it->second++;
    }

    std::stringstream out;
    for (size_t i = 0; i < counts.size(); ++i) {
        if (static_cast<size_t>(counts[i].first) >= _availableEnemyTypes.size()) continue;
        const EnemyType& type = _availableEnemyTypes[counts[i].first];
        if (i > 0) out << ",  ";
        out << counts[i].second << "x " << type.name;
        if (type.armour > 0) out << " (armour " << type.armour << ")";
    }
    return out.str();
}

void cgame::buildWaveComposition() {
    // Prefer the queue that was previewed to the player.
    if (_pendingWaveNumber == _currentWave && !_pendingWaveQueue.empty()) {
        _spawnQueue = _pendingWaveQueue;
    }
    else {
        _spawnQueue = makeWaveComposition(_currentWave);
    }
    _pendingWaveQueue.clear();
    _pendingWaveNumber = -1;

    _enemiesPerWave = static_cast<int>(_spawnQueue.size());
    _currentWaveEnemyTypeIndex = _spawnQueue.empty() ? -1 : _spawnQueue.front();
}

void cgame::startNextWave() {
    _bullets.clear();
    if (_waveInProgress || _isGameOver || _levelWon) return;
    if (_currentWave == 0) {
        _levelIsActive = true;
        _instructionsDismissed = true;
    }
    _totalWaves = totalWavesForCurrentMap();
    _currentWave++;

    buildWaveComposition();

    // Later waves arrive in a tighter stream.
    float interval = 1.3f - _currentWave * 0.06f;
    if (interval < 0.45f) interval = 0.45f;
    _spawnInterval = sf::seconds(interval);

    _enemiesSpawnedThisWave = 0;
    _waveInProgress = true;
    _inIntermission = false;
    _timeSinceLastSpawn = _spawnInterval; // First enemy appears immediately.
    _messageText.setString("");
    _timerText.setString("");
    _enemies.reserve(_enemies.size() + _enemiesPerWave);
    if (_map) _map->calculateEnemyPath(getCurrentMapId());
    std::cout << "Wave " << _currentWave << "/" << _totalWaves
        << " start. Enemies: " << _enemiesPerWave << std::endl;
}

// Starting a wave early pays a bounty proportional to the time skipped, which
// gives a skilled player a real reason to press N instead of always waiting.
void cgame::requestNextWave() {
    if (_isGameOver || _levelWon || _waveInProgress) return;

    if (_inIntermission && _intermissionTimer > sf::Time::Zero) {
        const int bonus = static_cast<int>(_intermissionTimer.asSeconds()) * 4;
        if (bonus > 0) {
            _money += bonus;
            std::cout << "Early call bonus: +" << bonus << " gold" << std::endl;
        }
    }
    _inIntermission = false;
    _intermissionTimer = sf::Time::Zero;
    startNextWave();
}

void cgame::awardWaveClearBonus() {
    const int bonus = 30 + _currentWave * 12;
    _money += bonus;
    std::cout << "Wave " << _currentWave << " cleared. Bonus: +" << bonus << " gold" << std::endl;
}

void cgame::spawnEnemy() {
    if (!_map) return;
    if (_enemiesSpawnedThisWave >= _enemiesPerWave) return;
    if (static_cast<size_t>(_enemiesSpawnedThisWave) >= _spawnQueue.size()) return;

    const auto& path = _map->getEnemyPath();
    if (path.empty()) return;

    const int typeIndex = _spawnQueue[_enemiesSpawnedThisWave];
    if (typeIndex < 0 || static_cast<size_t>(typeIndex) >= _availableEnemyTypes.size()) return;

    const EnemyType& baseType = _availableEnemyTypes.at(typeIndex);
    EnemyType finalType = baseType;
    // Softer than the old 1.3^wave, which became unbeatable once the campaign
    // grew past five waves.
    finalType.health = static_cast<int>(baseType.health * std::pow(1.22, _currentWave - 1));
    finalType.speed = baseType.speed * (1.f + (_currentWave - 1) * 0.04f);
    finalType.moneyValue = baseType.moneyValue + _currentWave * 2;

    _enemies.emplace_back(this, finalType, typeIndex, path);
    _enemiesSpawnedThisWave++;
}

namespace {
    // Rain of Fire tuning. Damage ignores armour, which is what makes it the
    // answer to a heavily armoured pack that the towers are chewing through
    // too slowly.
    const float ABILITY_RADIUS = 115.f;
    const int ABILITY_DAMAGE = 130;
}

bool cgame::isAbilityReady() const {
    return _abilityCooldown <= sf::Time::Zero;
}

float cgame::getAbilityCooldownRatio() const {
    if (_abilityMaxCooldown <= sf::Time::Zero) return 1.f;
    if (_abilityCooldown <= sf::Time::Zero) return 1.f;
    return 1.f - (_abilityCooldown.asSeconds() / _abilityMaxCooldown.asSeconds());
}

void cgame::castAbilityAt(const sf::Vector2f& target) {
    if (!isAbilityReady()) return;

    _abilityCooldown = _abilityMaxCooldown;
    _selectingAbilityTarget = false;

    AbilityBlast blast;
    blast.position = target;
    blast.radius = ABILITY_RADIUS;
    _abilityBlasts.push_back(blast);

    // -1 as the "already hit" id means nothing is excluded.
    applySplashDamage(target, ABILITY_RADIUS, ABILITY_DAMAGE, -1, true);
    SoundManager::playSoundEffect("assets/enemy_explode.ogg");
    std::cout << "Rain of Fire cast at ("
              << static_cast<int>(target.x) << ","
              << static_cast<int>(target.y) << ")" << std::endl;
}

void cgame::spawnFloatingText(const sf::Vector2f& position, const std::string& label, const sf::Color& color) {
    // Cap the list so a big splash kill cannot flood the screen.
    if (_floatingTexts.size() > 40) return;

    FloatingText entry;
    entry.text.setFont(_gameFont);
    entry.text.setString(label);
    entry.text.setCharacterSize(16);
    entry.text.setFillColor(color);
    entry.text.setOutlineThickness(1.5f);
    entry.text.setOutlineColor(sf::Color(0, 0, 0, 180));
    const sf::FloatRect bounds = entry.text.getLocalBounds();
    entry.text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    entry.text.setPosition(position);
    entry.remaining = sf::seconds(0.9f);
    _floatingTexts.push_back(entry);
}

// Single place where a kill pays out, so bullets, splash and the ability all
// reward the player identically.
void cgame::onEnemyKilled(cenemy& enemy) {
    const int reward = enemy.getMoneyValue();
    _money += reward;
    _enemiesDefeated++;
    spawnFloatingText(enemy.getPosition(), "+" + std::to_string(reward), sf::Color(255, 220, 90));
    SoundManager::playSoundEffect("assets/enemy_explode.ogg");
}

void cgame::updateEffects(sf::Time deltaTime) {
    if (_abilityCooldown > sf::Time::Zero) {
        _abilityCooldown -= deltaTime;
        if (_abilityCooldown < sf::Time::Zero) _abilityCooldown = sf::Time::Zero;
    }

    for (auto& blast : _abilityBlasts) blast.elapsed += deltaTime;
    _abilityBlasts.erase(std::remove_if(_abilityBlasts.begin(), _abilityBlasts.end(),
        [](const AbilityBlast& b) { return b.elapsed >= b.duration; }), _abilityBlasts.end());

    for (auto& entry : _floatingTexts) {
        entry.remaining -= deltaTime;
        entry.text.move(0.f, -entry.riseSpeed * deltaTime.asSeconds());
    }
    _floatingTexts.erase(std::remove_if(_floatingTexts.begin(), _floatingTexts.end(),
        [](const FloatingText& t) { return t.remaining <= sf::Time::Zero; }), _floatingTexts.end());
}

void cgame::renderEffects(sf::RenderWindow& window) {
    // Expanding, fading ring where the strike landed.
    for (const auto& blast : _abilityBlasts) {
        const float progress = blast.elapsed.asSeconds() / blast.duration.asSeconds();
        const float radius = blast.radius * (0.35f + 0.65f * progress);
        const sf::Uint8 alpha = static_cast<sf::Uint8>(210 * (1.f - progress));

        sf::CircleShape ring(radius);
        ring.setOrigin(radius, radius);
        ring.setPosition(blast.position);
        ring.setFillColor(sf::Color(255, 140, 40, static_cast<sf::Uint8>(alpha / 3)));
        ring.setOutlineThickness(4.f);
        ring.setOutlineColor(sf::Color(255, 200, 80, alpha));
        window.draw(ring);
    }

    for (const auto& entry : _floatingTexts) {
        sf::Text faded = entry.text;
        const float alphaRatio = std::min(1.f, entry.remaining.asSeconds() / 0.4f);
        sf::Color color = faded.getFillColor();
        color.a = static_cast<sf::Uint8>(255 * alphaRatio);
        faded.setFillColor(color);
        sf::Color outline = faded.getOutlineColor();
        outline.a = static_cast<sf::Uint8>(180 * alphaRatio);
        faded.setOutlineColor(outline);
        window.draw(faded);
    }

    // Aiming reticle while the strike is armed.
    if (_selectingAbilityTarget) {
        const sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::CircleShape reticle(ABILITY_RADIUS);
        reticle.setOrigin(ABILITY_RADIUS, ABILITY_RADIUS);
        reticle.setPosition(mousePos);
        reticle.setFillColor(sf::Color(255, 120, 40, 45));
        reticle.setOutlineThickness(2.5f);
        reticle.setOutlineColor(sf::Color(255, 190, 90, 220));
        window.draw(reticle);
    }
}

// Damages every living enemy inside a blast, skipping the one the bullet
// already hit directly so it is not damaged twice by the same shot.
void cgame::applySplashDamage(const sf::Vector2f& center, float radius, int damage,
    int alreadyHitEnemyId, bool ignoreArmour) {
    if (radius <= 0.f) return;
    const float radiusSquared = radius * radius;

    for (auto& enemy : _enemies) {
        if (!enemy.isAlive() || enemy.getId() == alreadyHitEnemyId) continue;

        const sf::Vector2f delta = enemy.getPosition() - center;
        if (delta.x * delta.x + delta.y * delta.y > radiusSquared) continue;

        if (enemy.takeDamage(damage, ignoreArmour)) {
            onEnemyKilled(enemy);
        }
    }
}

void cgame::handleCollisions() {
    for (auto& bullet : _bullets) {
        if (!bullet.canCollide()) continue;
        for (auto& enemy : _enemies) {
            if (!enemy.isAlive()) continue;
            if (!bullet.getGlobalBounds().intersects(enemy.getGlobalBounds())) continue;

            bullet.setActive(false);
            const int directHitId = enemy.getId();
            if (enemy.takeDamage(bullet.getDamage())) {
                onEnemyKilled(enemy);
            }
            // Cannon shells damage the whole group around the impact.
            applySplashDamage(bullet.getPosition(), bullet.getSplashRadius(),
                bullet.getDamage(), directHitId);
            break;
        }
    }
}

bool cgame::isGameOver() const {
    return _isGameOver || _levelWon;
}

void cgame::loadFont() {
    if (!_gameFont.loadFromFile(FONT_PATH)) {
        std::cerr << "Loi: Khong the tai font game: " << FONT_PATH << std::endl;
    }
}
void cgame::setupUI() {
    _uiPanel.setSize(sf::Vector2f(230, 148));
    _uiPanel.setPosition(10, 10);
    _uiPanel.setFillColor(sf::Color(30, 30, 40, 200));
    _uiPanel.setOutlineThickness(2.f);
    _uiPanel.setOutlineColor(sf::Color(60, 60, 80, 255));
    const int fontSize = 22;
    const float padding = 15.f;
    const float iconSize = 24.f;
    const float yStart = 20.f;
    const float spacing = 30.f;
    if (!_heartIcon.loadFromFile("assets/heart_icon.png") || !_coinIcon.loadFromFile("assets/coin_icon.png") || !_waveIcon.loadFromFile("assets/wave_icon.png")) {
        std::cerr << "Warning: Failed to load UI icons" << std::endl;
    }
    _livesIconSprite.setTexture(_heartIcon);
    _livesIconSprite.setScale(iconSize / _heartIcon.getSize().x, iconSize / _heartIcon.getSize().y);
    _livesIconSprite.setPosition(padding + 10, yStart);
    _livesText.setFont(_gameFont);
    _livesText.setCharacterSize(fontSize);
    _livesText.setFillColor(sf::Color::White);
    _livesText.setPosition(padding + iconSize + 20, yStart - 2);
    _moneyIconSprite.setTexture(_coinIcon);
    _moneyIconSprite.setScale(iconSize / _coinIcon.getSize().x, iconSize / _coinIcon.getSize().y);
    _moneyIconSprite.setPosition(padding + 10, yStart + spacing);
    _moneyText.setFont(_gameFont);
    _moneyText.setCharacterSize(fontSize);
    _moneyText.setFillColor(sf::Color(255, 215, 0));
    _moneyText.setPosition(padding + iconSize + 20, yStart + spacing - 2);
    _waveIconSprite.setTexture(_waveIcon);
    _waveIconSprite.setScale(iconSize / _waveIcon.getSize().x, iconSize / _waveIcon.getSize().y);
    _waveIconSprite.setPosition(padding + 10, yStart + spacing * 2);
    _waveText.setFont(_gameFont);
    _waveText.setCharacterSize(fontSize);
    _waveText.setFillColor(sf::Color(120, 200, 255));
    _waveText.setPosition(padding + iconSize + 20, yStart + spacing * 2 - 2);
    _messageText.setFont(_gameFont);
    _messageText.setCharacterSize(50);
    _messageText.setFillColor(sf::Color::Yellow);
    _timerText.setFont(_gameFont);
    _timerText.setCharacterSize(28);
    _timerText.setFillColor(sf::Color::Yellow);

    // How many enemies of the current wave are still coming, shown right under
    // the wave counter.
    _waveProgressText.setFont(_gameFont);
    _waveProgressText.setCharacterSize(16);
    _waveProgressText.setFillColor(sf::Color(200, 210, 230));
    _waveProgressText.setPosition(padding + 10, yStart + spacing * 3);

    // A single contextual hint line above the tower bar.
    _hintText.setFont(_gameFont);
    _hintText.setCharacterSize(20);
    _hintText.setFillColor(sf::Color(230, 235, 250));

    _startWaveText.setFont(_gameFont);
    _startWaveText.setCharacterSize(20);
    _startWaveText.setFillColor(sf::Color::White);

    _speedLabel.setFont(_gameFont);
    _speedLabel.setCharacterSize(16);
    _speedLabel.setFillColor(sf::Color(180, 230, 190));

    _wavePreviewText.setFont(_gameFont);
    _wavePreviewText.setCharacterSize(17);
    _wavePreviewText.setFillColor(sf::Color(255, 205, 130));

    _abilityText.setFont(_gameFont);
    _abilityText.setCharacterSize(14);
    _abilityText.setFillColor(sf::Color::White);
    if (!_pauseButtonTexture.loadFromFile("assets/pause_icon.png")) {
        std::cerr << "Loi: Khong the tai pause_icon.png" << std::endl;
    }
    _pauseButtonSprite.setTexture(_pauseButtonTexture);
    float pauseIconSize = 40.f;
    _pauseButtonSprite.setScale(pauseIconSize / _pauseButtonTexture.getSize().x, pauseIconSize / _pauseButtonTexture.getSize().y);
    if (!_ffButtonTexture.loadFromFile("assets/ff_icon.png")) {
        std::cerr << "Loi: Khong the tai ff_icon.png" << std::endl;
    }
    _ffButtonSprite.setTexture(_ffButtonTexture);
    _ffButtonSprite.setScale(pauseIconSize / _ffButtonTexture.getSize().x, pauseIconSize / _ffButtonTexture.getSize().y);
}

void cgame::resetGame() {
    std::cout << "Reseting game state..." << std::endl;
    resetGameStats();
}

void cgame::updateEnemies(sf::Time deltaTime) {
    if (!_map) return;
    for (auto& enemy : _enemies) {
        if (enemy.isActive()) {
            enemy.update(deltaTime);
            if (enemy.hasReachedEnd()) {
                // A boss punching through costs far more than a goblin does.
                _lives -= enemy.getLivesCost();
                if (_lives < 0) _lives = 0;
                SoundManager::playSoundEffect("assets/life_lost.ogg");
                enemy.setActive(false);
                if (_lives <= 0) {
                    _isGameOver = true;
                    _messageText.setString("GAME OVER!");
                    SoundManager::playLoseMusic();
                }
            }
        }
    }
}

void cgame::updateTowers(sf::Time deltaTime) {
    for (auto& tower : _towers) {
        tower->update(deltaTime, _enemies, _bullets);
    }
}

void cgame::updateBullets(sf::Time deltaTime) {
    for (auto& bullet : _bullets) {
        if (!bullet.isActive()) continue;

        // Guide the shot onto its target if that target is still alive. Shots
        // used to fly on a fixed heading and sailed straight past anything
        // that kept moving, so most damage simply never landed.
        const int targetId = bullet.getTargetId();
        if (targetId != -1) {
            for (const auto& enemy : _enemies) {
                if (enemy.getId() == targetId) {
                    if (enemy.isAlive()) bullet.steerTowards(enemy.getPosition(), deltaTime);
                    break;
                }
            }
        }
        bullet.update(deltaTime);
    }
}

void cgame::cleanupInactiveObjects() {
    _enemies.erase(std::remove_if(_enemies.begin(), _enemies.end(), [](const cenemy& e) { return e.isReadyForRemoval(); }), _enemies.end());
    _bullets.erase(std::remove_if(_bullets.begin(), _bullets.end(), [](const cbullet& b) { return !b.isActive(); }), _bullets.end());

    // Drop the panel's reference before the tower it points at is destroyed,
    // otherwise a sold tower leaves _selectedTower dangling.
    if (_selectedTower && _selectedTower->isPendingRemoval()) {
        _selectedTower = nullptr;
        _isUpgradePanelVisible = false;
    }
    _towers.erase(std::remove_if(_towers.begin(), _towers.end(), [](const std::unique_ptr<cbasictower>& t) { return t->isPendingRemoval(); }), _towers.end());
}

void cgame::updateTowerPlacementPreview(sf::RenderWindow& window) {
    if (!_selectingTowerToBuild || !_map) return;
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);
    sf::Vector2i gridCoords = _map->getGridCoordinates(mouseWorldPos);
    cpoint tileCenterPixelPos = _map->getPixelPosition(static_cast<float>(gridCoords.y), static_cast<float>(gridCoords.x), PositionContext::TowerPlacement);
    _towerPlacementPreview.setPosition(tileCenterPixelPos.toVector2f());
    bool isOccupied = false;
    for (const auto& tower : _towers) {
        if (_map->getGridCoordinates(tower->getPosition()) == gridCoords) {
            isOccupied = true;
            break;
        }
    }
    if (_map->isBuildable(gridCoords.y, gridCoords.x) && !isOccupied) {
        _canPlaceTower = true;
        _towerPlacementPreview.setColor(sf::Color(255, 255, 255, 150));
    }
    else {
        _canPlaceTower = false;
        _towerPlacementPreview.setColor(sf::Color(255, 0, 0, 150));
    }
}

void cgame::handleInput(const sf::Event& event, sf::RenderWindow& window) {
    if (_isGameOver || !_map) return;

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::N) {
            requestNextWave();
        }
        else if (event.key.code == sf::Keyboard::Space) {
            // Same as clicking the speed button.
            _speedStep = (_speedStep + 1) % 3;
            _gameSpeedMultiplier = 1.f + _speedStep;
            _isFastForward = (_speedStep != 0);
            _ffButtonSprite.setColor(_isFastForward ? sf::Color(100, 255, 100) : sf::Color::White);
        }
        else if (event.key.code == sf::Keyboard::R) {
            if (isAbilityReady()) {
                _selectingAbilityTarget = !_selectingAbilityTarget;
                _selectingTowerToBuild = false;
            }
        }
        else if (event.key.code == sf::Keyboard::Escape) {
            cancelPendingAction();
        }
        else {
            _instructionsDismissed = true;
        }
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mouseWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            _instructionsDismissed = true;

            // An armed strike consumes the next click on the battlefield.
            if (_selectingAbilityTarget) {
                if (!_towerPanel.getGlobalBounds().contains(mouseWorldPos) &&
                    !_abilityButton.getGlobalBounds().contains(mouseWorldPos)) {
                    castAbilityAt(mouseWorldPos);
                    return;
                }
                _selectingAbilityTarget = false;
            }
            if (_abilityButton.getGlobalBounds().contains(mouseWorldPos)) {
                if (isAbilityReady()) {
                    _selectingAbilityTarget = !_selectingAbilityTarget;
                    _selectingTowerToBuild = false;
                    SoundManager::playSoundEffect("assets/menu_click.ogg");
                }
                return;
            }

            if (_pauseButtonSprite.getGlobalBounds().contains(mouseWorldPos)) {
                setPaused(true); SoundManager::playSoundEffect("assets/menu_click.ogg"); return;
            }
            if (_ffButtonSprite.getGlobalBounds().contains(mouseWorldPos)) {
                _speedStep = (_speedStep + 1) % 3;
                _gameSpeedMultiplier = 1.f + _speedStep;
                _isFastForward = (_speedStep != 0);
                _ffButtonSprite.setColor(_isFastForward ? sf::Color(100, 255, 100) : sf::Color::White);
                return;
            }
            if (!_waveInProgress && _startWaveButton.getGlobalBounds().contains(mouseWorldPos)) {
                SoundManager::playSoundEffect("assets/menu_click.ogg");
                requestNextWave();
                return;
            }
            for (const auto& button : _towerSelectionButtons) {
                if (button.buttonShape.getGlobalBounds().contains(mouseWorldPos) && button.isEnabled) {
                    selectTowerToBuild(button.towerTypeId);
                    return; // Thoát sớm để không xử lý các logic click khác
                }
            }

            if (_isUpgradePanelVisible) {
                if (_upgradeButton.getGlobalBounds().contains(mouseWorldPos)) { handleUpgrade(); return; }
                if (_sellButton.getGlobalBounds().contains(mouseWorldPos)) { handleSell(); return; }
            }

            if (_selectingTowerToBuild) {
                sf::Vector2i gridCoords = _map->getGridCoordinates(mouseWorldPos);
                if (_map->isBuildable(gridCoords.y, gridCoords.x)) {
                    bool tileIsOccupied = false;
                    for (const auto& existingTower : _towers) {
                        if (_map->getGridCoordinates(existingTower->getPosition()) == gridCoords) {
                            tileIsOccupied = true;
                            break;
                        }
                    }
                    if (!tileIsOccupied) {
                        const auto& blueprint = _towerBlueprints.at(_selectedTowerType);
                        int buildCost = blueprint[0].cost;
                        if (buildCost > 0 && _money >= buildCost) {
                            cpoint towerPosition = _map->getPixelPosition(static_cast<float>(gridCoords.y), static_cast<float>(gridCoords.x), PositionContext::TowerPlacement);

                            if (_selectedTowerType == "ArcherTower") {
                                _towers.push_back(std::make_unique<carchertower>(this, blueprint[0], towerPosition, nextTowerId++));
                            }
                            else if (_selectedTowerType == "CannonTower") {
                                _towers.push_back(std::make_unique<ccannontower>(this, blueprint[0], towerPosition, nextTowerId++));
                            }
                            
                            // Draw order: towers higher up the screen render
                            // first so the ones in front overlap them.
                            std::stable_sort(_towers.begin(), _towers.end(),
                                [](const std::unique_ptr<cbasictower>& a, const std::unique_ptr<cbasictower>& b) {
                                    return a->getPosition().y < b->getPosition().y;
                                });


                            SoundManager::playSoundEffect("assets/tower_place.ogg");
                            _money -= buildCost;
                            std::cout << "Built " << _selectedTowerType << " at tile ("
                                << gridCoords.y << "," << gridCoords.x << "). Gold left: "
                                << _money << std::endl;
                            _selectingTowerToBuild = false;
                            _selectedTower = nullptr;
                            _isUpgradePanelVisible = false;
                            for (auto& button : _towerSelectionButtons) {
                                button.buttonShape.setOutlineColor(sf::Color(60, 60, 80, 255));
                            }
                        }
                        else {
                            std::cout << "Khong du tien de xay thap!" << std::endl;
                            _selectingTowerToBuild = false;
                        }
                    }
                }
                else {
                    std::cout << "Khong the dat thap o vi tri nay." << std::endl;
                }
            }
            else {
                handleTowerSelection(mouseWorldPos);
            }
        }
    }
}
void cgame::handleTowerSelection(const sf::Vector2f& mousePos) {
    bool towerClicked = false;
    for (auto& tower : _towers) {
        if (tower->getGlobalBounds().contains(mousePos)) {
            if (tower->getCurrentState() == cbasictower::State::CONSTRUCTING && tower->getLevel() == 1) {
                _selectedTower = nullptr;
                _isUpgradePanelVisible = false;
                return;
            }
            _selectedTower = tower.get();
            _isUpgradePanelVisible = true;
            towerClicked = true;
            break;
        }
    }
    if (!towerClicked) {
        _selectedTower = nullptr;
        _isUpgradePanelVisible = false;
    }
}

// ESC first backs out of whatever the player is in the middle of. Only when
// there is nothing to back out of does the caller open the pause menu.
bool cgame::cancelPendingAction() {
    const bool hadSomethingToCancel = _selectingTowerToBuild || _isUpgradePanelVisible ||
        _selectingAbilityTarget;

    _selectingAbilityTarget = false;
    _selectingTowerToBuild = false;
    _selectedTower = nullptr;
    _isUpgradePanelVisible = false;
    for (auto& button : _towerSelectionButtons) {
        button.buttonShape.setOutlineColor(sf::Color(60, 60, 80, 255));
    }
    if (!_instructionsDismissed) {
        _instructionsDismissed = true;
        return true;
    }
    return hadSomethingToCancel;
}

void cgame::handleUpgrade() {
    if (_selectedTower && _selectedTower->canUpgrade() && !_selectedTower->isBusy()) {
        int upgradeCost = _selectedTower->getUpgradeCost();
        if (upgradeCost > 0 && _money >= upgradeCost) {
            _money -= upgradeCost;
            _selectedTower->upgrade();
            SoundManager::playSoundEffect("assets/tower_upgrade.ogg");
        }
        else if (upgradeCost <= 0) {
            std::cerr << "Loi: Chi phi nang cap khong hop le (" << upgradeCost << ")" << std::endl;
        }
        else {
            std::cout << "Khong du tien de nang cap!" << std::endl;
        }
    }
}

void cgame::handleSell() {
    if (!_selectedTower || _selectedTower->isBusy()) return;
    int sellValue = _selectedTower->getSellValue();
    _money += sellValue;
    _selectedTower->sell();
    SoundManager::playSoundEffect("assets/tower_sell.ogg");
    _selectedTower = nullptr;
    _isUpgradePanelVisible = false;
}

void cgame::updateInterMission(sf::Time deltaTime) {
    if (_intermissionTimer > sf::Time::Zero) {
        _intermissionTimer -= deltaTime;
        std::stringstream ss;
        ss << "Next wave in " << static_cast<int>(_intermissionTimer.asSeconds()) + 1 << "s";
        _timerText.setString(ss.str());
    }
    else {
        _inIntermission = false;
        _timerText.setString("");
        if (!_levelWon) startNextWave();
    }
}

// Keeps every HUD string in one place so the numbers cannot drift apart.
void cgame::updateHudText() {
    std::stringstream ssLives, ssMoney, ssWave;
    ssLives << _lives << " / " << _maxLives;
    ssMoney << _money;
    if (_currentWave > 0) ssWave << "Wave " << _currentWave << " / " << _totalWaves;
    else ssWave << "Wave - / " << _totalWaves;
    _livesText.setString(ssLives.str());
    _moneyText.setString(ssMoney.str());
    _waveText.setString(ssWave.str());

    // Lives turn red once the run is in danger.
    const float livesRatio = (_maxLives > 0) ? static_cast<float>(_lives) / _maxLives : 0.f;
    if (livesRatio > 0.5f) _livesText.setFillColor(sf::Color::White);
    else if (livesRatio > 0.25f) _livesText.setFillColor(sf::Color(255, 190, 80));
    else _livesText.setFillColor(sf::Color(255, 90, 90));

    // How much of the current wave is still standing.
    if (_waveInProgress) {
        int aliveOrPending = (_enemiesPerWave - _enemiesSpawnedThisWave);
        for (const auto& enemy : _enemies) {
            if (enemy.isAlive()) aliveOrPending++;
        }
        _waveProgressText.setString("Enemies left: " + std::to_string(aliveOrPending));
    }
    else {
        _waveProgressText.setString("");
    }

    // One short hint line, matched to what the player can usefully do now.
    if (_waveInProgress) {
        _hintText.setString("");
    }
    else if (_inIntermission) {
        const int bonus = static_cast<int>(_intermissionTimer.asSeconds()) * 4;
        _hintText.setString("Press N to call the wave early  (+" + std::to_string(bonus) + " gold)");
    }
    else {
        _hintText.setString("Build your towers, then press N to start wave " +
            std::to_string(_currentWave + 1));
    }

    // Preview of what is coming, so towers can be chosen against the actual
    // wave rather than guessed at.
    if (!_waveInProgress && !_pendingWaveQueue.empty()) {
        _wavePreviewText.setString("Next wave " + std::to_string(_pendingWaveNumber) +
            ":   " + describeWave(_pendingWaveQueue));
    }
    else {
        _wavePreviewText.setString("");
    }
}

void cgame::update(sf::Time deltaTime) {
    sf::Time modifiedDeltaTime = deltaTime * _gameSpeedMultiplier;
    if (_levelIsActive && !_isGameOver && !_levelWon) {
        _levelTime += modifiedDeltaTime;
    }
    if (_levelWon) {
        _messageText.setString("VICTORY!");
        _timerText.setString("");
        _levelIsActive = false;
        return;
    }
    if (_isPaused || _isGameOver) {
        if (_isGameOver) _levelIsActive = false;
        return;
    }
    updateTowerSelectionPanel();
    updateEffects(modifiedDeltaTime);
    if (!_waveInProgress) ensurePendingWave();

    if (_inIntermission) {
        updateInterMission(modifiedDeltaTime);
    }
    else if (_waveInProgress) {
        _timeSinceLastSpawn += modifiedDeltaTime;
        if (_timeSinceLastSpawn >= _spawnInterval && _enemiesSpawnedThisWave < _enemiesPerWave) {
            spawnEnemy();
            _timeSinceLastSpawn = sf::Time::Zero;
        }
        if (_enemiesSpawnedThisWave >= _enemiesPerWave && _enemies.empty()) {
            _bullets.clear();
            _waveInProgress = false;
            awardWaveClearBonus();
            if (_currentWave >= totalWavesForCurrentMap()) {
                _levelWon = true;
            }
            else {
                // The build phase between waves is now fully interactive, so
                // the timer is a convenience rather than a lock-out.
                _inIntermission = true;
                _intermissionTimer = _intermissionTime;
            }
        }
    }

    // Everything below keeps running through the intermission so towers finish
    // their build and upgrade animations instead of freezing between waves.
    updateEnemies(modifiedDeltaTime);
    if (_isGameOver) return;
    updateTowers(modifiedDeltaTime);
    updateBullets(modifiedDeltaTime);
    handleCollisions();
    cleanupInactiveObjects();
    updateUpgradePanel();
    updateHudText();

    if (!_inIntermission) {
        _messageText.setString("");
    }
}

// Shows every tile the selected tower can go on. The README always promised
// green build tiles, but nothing ever drew them, so players had to hunt for a
// legal spot by sweeping the mouse across the map.
void cgame::renderBuildableTiles(sf::RenderWindow& window) {
    if (!_selectingTowerToBuild || !_map) return;

    const float tile = static_cast<float>(CURRENT_TILE_SIZE);
    sf::RectangleShape marker(sf::Vector2f(tile - 6.f, tile - 6.f));
    marker.setFillColor(sf::Color(80, 230, 110, 55));
    marker.setOutlineThickness(1.f);
    marker.setOutlineColor(sf::Color(120, 255, 150, 110));

    for (int r = 0; r < _map->getMapHeightTiles(); ++r) {
        for (int c = 0; c < _map->getMapWidthTiles(); ++c) {
            if (!_map->isBuildable(r, c)) continue;

            bool occupied = false;
            for (const auto& tower : _towers) {
                if (_map->getGridCoordinates(tower->getPosition()) == sf::Vector2i(c, r)) {
                    occupied = true;
                    break;
                }
            }
            if (occupied) continue;

            marker.setPosition(c * tile + 3.f, r * tile + 3.f);
            window.draw(marker);
        }
    }
}

void cgame::render(sf::RenderWindow& window) {
    if (_map) {
        _map->render(window);
    }
    renderBuildableTiles(window);
    for (auto& tower : _towers) tower->render(window);

    // Painter's algorithm: enemies lower on the screen are nearer the camera,
    // so they must be drawn last. They used to be drawn in spawn order, which
    // let a monster behind another be painted on top of it.
    _renderOrder.clear();
    _renderOrder.reserve(_enemies.size());
    for (auto& enemy : _enemies) _renderOrder.push_back(&enemy);
    std::stable_sort(_renderOrder.begin(), _renderOrder.end(),
        [](const cenemy* a, const cenemy* b) { return a->getRenderDepth() < b->getRenderDepth(); });

    for (cenemy* enemy : _renderOrder) enemy->render(window);
    // Second pass: every health bar sits above every enemy body.
    for (cenemy* enemy : _renderOrder) enemy->renderHealthBar(window);

    for (auto& bullet : _bullets) bullet.render(window);
    renderEffects(window);

    const sf::Vector2f windowSize = sf::Vector2f(viewSizeU(window));

    window.draw(_uiPanel);
    if (_heartIcon.getSize().x > 0) window.draw(_livesIconSprite);
    window.draw(_livesText);
    if (_coinIcon.getSize().x > 0) window.draw(_moneyIconSprite);
    window.draw(_moneyText);
    if (_waveIcon.getSize().x > 0) window.draw(_waveIconSprite);
    window.draw(_waveText);

    if (!_waveProgressText.getString().isEmpty()) {
        window.draw(_waveProgressText);
    }

    // Rain of Fire button, bottom left, with its cooldown shown as a fill bar.
    {
        const sf::Vector2f abilitySize(150.f, 56.f);
        _abilityButton.setSize(abilitySize);
        _abilityButton.setPosition(24.f, windowSize.y - abilitySize.y - 24.f);
        const bool ready = isAbilityReady();
        _abilityButton.setFillColor(ready ? sf::Color(150, 60, 30, 235) : sf::Color(48, 48, 58, 225));
        _abilityButton.setOutlineThickness(2.f);
        _abilityButton.setOutlineColor(_selectingAbilityTarget ? sf::Color::Yellow
            : (ready ? sf::Color(255, 170, 90) : sf::Color(80, 80, 95)));
        window.draw(_abilityButton);

        if (!ready) {
            _abilityCooldownBar.setSize(sf::Vector2f(abilitySize.x * getAbilityCooldownRatio(), 5.f));
            _abilityCooldownBar.setPosition(_abilityButton.getPosition().x,
                _abilityButton.getPosition().y + abilitySize.y - 5.f);
            _abilityCooldownBar.setFillColor(sf::Color(255, 170, 90, 220));
            window.draw(_abilityCooldownBar);
        }

        if (ready) {
            _abilityText.setString("RAIN OF FIRE\n      (R)");
            _abilityText.setFillColor(sf::Color::White);
        }
        else {
            _abilityText.setString("RAIN OF FIRE\n      " +
                std::to_string(static_cast<int>(_abilityCooldown.asSeconds()) + 1) + "s");
            _abilityText.setFillColor(sf::Color(165, 170, 185));
        }
        sf::FloatRect ab = _abilityText.getLocalBounds();
        _abilityText.setOrigin(ab.left + ab.width / 2.f, ab.top + ab.height / 2.f);
        _abilityText.setPosition(_abilityButton.getPosition() + abilitySize / 2.f);
        window.draw(_abilityText);
    }

    // Wave preview banner, centred just under the top edge.
    if (!_wavePreviewText.getString().isEmpty() && !_levelWon && !_isGameOver) {
        sf::FloatRect pb = _wavePreviewText.getLocalBounds();
        _wavePreviewText.setOrigin(pb.left + pb.width / 2.f, pb.top + pb.height / 2.f);
        _wavePreviewText.setPosition(windowSize.x / 2.f, 108.f);

        sf::RectangleShape banner(sf::Vector2f(pb.width + 40.f, pb.height + 24.f));
        banner.setOrigin(banner.getSize().x / 2.f, banner.getSize().y / 2.f);
        banner.setPosition(windowSize.x / 2.f, 108.f);
        banner.setFillColor(sf::Color(18, 24, 44, 215));
        banner.setOutlineThickness(1.5f);
        banner.setOutlineColor(sf::Color(120, 100, 60, 220));
        window.draw(banner);
        window.draw(_wavePreviewText);
    }

    float pauseButtonX = windowSize.x - _pauseButtonSprite.getGlobalBounds().width - 20.f;
    _pauseButtonSprite.setPosition(pauseButtonX, 20.f);
    float ffButtonX = pauseButtonX - _ffButtonSprite.getGlobalBounds().width - 10.f;
    _ffButtonSprite.setPosition(ffButtonX, 20.f);
    window.draw(_ffButtonSprite);
    window.draw(_pauseButtonSprite);

    // Current speed, drawn under the fast-forward button so the player can see
    // which of the three steps is active.
    _speedLabel.setString(std::to_string(static_cast<int>(_gameSpeedMultiplier)) + "x");
    _speedLabel.setPosition(ffButtonX + _ffButtonSprite.getGlobalBounds().width / 2.f -
        _speedLabel.getLocalBounds().width / 2.f, 62.f);
    window.draw(_speedLabel);

    // Between waves, offer an explicit button instead of relying on the player
    // knowing about the N key.
    if (!_waveInProgress && !_levelWon && !_isGameOver) {
        const sf::Vector2f buttonSize(230.f, 54.f);
        _startWaveButton.setSize(buttonSize);
        _startWaveButton.setPosition(windowSize.x / 2.f - buttonSize.x / 2.f, windowSize.y - 96.f);
        _startWaveButton.setFillColor(sf::Color(40, 140, 70, 235));
        _startWaveButton.setOutlineThickness(2.f);
        _startWaveButton.setOutlineColor(sf::Color(140, 235, 160, 255));

        _startWaveText.setString("START WAVE  (N)");
        sf::FloatRect b = _startWaveText.getLocalBounds();
        _startWaveText.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
        _startWaveText.setPosition(_startWaveButton.getPosition() + buttonSize / 2.f);

        window.draw(_startWaveButton);
        window.draw(_startWaveText);
    }

    renderTowerUI(window);
    renderTowerSelectionPanel(window);

    if (!_hintText.getString().isEmpty() && !_levelWon && !_isGameOver) {
        sf::FloatRect hintBounds = _hintText.getLocalBounds();
        _hintText.setOrigin(hintBounds.left + hintBounds.width / 2.f, hintBounds.top + hintBounds.height / 2.f);
        _hintText.setPosition(windowSize.x / 2.f, windowSize.y - 130.f);
        window.draw(_hintText);
    }

    if (!_messageText.getString().isEmpty()) {
        sf::FloatRect textBounds = _messageText.getLocalBounds();
        _messageText.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        _messageText.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);
        window.draw(_messageText);
    }
    if (_inIntermission) {
        // Moved off the centre of the map so it stops covering the battlefield.
        sf::FloatRect timerBounds = _timerText.getLocalBounds();
        _timerText.setOrigin(timerBounds.left + timerBounds.width / 2.f, timerBounds.top + timerBounds.height / 2.f);
        _timerText.setPosition(windowSize.x / 2.f, 60.f);
        window.draw(_timerText);
    }
    if (_selectingTowerToBuild) {
        updateTowerPlacementPreview(window);

        // Show what the tower would actually cover before paying for it.
        auto it = _towerBlueprints.find(_selectedTowerType);
        if (it != _towerBlueprints.end() && !it->second.empty()) {
            const float radius = it->second[0].range;
            sf::CircleShape rangePreview(radius);
            rangePreview.setOrigin(radius, radius);
            rangePreview.setPosition(_towerPlacementPreview.getPosition());
            rangePreview.setFillColor(sf::Color(255, 255, 255, 20));
            rangePreview.setOutlineThickness(1.5f);
            rangePreview.setOutlineColor(_canPlaceTower ? sf::Color(140, 255, 160, 170)
                : sf::Color(255, 120, 120, 170));
            window.draw(rangePreview);
        }
        window.draw(_towerPlacementPreview);
    }

    // Drawn last so nothing overlaps the opening instructions.
    if (!_instructionsDismissed && !_waveInProgress && !_levelWon && !_isGameOver) {
        renderInstructionPanel(window);
    }
}

void cgame::renderTowerUI(sf::RenderWindow& window) {
    if (!_selectedTower || !_isUpgradePanelVisible) return;

    const cbasictower& tower = *_selectedTower;
    const TowerLevelData& data = tower.getCurrentLevelData();

    const float radius = data.range;
    sf::CircleShape rangeCircle(radius);
    rangeCircle.setFillColor(sf::Color(100, 100, 100, 30));
    rangeCircle.setOutlineColor(sf::Color(255, 255, 255, 150));
    rangeCircle.setOutlineThickness(1.5f);
    rangeCircle.setOrigin(radius, radius);
    rangeCircle.setPosition(tower.getPosition());
    window.draw(rangeCircle);

    // Splash towers also show the blast they cover, so the difference between
    // the two tower types is visible rather than just described.
    if (data.splashRadius > 0.f) {
        sf::CircleShape splashCircle(data.splashRadius);
        splashCircle.setFillColor(sf::Color(255, 160, 60, 35));
        splashCircle.setOutlineColor(sf::Color(255, 180, 90, 130));
        splashCircle.setOutlineThickness(1.f);
        splashCircle.setOrigin(data.splashRadius, data.splashRadius);
        splashCircle.setPosition(tower.getPosition());
        window.draw(splashCircle);
    }

    window.draw(_upgradeButton);
    window.draw(_upgradeText);
    if (tower.canUpgrade()) window.draw(_costText);
    window.draw(_sellButton);
    window.draw(_sellText);
    window.draw(_sellValueText);

    // Level plus the numbers that actually matter, right under the tower.
    std::stringstream label;
    label << "Lv " << tower.getLevel() << "   DMG " << data.damage
        << "   RNG " << static_cast<int>(data.range);
    sf::Text towerLabel(label.str(), _gameFont, 14);
    towerLabel.setFillColor(sf::Color(235, 240, 255));
    sf::FloatRect labelBounds = towerLabel.getLocalBounds();
    towerLabel.setOrigin(labelBounds.left + labelBounds.width / 2.f, labelBounds.top);

    sf::RectangleShape labelBox(sf::Vector2f(labelBounds.width + 16.f, labelBounds.height + 12.f));
    labelBox.setFillColor(sf::Color(18, 24, 44, 210));
    labelBox.setOrigin(labelBox.getSize().x / 2.f, 0.f);
    labelBox.setPosition(tower.getPosition().x, tower.getPosition().y + 26.f);

    towerLabel.setPosition(tower.getPosition().x, tower.getPosition().y + 32.f);
    window.draw(labelBox);
    window.draw(towerLabel);
}

sf::Texture& cgame::getTexture(const std::string& texturePath) {
    auto it = _textureManager.find(texturePath);
    if (it != _textureManager.end()) {
        return it->second;
    }
    else {
        sf::Texture newTexture;
        if (newTexture.loadFromFile(texturePath)) {
            _textureManager[texturePath] = newTexture;
            return _textureManager[texturePath];
        }
        else {
            std::cerr << "Khong the tai texture: " << texturePath << std::endl;
            static sf::Texture emptyTexture;
            return emptyTexture;
        }
    }
}

void cgame::setPaused(bool paused) {
    _isPaused = paused;
}

bool cgame::isPaused() const {
    return _isPaused;
}

void cgame::updateUpgradePanel() {
    if (!_selectedTower || !_isUpgradePanelVisible) return;
    cbasictower& tower = *_selectedTower;
    sf::Vector2f towerPos = tower.getPosition();
    float buttonWidth = 90.f, buttonHeight = 35.f, buttonSpacing = 10.f;
    float totalWidth = buttonWidth * 2 + buttonSpacing;
    float startX = towerPos.x - totalWidth / 2.f;
    float buttonY = towerPos.y - 120;
    // Keep the panel on the canvas: a tower near an edge used to push its
    // buttons off screen where they could not be clicked.
    if (startX < 8.f) startX = 8.f;
    if (startX + totalWidth > VIRTUAL_WIDTH - 8.f) startX = VIRTUAL_WIDTH - 8.f - totalWidth;
    if (buttonY < 8.f) buttonY = towerPos.y + 40.f;
    _upgradeButton.setSize({ buttonWidth, buttonHeight });
    _upgradeButton.setOrigin(0, 0);
    _upgradeButton.setPosition(startX, buttonY);
    if (tower.isBusy()) {
        _upgradeButton.setFillColor(sf::Color(80, 80, 80));
        _upgradeText.setString("UPGRADING...");
        _costText.setString("");
    }
    else if (tower.canUpgrade()) {
        // Grey the button out when the upgrade is unaffordable, instead of
        // letting the player click it and have nothing happen.
        const bool affordable = (_money >= tower.getUpgradeCost());
        _upgradeButton.setFillColor(affordable ? sf::Color(30, 144, 255) : sf::Color(70, 80, 95));
        _upgradeText.setString("UPGRADE");
        _costText.setString(std::to_string(tower.getUpgradeCost()) + " G");
        _costText.setFillColor(affordable ? sf::Color::White : sf::Color(230, 120, 120));
    }
    else {
        _upgradeButton.setFillColor(sf::Color(80, 80, 80));
        _upgradeText.setString("MAX LEVEL");
        _costText.setString("");
    }
    _upgradeText.setFont(_gameFont);
    _upgradeText.setCharacterSize(14);
    sf::FloatRect textBounds = _upgradeText.getLocalBounds();
    _upgradeText.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
    _upgradeText.setPosition(_upgradeButton.getPosition() + sf::Vector2f(buttonWidth / 2, buttonHeight / 2 - 5));
    _costText.setFont(_gameFont);
    _costText.setCharacterSize(12);
    sf::FloatRect costBounds = _costText.getLocalBounds();
    _costText.setOrigin(costBounds.left + costBounds.width / 2.f, costBounds.top + costBounds.height / 2.f);
    _costText.setPosition(_upgradeButton.getPosition() + sf::Vector2f(buttonWidth / 2, buttonHeight / 2 + 8));
    _sellButton.setSize({ buttonWidth, buttonHeight });
    _sellButton.setOrigin(0, 0);
    _sellButton.setPosition(startX + buttonWidth + buttonSpacing, buttonY);
    if (tower.isBusy()) {
        _sellButton.setFillColor(sf::Color(80, 80, 80));
    }
    else {
        _sellButton.setFillColor(sf::Color(220, 50, 50));
    }
    _sellText.setFont(_gameFont);
    _sellText.setString("SELL");
    _sellText.setCharacterSize(14);
    sf::FloatRect sellTextBounds = _sellText.getLocalBounds();
    _sellText.setOrigin(sellTextBounds.left + sellTextBounds.width / 2.f, sellTextBounds.top + sellTextBounds.height / 2.f);
    _sellText.setPosition(_sellButton.getPosition() + sf::Vector2f(buttonWidth / 2, buttonHeight / 2 - 5));
    _sellValueText.setFont(_gameFont);
    _sellValueText.setString(std::to_string(tower.getSellValue()) + " G");
    _sellValueText.setCharacterSize(12);
    sf::FloatRect sellValueBounds = _sellValueText.getLocalBounds();
    _sellValueText.setOrigin(sellValueBounds.left + sellValueBounds.width / 2.f, sellValueBounds.top + sellValueBounds.height / 2.f);
    _sellValueText.setPosition(_sellButton.getPosition() + sf::Vector2f(buttonWidth / 2, buttonHeight / 2 + 8));
}

bool cgame::hasWon() const {
    return _levelWon;
}

int cgame::getLives() const {
    return _lives;
}

int cgame::getMaxLives() const {
    return _maxLives;
}

std::string cgame::getCurrentMapId() const {
    return _currentMapId;
}
void cgame::saveGame(const std::string& filename) const {
    std::ofstream saveFile(filename);
    if (!saveFile.is_open()) {
        std::cerr << "Error: Could not open save file for writing: " << filename << std::endl;
        return;
    }
    saveFile << "map_id " << _currentMapId << std::endl;
    saveFile << "lives " << _lives << std::endl;
    saveFile << "max_lives " << _maxLives << std::endl;
    saveFile << "money " << _money << std::endl;
    saveFile << "wave " << _currentWave << std::endl;
    saveFile << "wave_in_progress " << (_waveInProgress ? 1 : 0) << std::endl;
    saveFile << "wave_enemy_type " << _currentWaveEnemyTypeIndex << std::endl;
    saveFile << "enemies_spawned " << _enemiesSpawnedThisWave << std::endl;
    saveFile << "spawn_timer " << _timeSinceLastSpawn.asSeconds() << std::endl;
    // Progress and score state, so a resumed run keeps its stats instead of
    // reporting zero kills and zero time on the victory screen.
    saveFile << "enemies_defeated " << _enemiesDefeated << std::endl;
    saveFile << "level_time " << _levelTime.asSeconds() << std::endl;
    saveFile << "in_intermission " << (_inIntermission ? 1 : 0) << std::endl;
    saveFile << "intermission_timer " << _intermissionTimer.asSeconds() << std::endl;

    // The rest of the wave's spawn order. Without it, resuming mid-wave used to
    // invent a fresh set of enemy types.
    saveFile << "spawn_queue " << _spawnQueue.size();
    for (int typeIndex : _spawnQueue) saveFile << " " << typeIndex;
    saveFile << std::endl;

    int validTowers = 0;
    for (const auto& tower : _towers) if (!tower->isPendingRemoval()) validTowers++;
    saveFile << "towers_count " << validTowers << std::endl;
    for (const auto& tower : _towers) {
        if (!tower->isPendingRemoval()) {
            saveFile << "tower " << tower->getTypeId() << " " << tower->getLevel() << " "
                << tower->getPosition().x << " " << tower->getPosition().y << std::endl;
        }
    }

    // Count only the enemies actually written below; the old code wrote
    // _enemies.size() but then skipped the dying ones.
    int savedEnemies = 0;
    for (const auto& enemy : _enemies) {
        if (enemy.isActive() && enemy.isAlive()) savedEnemies++;
    }
    saveFile << "enemies_on_map_count " << savedEnemies << std::endl;
    for (const auto& enemy : _enemies) {
        if (enemy.isActive() && enemy.isAlive()) {
            saveFile << "enemy " << enemy.getTypeIndex() << " " << enemy.getHealth() << " "
                << enemy.getPosition().x << " " << enemy.getPosition().y << " "
                << enemy.getPathIndex() << std::endl;
        }
    }
    std::cout << "Game saved successfully to " << filename << std::endl;
    saveFile.close();
}

bool cgame::loadGame(const std::string& filename) {
    std::ifstream saveFile(filename);
    if (!saveFile.is_open()) {
        std::cerr << "Info: No save file found at: " << filename << std::endl;
        return false;
    }
    resetGameStats();
    std::string key;
    int towersCount = 0, enemiesOnMapCount = 0;
    while (saveFile >> key) {
        if (key == "map_id") saveFile >> _currentMapId;
        else if (key == "lives") saveFile >> _lives;
        else if (key == "max_lives") saveFile >> _maxLives;
        else if (key == "money") saveFile >> _money;
        else if (key == "wave") saveFile >> _currentWave;
        else if (key == "wave_in_progress") { int v; saveFile >> v; _waveInProgress = (v == 1); }
        else if (key == "wave_enemy_type") saveFile >> _currentWaveEnemyTypeIndex;
        else if (key == "enemies_spawned") saveFile >> _enemiesSpawnedThisWave;
        else if (key == "spawn_timer") { float s; saveFile >> s; _timeSinceLastSpawn = sf::seconds(s); }
        else if (key == "enemies_defeated") saveFile >> _enemiesDefeated;
        else if (key == "level_time") { float s; saveFile >> s; _levelTime = sf::seconds(s); }
        else if (key == "in_intermission") { int v; saveFile >> v; _inIntermission = (v == 1); }
        else if (key == "intermission_timer") { float s; saveFile >> s; _intermissionTimer = sf::seconds(s); }
        else if (key == "spawn_queue") {
            size_t queueSize = 0;
            saveFile >> queueSize;
            _spawnQueue.clear();
            _spawnQueue.reserve(queueSize);
            for (size_t i = 0; i < queueSize; ++i) {
                int typeIndex = 0;
                if (!(saveFile >> typeIndex)) break;
                _spawnQueue.push_back(typeIndex);
            }
        }
        else if (key == "towers_count") saveFile >> towersCount;
        else if (key == "tower") {
            std::string typeId; int level; float posX, posY;
            saveFile >> typeId >> level >> posX >> posY;
            auto it = _towerBlueprints.find(typeId);
            if (it != _towerBlueprints.end()) {
                const auto& towerLevels = it->second;
                if (level > 0 && static_cast<size_t>(level) <= towerLevels.size()) {
                    const TowerLevelData& levelData = towerLevels[level - 1];
                    cpoint towerPosition(posX, posY);
                    if (typeId == "ArcherTower") {
                        _towers.push_back(std::make_unique<carchertower>(this, levelData, towerPosition, nextTowerId++));
                    }
                    else if (typeId == "CannonTower") {
                        _towers.push_back(std::make_unique<ccannontower>(this, levelData, towerPosition, nextTowerId++));
                    }
                    else {
                        continue;
                    }
                    // A restored tower only knew about its final level's cost,
                    // so selling it refunded far too little. Rebuild the real
                    // total from the blueprint.
                    int invested = 0;
                    for (int l = 0; l < level; ++l) invested += towerLevels[l].cost;
                    _towers.back()->restoreInvestment(invested);
                }
            }
        }
        else if (key == "enemies_on_map_count") {
            saveFile >> enemiesOnMapCount;
            if (enemiesOnMapCount > 0) _enemies.reserve(enemiesOnMapCount);
        }
        else if (key == "enemy") {
            if (!_map) continue;
            int typeIndex, pathIndex; float health, posX, posY;
            saveFile >> typeIndex >> health >> posX >> posY >> pathIndex;
            if (typeIndex >= 0 && static_cast<size_t>(typeIndex) < _availableEnemyTypes.size()) {
                // Rebuild the enemy with the same wave scaling it spawned with,
                // otherwise its health bar reads against the unscaled maximum.
                EnemyType scaledType = _availableEnemyTypes.at(typeIndex);
                const int waveForScaling = (_currentWave > 0) ? _currentWave : 1;
                scaledType.health = static_cast<int>(scaledType.health * std::pow(1.22, waveForScaling - 1));
                scaledType.speed = scaledType.speed * (1.f + (waveForScaling - 1) * 0.04f);
                scaledType.moneyValue = scaledType.moneyValue + waveForScaling * 2;

                const auto& path = _map->getEnemyPath();
                _enemies.emplace_back(this, scaledType, typeIndex, path);
                cenemy& newEnemy = _enemies.back();
                newEnemy.setHealth(health);
                newEnemy.setPosition(cpoint(posX, posY));
                newEnemy.setPathIndex(pathIndex);
            }
        }
    }

    _totalWaves = totalWavesForCurrentMap();
    _enemiesPerWave = static_cast<int>(_spawnQueue.size());
    if (_currentWave > 0) {
        _levelIsActive = true;
        _instructionsDismissed = true;
    }
    if (_waveInProgress && _spawnQueue.empty()) {
        // Older save files carry no spawn queue; rebuild one so the wave can
        // still finish instead of hanging with no enemies left to spawn.
        buildWaveComposition();
        if (_enemiesSpawnedThisWave > _enemiesPerWave) _enemiesSpawnedThisWave = _enemiesPerWave;
    }
    _messageText.setString("");
    updateHudText();

    std::cout << "Game loaded successfully from " << filename << std::endl;
    saveFile.close();
    return true;
}

int cgame::getEnemiesDefeated() const {
    return _enemiesDefeated;
}

sf::Time cgame::getLevelTime() const {
    return _levelTime;
}

long cgame::calculateScore() const {
    long score = _enemiesDefeated * 100;
    long timePenalty = static_cast<long>(_levelTime.asSeconds());
    score -= timePenalty;
    return std::max(0L, score);
}

void cgame::renderInstructionPanel(sf::RenderWindow& window) {
    sf::Vector2f windowSize = sf::Vector2f(viewSizeU(window));
    sf::Vector2f windowCenter = windowSize / 2.f;

    sf::RectangleShape panel;
    panel.setSize({ windowSize.x * 0.66f, windowSize.y * 0.60f });
    panel.setFillColor(sf::Color(25, 40, 80, 235));
    panel.setOutlineColor(sf::Color(100, 120, 180, 255));
    panel.setOutlineThickness(3.f);
    panel.setOrigin(panel.getSize() / 2.f);
    panel.setPosition(windowCenter);

    sf::Text title;
    title.setFont(_gameFont);
    title.setString("INSTRUCTIONS");
    title.setCharacterSize(48);
    title.setStyle(sf::Text::Bold);
    title.setFillColor(sf::Color::Yellow);

    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2.f, titleBounds.top + titleBounds.height / 2.f);
    title.setPosition(windowCenter.x, panel.getPosition().y - panel.getSize().y / 2.f + 50.f);

    std::vector<sf::Text> lines;
    std::vector<std::string> instructions = {
        "Archer Tower - cheap, fast, long range, hits one enemy.",
        "Cannon Tower - costly, slow, short range, hits a whole group.",
        "",
        "Armoured enemies soak a flat amount from EVERY hit, so a few heavy",
        "cannon shells beat a stream of light arrows. The banner at the top",
        "shows what the next wave brings and how armoured it is.",
        "",
        "Pick a tower at the bottom right, then click a green tile.",
        "RAIN OF FIRE (R) calls an area strike that ignores armour.",
        "SPACE cycles speed 1x / 2x / 3x.   N calls the wave early for gold.",
        "",
        "PRESS 'N' OR CLICK ANYWHERE TO BEGIN"
    };

    float textStartY = title.getPosition().y + 80.f;
    for (size_t i = 0; i < instructions.size(); ++i) {
        sf::Text line;
        line.setFont(_gameFont);
        line.setString(instructions[i]);
        line.setCharacterSize(22);
        line.setFillColor(sf::Color::White);

        if (i == instructions.size() - 1) {
            line.setCharacterSize(26);
            line.setStyle(sf::Text::Bold);
            line.setFillColor(sf::Color(100, 255, 100));
        }

        sf::FloatRect lineBounds = line.getLocalBounds();
        line.setOrigin(lineBounds.left + lineBounds.width / 2.f, lineBounds.top + lineBounds.height / 2.f);
        line.setPosition(windowCenter.x, textStartY + (i * 34.f));
        lines.push_back(line);
    }

    window.draw(panel);
    window.draw(title);
    for (const auto& line : lines) {
        window.draw(line);
    }
}

// Friendly name for a blueprint id, used on the build bar and its tooltip.
static std::string towerDisplayName(const std::string& typeId) {
    if (typeId == "ArcherTower") return "Archer";
    if (typeId == "CannonTower") return "Cannon";
    return typeId;
}

void cgame::setupTowerSelectionPanel(sf::RenderWindow& window) {
    const sf::Vector2u windowSize = viewSizeU(window);

    const float panelWidth = 260.f;
    const float panelHeight = 132.f;
    const float buttonSize = 84.f;
    const float spacing = 16.f;
    const float panelX = static_cast<float>(windowSize.x) - panelWidth;
    const float panelY = static_cast<float>(windowSize.y) - panelHeight;
    _towerPanel.setSize({ panelWidth, panelHeight });
    _towerPanel.setPosition(panelX, panelY);
    _towerPanel.setFillColor(sf::Color(20, 20, 30, 225));
    _towerPanel.setOutlineColor(sf::Color(60, 60, 80, 255));
    _towerPanel.setOutlineThickness(2.f);

    // Give the start-wave button its geometry up front. render() lays it out
    // every frame, but input is handled before the first render, so without
    // this its bounds would be an empty rect on frame one.
    const sf::Vector2f startButtonSize(230.f, 54.f);
    _startWaveButton.setSize(startButtonSize);
    _startWaveButton.setPosition(windowSize.x / 2.f - startButtonSize.x / 2.f,
        static_cast<float>(windowSize.y) - 96.f);

    _towerSelectionButtons.clear();
    if (_towerBlueprints.empty()) return;

    const float count = static_cast<float>(_towerBlueprints.size());
    const float totalButtonsWidth = (count * buttonSize) + ((count - 1.f) * spacing);
    const float buttonsStartX = panelX + (panelWidth - totalButtonsWidth) / 2.f;
    const float buttonsY = panelY + panelHeight - buttonSize - 10.f;

    int i = 0;
    for (const auto& pair : _towerBlueprints) {
        const std::string& typeId = pair.first;
        const auto& levelData = pair.second[0];

        TowerSelectionButton newButton;
        newButton.towerTypeId = typeId;

        newButton.buttonShape.setSize({ buttonSize, buttonSize });
        newButton.buttonShape.setPosition(buttonsStartX + i * (buttonSize + spacing), buttonsY);
        newButton.buttonShape.setFillColor(sf::Color(50, 50, 70, 255));
        newButton.buttonShape.setOutlineThickness(2.f);
        newButton.buttonShape.setOutlineColor(sf::Color(60, 60, 80, 255));

        newButton.towerIcon.setTexture(getTexture(levelData.idle_texturePath));
        newButton.towerIcon.setTextureRect(sf::IntRect(levelData.idle_startFrame * levelData.frameSize.x, 0, levelData.frameSize.x, levelData.frameSize.y));
        newButton.towerIcon.setScale(0.5f, 0.5f);
        sf::FloatRect iconBounds = newButton.towerIcon.getLocalBounds();
        newButton.towerIcon.setOrigin(iconBounds.width / 2.f, iconBounds.height / 2.f);
        newButton.towerIcon.setPosition(newButton.buttonShape.getPosition().x + buttonSize / 2.f,
            newButton.buttonShape.getPosition().y + buttonSize / 2.f - 8.f);

        newButton.costText.setFont(_gameFont);
        newButton.costText.setString(std::to_string(levelData.cost) + " G");
        newButton.costText.setCharacterSize(16);
        newButton.costText.setFillColor(sf::Color::Yellow);
        sf::FloatRect textBounds = newButton.costText.getLocalBounds();
        newButton.costText.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        newButton.costText.setPosition(newButton.buttonShape.getPosition().x + buttonSize / 2.f,
            newButton.buttonShape.getPosition().y + buttonSize - 13.f);

        newButton.nameText.setFont(_gameFont);
        newButton.nameText.setString(towerDisplayName(typeId));
        newButton.nameText.setCharacterSize(14);
        newButton.nameText.setFillColor(sf::Color(210, 220, 240));
        sf::FloatRect nameBounds = newButton.nameText.getLocalBounds();
        newButton.nameText.setOrigin(nameBounds.left + nameBounds.width / 2.f, nameBounds.top + nameBounds.height / 2.f);
        newButton.nameText.setPosition(newButton.buttonShape.getPosition().x + buttonSize / 2.f, panelY + 18.f);

        _towerSelectionButtons.push_back(newButton);
        i++;
    }
}

void cgame::updateTowerSelectionPanel() {
    for (auto& button : _towerSelectionButtons) {
        const auto& levelData = _towerBlueprints.at(button.towerTypeId)[0];
        const bool isSelected = (_selectingTowerToBuild && _selectedTowerType == button.towerTypeId);

        if (_money >= levelData.cost) {
            button.isEnabled = true;
            button.buttonShape.setFillColor(sf::Color(50, 50, 70, 255));
            button.towerIcon.setColor(sf::Color::White);
            button.costText.setFillColor(sf::Color::Yellow);
        }
        else {
            button.isEnabled = false;
            button.buttonShape.setFillColor(sf::Color(30, 30, 30, 255));
            button.towerIcon.setColor(sf::Color(128, 128, 128, 200));
            // Say *why* the button is dead instead of just dimming it.
            button.costText.setFillColor(sf::Color(220, 110, 110));
        }
        button.buttonShape.setOutlineColor(isSelected ? sf::Color::Yellow : sf::Color(60, 60, 80, 255));
    }
}

void cgame::renderTowerSelectionPanel(sf::RenderWindow& window) {
    window.draw(_towerPanel);
    for (const auto& button : _towerSelectionButtons) {
        window.draw(button.buttonShape);
        window.draw(button.towerIcon);
        window.draw(button.nameText);
        window.draw(button.costText);
    }

    // Hover tooltip. The stats used to be invisible, so there was no way to
    // tell the two towers apart without buying one.
    const sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    for (const auto& button : _towerSelectionButtons) {
        if (!button.buttonShape.getGlobalBounds().contains(mousePos)) continue;

        const auto& data = _towerBlueprints.at(button.towerTypeId)[0];
        std::stringstream body;
        body << towerDisplayName(button.towerTypeId) << "\n"
            << "Cost    " << data.cost << " G\n"
            << "Damage  " << data.damage << "\n"
            << "Range   " << static_cast<int>(data.range) << "\n"
            << "Rate    " << std::fixed << std::setprecision(2) << data.fireRate << "s";
        if (data.splashRadius > 0.f) {
            body << "\nSplash  " << static_cast<int>(data.splashRadius);
        }

        sf::Text tip(body.str(), _gameFont, 15);
        tip.setFillColor(sf::Color(235, 240, 255));
        tip.setLineSpacing(1.25f);

        const sf::FloatRect tipBounds = tip.getLocalBounds();
        sf::RectangleShape tipBox(sf::Vector2f(tipBounds.width + 24.f, tipBounds.height + 24.f));
        tipBox.setFillColor(sf::Color(18, 24, 44, 240));
        tipBox.setOutlineThickness(1.5f);
        tipBox.setOutlineColor(sf::Color(110, 130, 190, 255));

        // Anchor above the build bar and keep it inside the canvas.
        float boxX = button.buttonShape.getPosition().x - 20.f;
        const float canvasWidth = static_cast<float>(viewSizeU(window).x);
        if (boxX + tipBox.getSize().x > canvasWidth - 8.f) {
            boxX = canvasWidth - 8.f - tipBox.getSize().x;
        }
        const float boxY = _towerPanel.getPosition().y - tipBox.getSize().y - 10.f;

        tipBox.setPosition(boxX, boxY);
        tip.setPosition(boxX + 12.f - tipBounds.left, boxY + 12.f - tipBounds.top);

        window.draw(tipBox);
        window.draw(tip);
        break;
    }
}
