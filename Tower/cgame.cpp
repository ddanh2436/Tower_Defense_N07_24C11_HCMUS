#include "cgame.h"
#include "SoundManager.h"
#include <iostream>
#include <sstream>
#include <random>
#include <algorithm>
#include <fstream>
#include <cmath>

static int nextTowerId = 0;

cgame::cgame() : _rng(std::random_device{}()) {
    _map = nullptr;

    loadFont();
    setupUI();
    setupEnemyTypes();
    setupTowerTypes();
    _isPaused = false;
    resetGame();
}

cgame::~cgame() {
    if (_map) {
        delete _map;
        _map = nullptr;
    }
}

void cgame::loadMap(const std::string& mapId, const std::string& dataFilePath) {
    std::cout << "Loading map: " << mapId << " from " << dataFilePath << std::endl;
    _currentMapId = mapId;
    // Xóa map cũ nếu có
    if (_map) {
        delete _map;
        _map = nullptr;
    }

    // Tạo map mới từ thông tin được cung cấp
    _map = new cmap(dataFilePath, mapId);

    // Reset lại trạng thái game cho map mới
    resetGame();
}

void cgame::setupTowerTypes() {
    _towerBlueprints.clear();

    sf::Vector2i archerFrameSize = { 70, 115 };
    int archerFrameOffsetY = 15;

    std::vector<TowerLevelData> archerLevels;

    // ----- CẤP 1 -----
    archerLevels.push_back(TowerLevelData{
        /*level*/ 1, /*cost*/ 50,
        /*frameSize*/ archerFrameSize, /*frameOffsetY*/ archerFrameOffsetY,
        /*texturePath*/ "assets/4.png", /*startFrame*/ 2, /*numFrames*/ 2, /*speed*/ 0.5f,
        "assets/4_idle.png", 0, 6, 2.0f,
        /*range*/ 150.f, /*fireRate*/ 1.0f, /*damage*/ 25, /*bulletSpeed*/ 200.f
        });

    // ----- CẤP 2 -----
    archerLevels.push_back(TowerLevelData{
        /*level*/ 2, /*cost*/ 75,
        /*frameSize*/ archerFrameSize, /*frameOffsetY*/ archerFrameOffsetY,
        "assets/5.png", 2, 2, 0.4f,
        "assets/5_idle.png", 0, 6, 1.8f,
        175.f, 0.8f, 40, 220.f
        });

    // ----- CẤP 3 -----
    archerLevels.push_back(TowerLevelData{
        /*level*/ 3, /*cost*/ 100,
        /*frameSize*/ archerFrameSize, /*frameOffsetY*/ archerFrameOffsetY,
        "assets/6.png", 2, 2, 0.4f,
        "assets/6_idle.png", 0, 6, 1.8f,
        200.f, 0.9f, 50, 230.f
        });

    // ----- CẤP 4 -----
    archerLevels.push_back(TowerLevelData{
        /*level*/ 4, /*cost*/ 150,
        /*frameSize*/ archerFrameSize, /*frameOffsetY*/ archerFrameOffsetY,
        "assets/7.png", 2, 2, 0.4f,
        "assets/7_idle.png", 0, 6, 1.8f,
        230.f, 1.2f, 65, 235.f
        });

    _towerBlueprints["ArcherTower"] = archerLevels;
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
    goblin.health = 120; goblin.speed = 20.f; goblin.scale = 2.0f; goblin.moneyValue = 30;
    goblin.frameSize = { 36, 48 }; goblin.frameCount = 6; goblin.stride = 48;
    goblin.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U_Walk.png";
    goblin.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D_Walk.png";
    goblin.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S_Walk.png";
    goblin.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U_Death.png";
    goblin.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D_Death.png";
    goblin.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S_Death.png";
    _availableEnemyTypes.push_back(goblin);

    // --- WOLF ---
    EnemyType wolf = goblin;
    wolf.health = 60; wolf.speed = 35.f; wolf.scale = 1.2f; wolf.moneyValue = 20;
    wolf.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U1_Walk.png";
    wolf.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D1_Walk.png";
    wolf.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S1_Walk.png";
    wolf.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U1_Death.png";
    wolf.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D1_Death.png";
    wolf.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S1_Death.png";
    _availableEnemyTypes.push_back(wolf);

    // --- BEE ---
    EnemyType bee = goblin;
    bee.health = 250; bee.speed = 15.f; bee.scale = 2.0f; bee.moneyValue = 20;
    bee.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U2_Walk.png";
    bee.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D2_Walk.png";
    bee.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S2_Walk.png";
    bee.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U2_Death.png";
    bee.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D2_Death.png";
    bee.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S2_Death.png";
    _availableEnemyTypes.push_back(bee);

    // --- SLIME ---
    EnemyType slime = goblin;
    slime.health = 150; slime.speed = 20.f; slime.scale = 2.0f; slime.moneyValue = 20;
    slime.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U3_Walk.png";
    slime.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D3_Walk.png";
    slime.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S3_Walk.png";
    slime.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U3_Death.png";
    slime.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D3_Death.png";
    slime.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S3_Death.png";
    _availableEnemyTypes.push_back(slime);

    // --- SLIME 1 ---
    EnemyType slime1 = goblin;
    slime1.health = 150; slime1.speed = 20.f; slime1.scale = 2.0f; slime1.moneyValue = 20;
    slime1.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U3_Special.png";
    slime1.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D3_Special.png";
    slime1.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S3_Special.png";
    slime1.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U3_Death2.png";
    slime1.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D3_Death2.png";
    slime1.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S3_Death2.png";
    _availableEnemyTypes.push_back(slime1);
}

void cgame::resetGameStats() {
    _isPaused = false;
    _lives = 1;
    _maxLives = 1;
    _money = 1000;
    _currentWave = 0;
    _enemiesPerWave = 5;
    _spawnInterval = sf::seconds(2.0f);
    _timeSinceLastSpawn = sf::Time::Zero;
    _enemiesSpawnedThisWave = 0;
    _isGameOver = false;
    _inIntermission = false;
    _intermissionTime = sf::seconds(5.f);
    _intermissionTimer = sf::Time::Zero;
    _levelWon = false;
    _waveInProgress = false;
    _currentWaveEnemyTypeIndex = -1;
    _isFastForward = false;
    _gameSpeedMultiplier = 2.0f;
    _ffButtonSprite.setColor(sf::Color::White);

    _enemies.clear();
    _towers.clear();
    _bullets.clear();

    _selectingTowerToBuild = false;
    _selectedTower = nullptr;
    _isUpgradePanelVisible = false;
    nextTowerId = 0;

    _enemiesDefeated = 0;
    _levelTime = sf::Time::Zero;
    _levelIsActive = false;
}

void cgame::startNextWave() {
    if (_waveInProgress || _isGameOver || _levelWon) return;

    if (_currentWave == 0) {
        _levelIsActive = true;
    }

    if (!_availableEnemyTypes.empty()) {
        std::uniform_int_distribution<int> dist(0, static_cast<int>(_availableEnemyTypes.size() - 1));
        _currentWaveEnemyTypeIndex = dist(_rng);
    }

    _currentWave++;
    _enemiesSpawnedThisWave = 0;
    _enemiesPerWave = 5 + _currentWave * 2;
    _waveInProgress = true;
    _timeSinceLastSpawn = sf::Time::Zero;
    _messageText.setString("");
    _enemies.reserve(_enemiesPerWave);
    _map->calculateEnemyPath(getCurrentMapId()); // Đặt lại đường đi của quái vật cho wave mới

    std::cout << "Wave " << _currentWave << " Start!" << std::endl;
}

void cgame::spawnEnemy() {
    if (!_map) return;

    if (_enemiesSpawnedThisWave < _enemiesPerWave && _currentWaveEnemyTypeIndex != -1) {
        const auto& path = _map->getEnemyPath();
        if (path.empty()) return;

        const EnemyType& baseType = _availableEnemyTypes.at(_currentWaveEnemyTypeIndex);

        EnemyType finalType = baseType;
        finalType.health = static_cast<int>(baseType.health * std::pow(1.3, _currentWave - 1));
        finalType.speed = baseType.speed * (1.f + (_currentWave - 1) * 0.05f);
        finalType.moneyValue = baseType.moneyValue + (_currentWave * 2);

        // SỬA ĐỔI: Truyền vào typeIndex để đối tượng quái biết nó là loại nào
        _enemies.emplace_back(this, finalType, _currentWaveEnemyTypeIndex, path);

        _enemiesSpawnedThisWave++;
    }
}

// Dán đoạn code này vào file cgame.cpp, thay thế hoàn toàn cho hàm handleCollisions cũ

void cgame::handleCollisions() {
    for (auto& bullet : _bullets) {
        if (!bullet.canCollide()) continue;

        for (auto& enemy : _enemies) {
            // Chỉ kiểm tra va chạm với enemy còn thực sự sống (trạng thái WALKING)
            if (enemy.isAlive()) {
                if (bullet.getGlobalBounds().intersects(enemy.getGlobalBounds())) {
                    bullet.setActive(false); // Vô hiệu hóa đạn ngay khi trúng

                    // Hàm takeDamage giờ sẽ trả về 'true' nếu cú đánh đó giết chết enemy
                    bool wasKilledByThisHit = enemy.takeDamage(bullet.getDamage());

                    // Nếu hàm trả về true, ta mới tính tiền và tăng kill
                    if (wasKilledByThisHit) {
                        _money += enemy.getMoneyValue();
                        _enemiesDefeated++; // <-- SẼ ĐƯỢC CỘNG ĐIỂM CHÍNH XÁC TẠI ĐÂY

                        // In ra console để kiểm tra
                        std::cout << "An enemy was killed! Total kills: " << _enemiesDefeated << std::endl;

                        SoundManager::playSoundEffect("assets/enemy_explode.ogg");
                    }

                    // Thoát vòng lặp enemy vì đạn này đã trúng mục tiêu và hết tác dụng
                    break;
                }
            }
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
    _uiPanel.setSize(sf::Vector2f(200, 120));
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
    _timerText.setCharacterSize(50);
    _timerText.setFillColor(sf::Color::Yellow);

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
                _lives--;
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
        tower.update(deltaTime, _enemies, _bullets);
    }
}

void cgame::updateBullets(sf::Time deltaTime) {
    for (auto& bullet : _bullets) {
        if (bullet.isActive()) {
            bullet.update(deltaTime);
            sf::Vector2f pos = bullet.getPosition();
            if (pos.x < -50 || pos.x > 1024 + 50 || pos.y < -50 || pos.y > 768 + 50) {
                bullet.setActive(false);
            }
        }
    }
}

void cgame::cleanupInactiveObjects() {
    _enemies.erase(std::remove_if(_enemies.begin(), _enemies.end(), [](const cenemy& e) { return e.isReadyForRemoval(); }), _enemies.end());
    _bullets.erase(std::remove_if(_bullets.begin(), _bullets.end(), [](const cbullet& b) { return !b.isActive(); }), _bullets.end());
    _towers.erase(std::remove_if(_towers.begin(), _towers.end(), [](const ctower& t) { return t.isPendingRemoval(); }), _towers.end());
}


void cgame::updateTowerPlacementPreview(sf::RenderWindow& window) {
    if (!_selectingTowerToBuild || !_map) return;
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);
    sf::Vector2i gridCoords = _map->getGridCoordinates(mouseWorldPos);
    cpoint tileCenterPixelPos = _map->getPixelPosition(gridCoords.y, gridCoords.x, PositionContext::TowerPlacement);
    _towerPlacementPreview.setPosition(tileCenterPixelPos.toVector2f());
    bool isOccupied = false;
    for (const auto& tower : _towers) {
        if (_map->getGridCoordinates(tower.getPosition()) == gridCoords) {
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
            if (!_waveInProgress && !_inIntermission) startNextWave();
        }
        if (event.key.code == sf::Keyboard::Num1) {
            const auto& blueprint = _towerBlueprints.at("ArcherTower");
            int buildCost = blueprint[0].cost;
            if (buildCost > 0 && _money >= buildCost) {
                _selectingTowerToBuild = true;
                _selectedTowerType = "ArcherTower";
                static sf::Texture previewTexture;
                if (previewTexture.loadFromFile(blueprint[0].idle_texturePath)) {
                    _towerPlacementPreview.setTexture(previewTexture);
                    _towerPlacementPreview.setTextureRect(sf::IntRect(blueprint[0].idle_startFrame * blueprint[0].frameSize.x, 0, blueprint[0].frameSize.x, blueprint[0].frameSize.y));
                }
                else {
                    std::cerr << "Loi tai texture preview thap!" << std::endl;
                    _selectingTowerToBuild = false;
                    return;
                }
                sf::FloatRect bounds = _towerPlacementPreview.getLocalBounds();
                _towerPlacementPreview.setOrigin(bounds.width / 2.f, bounds.height - blueprint[0].frameOffsetY);
            }
            else if (buildCost <= 0) {
                std::cerr << "Loi: Chi phi xay dung khong hop le (" << buildCost << ")" << std::endl;
            }
            else {
                std::cout << "Khong du tien de mua thap!" << std::endl;
            }
        }
        if (event.key.code == sf::Keyboard::Escape) {
            _selectingTowerToBuild = false;
            _selectedTower = nullptr;
            _isUpgradePanelVisible = false;
        }
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mouseWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if (_pauseButtonSprite.getGlobalBounds().contains(mouseWorldPos)) {
                setPaused(true);
                SoundManager::playSoundEffect("assets/menu_click.wav");
                return;
            }
            if (_ffButtonSprite.getGlobalBounds().contains(mouseWorldPos)) {
                _isFastForward = !_isFastForward;
                _ffButtonSprite.setColor(_isFastForward ? sf::Color(100, 255, 100) : sf::Color::White);
                return;
            }
            if (_isUpgradePanelVisible) {
                if (_upgradeButton.getGlobalBounds().contains(mouseWorldPos)) {
                    handleUpgrade();
                    return;
                }
                if (_sellButton.getGlobalBounds().contains(mouseWorldPos)) {
                    handleSell();
                    return;
                }
            }
            if (_selectingTowerToBuild) {
                sf::Vector2i gridCoords = _map->getGridCoordinates(mouseWorldPos);
                if (_map->isBuildable(gridCoords.y, gridCoords.x)) {
                    bool tileIsOccupied = false;
                    for (const auto& existingTower : _towers) {
                        if (_map->getGridCoordinates(existingTower.getPosition()) == gridCoords) {
                            tileIsOccupied = true;
                            break;
                        }
                    }
                    if (!tileIsOccupied) {
                        const auto& blueprint = _towerBlueprints.at(_selectedTowerType);
                        int buildCost = blueprint[0].cost;
                        if (buildCost > 0 && _money >= buildCost) {
                            cpoint towerPosition = _map->getPixelPosition(gridCoords.y, gridCoords.x, PositionContext::TowerPlacement);
                            _towers.emplace_back(this, _selectedTowerType, blueprint[0], towerPosition, nextTowerId++);
                            SoundManager::playSoundEffect("assets/tower_place.wav");
                            _money -= buildCost;
                            _selectingTowerToBuild = false;
                            _selectedTower = nullptr;
                            _isUpgradePanelVisible = false;
                        }
                        else if (buildCost <= 0) {
                            std::cerr << "Loi: Chi phi xay dung khong hop le (" << buildCost << ")" << std::endl;
                            _selectingTowerToBuild = false;
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
        if (tower.getGlobalBounds().contains(mousePos)) {
            if (tower.getCurrentState() == ctower::State::CONSTRUCTING && tower.getLevel() == 1) {
                _selectedTower = nullptr;
                _isUpgradePanelVisible = false;
                return;
            }
            _selectedTower = &tower;
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
        ss << "Wave tiep theo sau: " << static_cast<int>(_intermissionTimer.asSeconds()) + 1;
        _timerText.setString(ss.str());
    }
    else {
        _inIntermission = false;
        _timerText.setString("");
        if (!_levelWon) startNextWave();
    }
}

void cgame::update(sf::Time deltaTime) {
    sf::Time modifiedDeltaTime = deltaTime * (_isFastForward ? _gameSpeedMultiplier : 1.0f);

    // ================== CẬP NHẬT ĐỒNG HỒ ĐẾM GIỜ ==================
    if (_levelIsActive && !_isGameOver && !_levelWon) {
        _levelTime += modifiedDeltaTime;
    }
    // ============================================================

    if (_levelWon) {
        _messageText.setString("VICTORY!");
        _timerText.setString("");
        _levelIsActive = false; // Dừng đếm giờ khi thắng
        return;
    }

    if (_isPaused || _isGameOver) {
        if (_isGameOver) _levelIsActive = false; // Dừng đếm giờ khi thua
        return;
    }

    if (_inIntermission) {
        updateInterMission(modifiedDeltaTime);
        return;
    }
    if (_waveInProgress) {
        _timeSinceLastSpawn += modifiedDeltaTime;
        if (_timeSinceLastSpawn >= _spawnInterval && _enemiesSpawnedThisWave < _enemiesPerWave) {
            spawnEnemy();
            _timeSinceLastSpawn = sf::Time::Zero;
        }
        if (_enemiesSpawnedThisWave >= _enemiesPerWave && _enemies.empty()) {
            _waveInProgress = false;
            if (_currentWave >= 2) { // Bạn có thể thay 5 bằng tổng số wave của map
                _levelWon = true;
            }
            else {
                _inIntermission = true;
                _intermissionTimer = _intermissionTime;
            }
        }
    }
    updateEnemies(modifiedDeltaTime);
    if (_isGameOver) return;
    updateTowers(modifiedDeltaTime);
    updateBullets(modifiedDeltaTime);
    handleCollisions();
    cleanupInactiveObjects();
    updateUpgradePanel();
    std::stringstream ssLives, ssMoney, ssWave;
    ssLives << _lives;
    ssMoney << _money;
    if (_currentWave > 0) ssWave << "Wave " << _currentWave;
    _livesText.setString(ssLives.str());
    _moneyText.setString(ssMoney.str());
    _waveText.setString(ssWave.str());
    if (_isGameOver) {
        _messageText.setString("GAME OVER");
    }
    else if (!_waveInProgress && !_inIntermission) {
        std::stringstream ss;
        ss << "Nhan 'N' de bat dau Wave " << (_currentWave + 1);
        _messageText.setString(ss.str());
    }
    else if (!_inIntermission) {
        _messageText.setString("");
    }
}

void cgame::render(sf::RenderWindow& window) {
    if (_map) {
        _map->render(window);
    }
    for (auto& tower : _towers) tower.render(window);
    for (auto& enemy : _enemies) enemy.render(window);
    for (auto& bullet : _bullets) bullet.render(window);
    window.draw(_uiPanel);
    if (_heartIcon.getSize().x > 0) window.draw(_livesIconSprite);
    window.draw(_livesText);
    if (_coinIcon.getSize().x > 0) window.draw(_moneyIconSprite);
    window.draw(_moneyText);
    if (_waveIcon.getSize().x > 0) window.draw(_waveIconSprite);
    window.draw(_waveText);
    float pauseButtonX = window.getSize().x - _pauseButtonSprite.getGlobalBounds().width - 20.f;
    _pauseButtonSprite.setPosition(pauseButtonX, 20.f);
    float ffButtonX = pauseButtonX - _ffButtonSprite.getGlobalBounds().width - 10.f;
    _ffButtonSprite.setPosition(ffButtonX, 20.f);
    window.draw(_ffButtonSprite);
    window.draw(_pauseButtonSprite);
    renderTowerUI(window);
    sf::Vector2f windowSize = sf::Vector2f(window.getSize());
    if (!_messageText.getString().isEmpty()) {
        sf::FloatRect textBounds = _messageText.getLocalBounds();
        _messageText.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        _messageText.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);
        window.draw(_messageText);
    }
    if (_inIntermission) {
        sf::FloatRect timerBounds = _timerText.getLocalBounds();
        _timerText.setOrigin(timerBounds.left + timerBounds.width / 2.f, timerBounds.top + timerBounds.height / 2.f);
        _timerText.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);
        window.draw(_timerText);
    }
    if (_selectingTowerToBuild) {
        updateTowerPlacementPreview(window);
        window.draw(_towerPlacementPreview);
    }
}

void cgame::renderTowerUI(sf::RenderWindow& window) {
    if (_selectedTower && _isUpgradePanelVisible) {
        float radius = _selectedTower->getCurrentLevelData().range;
        sf::CircleShape rangeCircle(radius);
        rangeCircle.setFillColor(sf::Color(100, 100, 100, 30));
        rangeCircle.setOutlineColor(sf::Color(255, 255, 255, 150));
        rangeCircle.setOutlineThickness(1.5f);
        rangeCircle.setOrigin(radius, radius);
        rangeCircle.setPosition(_selectedTower->getPosition());
        window.draw(rangeCircle);
        window.draw(_upgradeButton);
        window.draw(_upgradeText);
        if (_selectedTower->canUpgrade()) window.draw(_costText);
        window.draw(_sellButton);
        window.draw(_sellText);
        window.draw(_sellValueText);
        std::stringstream levelText;
        levelText << "Level " << _selectedTower->getLevel();
        sf::Text towerLevel;
        towerLevel.setFont(_gameFont);
        towerLevel.setString(levelText.str());
        towerLevel.setCharacterSize(14);
        sf::FloatRect levelBounds = towerLevel.getLocalBounds();
        towerLevel.setOrigin(levelBounds.width / 2.f, 0);
        towerLevel.setPosition(_selectedTower->getPosition().x, _selectedTower->getPosition().y + 30);
        window.draw(towerLevel);
    }
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
    ctower& tower = *_selectedTower;
    sf::Vector2f towerPos = tower.getPosition();
    float buttonWidth = 90.f, buttonHeight = 35.f, buttonSpacing = 10.f;
    float totalWidth = buttonWidth * 2 + buttonSpacing;
    float startX = towerPos.x - totalWidth / 2.f;
    float buttonY = towerPos.y - 120;
    _upgradeButton.setSize({ buttonWidth, buttonHeight });
    _upgradeButton.setOrigin(0, 0);
    _upgradeButton.setPosition(startX, buttonY);
    if (tower.isBusy()) {
        _upgradeButton.setFillColor(sf::Color(80, 80, 80));
        _upgradeText.setString("BUSY...");
        _costText.setString("");
    }
    else if (tower.canUpgrade()) {
        _upgradeButton.setFillColor(sf::Color(30, 144, 255));
        _upgradeText.setString("UPGRADE");
        _costText.setString(std::to_string(tower.getUpgradeCost()) + " G");
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
    saveFile << "money " << _money << std::endl;
    saveFile << "wave " << _currentWave << std::endl;

    // LƯU TRẠNG THÁI CHI TIẾT CỦA WAVE
    saveFile << "wave_in_progress " << (_waveInProgress ? 1 : 0) << std::endl;
    saveFile << "wave_enemy_type " << _currentWaveEnemyTypeIndex << std::endl;
    saveFile << "enemies_spawned " << _enemiesSpawnedThisWave << std::endl;
    saveFile << "spawn_timer " << _timeSinceLastSpawn.asSeconds() << std::endl;

    // Lưu trụ
    int validTowers = 0;
    for (const auto& tower : _towers) if (!tower.isPendingRemoval()) validTowers++;
    saveFile << "towers_count " << validTowers << std::endl;
    for (const auto& tower : _towers) {
        if (!tower.isPendingRemoval()) {
            saveFile << "tower " << tower.getTypeId() << " " << tower.getLevel() << " "
                << tower.getPosition().x << " " << tower.getPosition().y << std::endl;
        }
    }

    // LƯU TRẠNG THÁI CỦA TỪNG CON QUÁI
    saveFile << "enemies_on_map_count " << _enemies.size() << std::endl;
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
        else if (key == "money") saveFile >> _money;
        else if (key == "wave") saveFile >> _currentWave;
        // TẢI TRẠNG THÁI CHI TIẾT CỦA WAVE
        else if (key == "wave_in_progress") { int v; saveFile >> v; _waveInProgress = (v == 1); }
        else if (key == "wave_enemy_type") saveFile >> _currentWaveEnemyTypeIndex;
        else if (key == "enemies_spawned") saveFile >> _enemiesSpawnedThisWave;
        else if (key == "spawn_timer") { float s; saveFile >> s; _timeSinceLastSpawn = sf::seconds(s); }
        // Tải trụ
        else if (key == "towers_count") saveFile >> towersCount;
        else if (key == "tower") {
            std::string typeId; int level; float posX, posY;
            saveFile >> typeId >> level >> posX >> posY;
            auto it = _towerBlueprints.find(typeId);
            if (it != _towerBlueprints.end()) {
                const auto& towerLevels = it->second;
                if (level > 0 && static_cast<size_t>(level) <= towerLevels.size()) {
                    const TowerLevelData& levelData = towerLevels[level - 1];
                    _towers.emplace_back(this, typeId, levelData, cpoint(posX, posY), nextTowerId++);
                }
            }
        }
        // TẢI TỪNG CON QUÁI
        else if (key == "enemies_on_map_count") {
            saveFile >> enemiesOnMapCount;
            if (enemiesOnMapCount > 0) _enemies.reserve(enemiesOnMapCount);
        }
        else if (key == "enemy") {
            if (!_map) continue;
            int typeIndex, pathIndex; float health, posX, posY;
            saveFile >> typeIndex >> health >> posX >> posY >> pathIndex;
            if (typeIndex >= 0 && static_cast<size_t>(typeIndex) < _availableEnemyTypes.size()) {
                const EnemyType& baseType = _availableEnemyTypes.at(typeIndex);
                const auto& path = _map->getEnemyPath();
                _enemies.emplace_back(this, baseType, typeIndex, path);
                cenemy& newEnemy = _enemies.back();
                newEnemy.setHealth(health);
                newEnemy.setPosition(cpoint(posX, posY));
                newEnemy.setPathIndex(pathIndex);
            }
        }
    }

    if (_waveInProgress) {
        _enemiesPerWave = 5 + _currentWave * 2;
        _messageText.setString(""); 
    }

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
    // Công thức tính điểm: (số quái * 100) - (thời gian tính bằng giây)
    // Bạn có thể tùy chỉnh công thức này cho phù hợp
    long score = _enemiesDefeated * 100;
    long timePenalty = static_cast<long>(_levelTime.asSeconds());
    score -= timePenalty;

    // Đảm bảo điểm không bị âm
    return std::max(0L, score);
}