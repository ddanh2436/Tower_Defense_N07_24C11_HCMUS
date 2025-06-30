#include "cgame.h"
#include "SoundManager.h"
#include <iostream>
#include <sstream>
#include <random>

cgame::cgame() : _rng(std::random_device{}()) {
    loadFont();
    setupUI();
    setupEnemyTypes();
    setupTowerTypes();
    _isPaused = false;
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

        /*--- Animation chính (Xây/Tấn công) ---*/
        /*texturePath*/ "assets/4.png",
        /*startFrame*/ 2, /*numFrames*/ 2, /*speed*/ 0.5f,

        /*--- Animation IDLE ---*/
         "assets/4_idle.png",
         0,
         6,
         2.0f,

         /*--- Chỉ số chiến đấu ---*/
        /*range*/ 150.f, /*fireRate*/ 1.0f, /*damage*/ 25, /*bulletSpeed*/ 200.f
        });

    // ----- CẤP 2 -----
    archerLevels.push_back(TowerLevelData{
        /*level*/ 2, /*cost*/ 75,
        /*frameSize*/ archerFrameSize, /*frameOffsetY*/ archerFrameOffsetY,

        /*--- Animation chính (Xây/Tấn công) ---*/
        /*texturePath*/ "assets/5.png",
        /*startFrame*/ 2, /*numFrames*/ 2, /*speed*/ 0.4f,

        /*--- Animation IDLE ---*/
        /*idle_texturePath*/ "assets/5_idle.png",
        /*idle_startFrame*/  0,
        /*idle_numFrames*/   6,
        /*idle_speed*/       1.8f,

        /*--- Chỉ số chiến đấu ---*/
        /*range*/ 175.f, /*fireRate*/ 0.8f, /*damage*/ 40, /*bulletSpeed*/ 220.f
        });

    archerLevels.push_back(TowerLevelData{
        /*level*/ 3, /*cost*/ 100,
        /*frameSize*/ archerFrameSize, /*frameOffsetY*/ archerFrameOffsetY,

        /*--- Animation chính (Xây/Tấn công) ---*/
        /*texturePath*/ "assets/6.png",
        /*startFrame*/ 2, /*numFrames*/ 2, /*speed*/ 0.4f,

        /*--- Animation IDLE ---*/
        /*idle_texturePath*/ "assets/6_idle.png",
        /*idle_startFrame*/  0,
        /*idle_numFrames*/   6,
        /*idle_speed*/       1.8f,

        /*--- Chỉ số chiến đấu ---*/
        /*range*/ 200.f, /*fireRate*/ 0.9f, /*damage*/ 50, /*bulletSpeed*/ 230.f
        });

    archerLevels.push_back(TowerLevelData{
        /*level*/ 4, /*cost*/ 150,
        /*frameSize*/ archerFrameSize, /*frameOffsetY*/ archerFrameOffsetY,

        /*--- Animation chính (Xây/Tấn công) ---*/
        /*texturePath*/ "assets/7.png",
        /*startFrame*/ 2, /*numFrames*/ 2, /*speed*/ 0.4f,

        /*--- Animation IDLE ---*/
        /*idle_texturePath*/ "assets/7_idle.png",
        /*idle_startFrame*/  0,
        /*idle_numFrames*/   6,
        /*idle_speed*/       1.8f,

        /*--- Chỉ số chiến đấu ---*/
        /*range*/ 230.f, /*fireRate*/ 1.2f, /*damage*/ 65, /*bulletSpeed*/ 235.f
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

    // Loại 1: (Goblin)
    EnemyType goblin;
    goblin.health = 120;
    goblin.speed = 20.f;
    goblin.scale = 2.0f;
    goblin.moneyValue = 30;
    goblin.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U_Walk.png";
    goblin.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D_Walk.png";
    goblin.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S_Walk.png";
    goblin.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U_Death.png";
    goblin.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D_Death.png";
    goblin.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S_Death.png";
    _availableEnemyTypes.push_back(goblin);

    // Loại 2: WOLF
    EnemyType wolf = goblin;
    wolf.health = 60;
    wolf.speed = 35.f;
    wolf.scale = 1.2f;
    wolf.moneyValue = 20;
    wolf.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U1_Walk.png";
    wolf.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D1_Walk.png";
    wolf.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S1_Walk.png";
    wolf.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U1_Death.png";
    wolf.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D1_Death.png";
    wolf.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S1_Death.png";
    _availableEnemyTypes.push_back(wolf);

    // Loại 3: Bee
    EnemyType bee = wolf;
    bee.health = 250;
    bee.speed = 15.f;
    bee.scale = 2.0f;
    bee.moneyValue = 20;
    bee.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U2_Walk.png";
    bee.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D2_Walk.png";
    bee.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S2_Walk.png";
    bee.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U2_Death.png";
    bee.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D2_Death.png";
    bee.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S2_Death.png";
    _availableEnemyTypes.push_back(bee);

    //Loai 4: Slime
    EnemyType slime = bee;
    slime.health = 150;
    slime.speed = 20.f;
    slime.scale = 2.0f;
    slime.moneyValue = 20;
    slime.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U3_Walk.png";
    slime.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D3_Walk.png";
    slime.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S3_Walk.png";
    slime.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U3_Death.png";
    slime.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D3_Death.png";
    slime.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S3_Death.png";
    _availableEnemyTypes.push_back(slime);

    EnemyType slime1 = slime;
    slime1.health = 150;
    slime1.speed = 20.f;
    slime1.scale = 2.0f;
    slime1.moneyValue = 20;
    slime1.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U3_Special.png";
    slime1.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D3_Special.png";
    slime1.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S3_Special.png";
    slime1.texturePaths[EnemyState::DYING][MovementDirection::UP] = "assets/U3_Death2.png";
    slime1.texturePaths[EnemyState::DYING][MovementDirection::DOWN] = "assets/D3_Death2.png";
    slime1.texturePaths[EnemyState::DYING][MovementDirection::SIDE] = "assets/S3_Death2.png";
    _availableEnemyTypes.push_back(slime1);

    // Knight
    EnemyType knight = goblin;
    knight.health = 250;
    knight.speed = 20.f;
    knight.scale = 2.0f;
    knight.moneyValue = 20;
    knight.texturePaths[EnemyState::WALKING][MovementDirection::UP] = "assets/U4_Walk.png";
    knight.texturePaths[EnemyState::WALKING][MovementDirection::DOWN] = "assets/D4_Walk.png";
    knight.texturePaths[EnemyState::WALKING][MovementDirection::SIDE] = "assets/S4_Walk.png";
    /*knight.frameSize = { 96, 96 };*/
    _availableEnemyTypes.push_back(knight);

}

void cgame::resetGameStats() {
    _isPaused = false;
    _lives = 1;
    _money = 1000;
    _currentWave = 0;
    _enemiesPerWave = 5;
    _spawnInterval = sf::seconds(2.0f); // Giảm thời gian spawn quái
    _timeSinceLastSpawn = sf::Time::Zero;
    _enemiesSpawnedThisWave = 0;
    _isGameOver = false;
    _inIntermission = false;
    _intermissionTime = sf::seconds(5.f);
    _intermissionTimer = sf::Time::Zero;
    _levelWon = false;
    _waveInProgress = false;
    _currentWaveEnemyTypeIndex = -1;
    _isFastForward = false; // <<< THÊM MỚI
    _gameSpeedMultiplier = 2.0f; // <<< THÊM MỚI

    _enemies.clear();
    _towers.clear();
    _bullets.clear();

    _selectingTowerToBuild = false;
}

void cgame::startNextWave() {
    // Thêm _levelWon vào điều kiện bảo vệ
    if (_waveInProgress || _isGameOver || _levelWon) return;

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

    std::cout << "Wave " << _currentWave << " bat dau!" << std::endl;
}
void cgame::spawnEnemy() {
    if (_enemiesSpawnedThisWave < _enemiesPerWave && _currentWaveEnemyTypeIndex != -1) {
        const auto& path = _map.getEnemyPath();
        if (!path.empty()) {
            const EnemyType& currentType = _availableEnemyTypes.at(_currentWaveEnemyTypeIndex);
            int finalHealth = static_cast<int>(currentType.health * std::pow(1.2, _currentWave - 1));
            if (finalHealth < currentType.health) finalHealth = currentType.health;
            float finalSpeed = currentType.speed * (1.f + (_currentWave - 1) * 0.05f);
            const float maxSpeed = 50.f;
            if (finalSpeed > maxSpeed) finalSpeed = maxSpeed;
            int finalMoneyValue = currentType.moneyValue + (_currentWave * 2);

            _enemies.emplace_back(
                this,
                currentType.speed,
                finalHealth,
                path,
                currentType.scale,
                currentType.moneyValue,
                currentType.frameSize,
                currentType.texturePaths
            );
            _enemiesSpawnedThisWave++;
        }
    }
}

void cgame::handleCollisions() {
    for (auto& bullet : _bullets) {
        if (!bullet.canCollide()) continue;

        for (auto& enemy : _enemies) {
            if (enemy.isActive() && enemy.isAlive()) {
                if (bullet.getGlobalBounds().intersects(enemy.getGlobalBounds())) {
                    enemy.takeDamage(bullet.getDamage());
                    bullet.setActive(false);

                    if (!enemy.isAlive()) {
                        _money += enemy.getMoneyValue();
                        SoundManager::playSoundEffect("enemy_explode");
                    }
                    break;
                }
            }
        }
    }
}

bool cgame::isGameOver() const {
    return _isGameOver;
}

void cgame::loadFont() {
    if (!_gameFont.loadFromFile(FONT_PATH)) {
        std::cerr << "Loi: Khong the tai font game: " << FONT_PATH << std::endl;
    }
}

void cgame::setupUI() {
    // Create UI panel background
    _uiPanel.setSize(sf::Vector2f(250, 120));
    _uiPanel.setPosition(10, 10);
    _uiPanel.setFillColor(sf::Color(30, 30, 40, 200));
    _uiPanel.setOutlineThickness(2.f);
    _uiPanel.setOutlineColor(sf::Color(60, 60, 80, 255));

    // Common text settings
    const int fontSize = 22;
    const float padding = 15.f;
    const float iconSize = 24.f;
    const float yStart = 20.f;
    const float spacing = 30.f;

    // Load UI icons
    if (!_heartIcon.loadFromFile("assets/heart_icon.png") ||
        !_coinIcon.loadFromFile("assets/coin_icon.png") ||
        !_waveIcon.loadFromFile("assets/wave_icon.png")) {
        std::cerr << "Warning: Failed to load UI icons, using text labels instead" << std::endl;
    }

    // Setup heart/lives UI
    _livesIconSprite.setTexture(_heartIcon);
    _livesIconSprite.setScale(iconSize / _heartIcon.getSize().x, iconSize / _heartIcon.getSize().y);
    _livesIconSprite.setPosition(padding + 10, yStart);

    _livesText.setFont(_gameFont);
    _livesText.setCharacterSize(fontSize);
    _livesText.setFillColor(sf::Color::White);
    _livesText.setOutlineColor(sf::Color(100, 0, 0));
    _livesText.setOutlineThickness(1.f);
    _livesText.setPosition(padding + iconSize + 20, yStart - 2);

    // Setup coin/money UI
    _moneyIconSprite.setTexture(_coinIcon);
    _moneyIconSprite.setScale(iconSize / _coinIcon.getSize().x, iconSize / _coinIcon.getSize().y);
    _moneyIconSprite.setPosition(padding + 10, yStart + spacing);

    _moneyText.setFont(_gameFont);
    _moneyText.setCharacterSize(fontSize);
    _moneyText.setFillColor(sf::Color(255, 215, 0)); // Gold color
    _moneyText.setOutlineColor(sf::Color(100, 80, 0));
    _moneyText.setOutlineThickness(1.f);
    _moneyText.setPosition(padding + iconSize + 20, yStart + spacing - 2);

    // Setup wave UI
    _waveIconSprite.setTexture(_waveIcon);
    _waveIconSprite.setScale(iconSize / _waveIcon.getSize().x, iconSize / _waveIcon.getSize().y);
    _waveIconSprite.setPosition(padding + 10, yStart + spacing * 2);

    _waveText.setFont(_gameFont);
    _waveText.setCharacterSize(fontSize);
    _waveText.setFillColor(sf::Color(120, 200, 255)); // Light blue
    _waveText.setOutlineColor(sf::Color(0, 50, 100));
    _waveText.setOutlineThickness(1.f);
    _waveText.setPosition(padding + iconSize + 20, yStart + spacing * 2 - 2);

    // Set up other UI elements
    _messageText.setFont(_gameFont);
    _messageText.setCharacterSize(50);
    _messageText.setFillColor(sf::Color::Yellow);
    _messageText.setOutlineColor(sf::Color(100, 100, 0));
    _messageText.setOutlineThickness(1.5f);

    _timerText.setFont(_gameFont);
    _timerText.setCharacterSize(50);
    _timerText.setFillColor(sf::Color::Yellow);
    _timerText.setOutlineColor(sf::Color(100, 100, 0));
    _timerText.setOutlineThickness(1.5f);

    if (!_pauseButtonTexture.loadFromFile("assets/pause_icon.png")) {
        std::cerr << "Loi: Khong the tai pause_icon.png" << std::endl;
    }
    _pauseButtonSprite.setTexture(_pauseButtonTexture);
    float pauseIconSize = 40.f;
    _pauseButtonSprite.setScale(
        pauseIconSize / _pauseButtonTexture.getSize().x,
        pauseIconSize / _pauseButtonTexture.getSize().y
    );

    // <<< THÊM MỚI: Thiết lập cho nút tăng tốc
    if (!_ffButtonTexture.loadFromFile("assets/ff_icon.png")) { // Nhớ tạo file assets/ff_icon.png
        std::cerr << "Loi: Khong the tai ff_icon.png" << std::endl;
    }
    _ffButtonSprite.setTexture(_ffButtonTexture);
    _ffButtonSprite.setScale(
        pauseIconSize / _ffButtonTexture.getSize().x,
        pauseIconSize / _ffButtonTexture.getSize().y
    );
}


void cgame::resetGame() {

    std::cout << "Reseting game state..." << std::endl;
    resetGameStats();
}

void cgame::updateEnemies(sf::Time deltaTime) {
    for (auto& enemy : _enemies) {
        if (enemy.isActive()) {
            enemy.update(deltaTime);
            if (enemy.hasReachedEnd()) {
                _lives--;
                enemy.setActive(false);
                std::cout << "Mot enemy da den dich! Mang con lai: " << _lives << std::endl;
                if (_lives <= 0) {
                    _isGameOver = true;
                    _messageText.setString("GAME OVER!");
                    std::cout << "GAME OVER!" << std::endl;
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
    _enemies.erase(std::remove_if(_enemies.begin(), _enemies.end(),
        [](const cenemy& e) { return e.isReadyForRemoval(); }),
        _enemies.end());

    _bullets.erase(std::remove_if(_bullets.begin(), _bullets.end(),
        [](const cbullet& b) { return !b.isActive(); }),
        _bullets.end());
}

void cgame::updateTowerPlacementPreview(sf::RenderWindow& window) {
    if (!_selectingTowerToBuild) return;

    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

    sf::Vector2i gridCoords = _map.getGridCoordinates(mouseWorldPos);
    cpoint tileCenterPixelPos = _map.getPixelPosition(gridCoords.y, gridCoords.x, PositionContext::TowerPlacement);

    _towerPlacementPreview.setPosition(tileCenterPixelPos.toVector2f());

    // Kiểm tra xem có thể xây dựng không (bao gồm cả việc check tile có bị chiếm chưa)
    bool isOccupied = false;
    for (const auto& tower : _towers) {
        if (_map.getGridCoordinates(tower.getPosition()) == gridCoords) {
            isOccupied = true;
            break;
        }
    }

    if (_map.isBuildable(gridCoords.y, gridCoords.x) && !isOccupied) {
        _canPlaceTower = true;
        _towerPlacementPreview.setColor(sf::Color(255, 255, 255, 150)); // Màu hợp lệ
    }
    else {
        _canPlaceTower = false;
        _towerPlacementPreview.setColor(sf::Color(255, 0, 0, 150)); // Màu không hợp lệ
    }
}


void cgame::handleInput(const sf::Event& event, sf::RenderWindow& window) {
    if (_isGameOver) return;

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::N) {
            if (!_waveInProgress && !_inIntermission) {
                startNextWave();
            }
        }
        if (event.key.code == sf::Keyboard::Num1) {
            const auto& blueprint = _towerBlueprints.at("ArcherTower");
            if (_money >= blueprint[0].cost) {
                _selectingTowerToBuild = true;
                _selectedTowerType = "ArcherTower"; // Lưu loại trụ đang chọn

                // Thiết lập preview từ blueprint cấp 1
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

                std::cout << "Dang chon vi tri dat thap. Click de dat." << std::endl;
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

            // Ưu tiên kiểm tra nút pause trước
            if (_pauseButtonSprite.getGlobalBounds().contains(mouseWorldPos)) {
                setPaused(true);
                SoundManager::playSoundEffect("menu_click");
                return;
            }

            // <<< THÊM MỚI: Xử lý click nút tăng tốc
            if (_ffButtonSprite.getGlobalBounds().contains(mouseWorldPos)) {
                _isFastForward = !_isFastForward;
                if (_isFastForward) {
                    _ffButtonSprite.setColor(sf::Color(100, 255, 100)); // Màu xanh khi bật
                }
                else {
                    _ffButtonSprite.setColor(sf::Color::White); // Màu trắng khi tắt
                }
                return;
            }

            if (_isUpgradePanelVisible && _upgradeButton.getGlobalBounds().contains(mouseWorldPos)) {
                handleUpgrade();
            }
            else if (_selectingTowerToBuild) {
                // ---- START: CẢI TIẾN LOGIC ĐẶT TRỤ ----
                sf::Vector2i gridCoords = _map.getGridCoordinates(mouseWorldPos);

                // KIỂM TRA 1: Tile có thể xây được không? (không phải đường đi)
                if (_map.isBuildable(gridCoords.y, gridCoords.x)) {

                    // KIỂM TRA 2: Tile đã có trụ nào chiếm chưa?
                    bool tileIsOccupied = false;
                    for (const auto& existingTower : _towers) {
                        sf::Vector2i existingGridCoords = _map.getGridCoordinates(existingTower.getPosition());
                        if (existingGridCoords == gridCoords) {
                            tileIsOccupied = true;
                            std::cout << "A tower already exists on this tile." << std::endl;
                            break;
                        }
                    }

                    // Nếu tile trống và có thể xây, tiến hành xây
                    if (!tileIsOccupied) {
                        const auto& blueprint = _towerBlueprints.at(_selectedTowerType);
                        if (_money >= blueprint[0].cost) {
                            cpoint towerPosition = _map.getPixelPosition(gridCoords.y, gridCoords.x, PositionContext::TowerPlacement);
                            _towers.emplace_back(this, _selectedTowerType, blueprint[0], towerPosition);
                            _money -= blueprint[0].cost;
                            _selectingTowerToBuild = false;
                        }
                        else {
                            std::cout << "Not enough money to build tower!" << std::endl;
                            _selectingTowerToBuild = false;
                        }
                    }
                }
                else {
                    std::cout << "Cannot place tower on path or obstacle." << std::endl;
                }
                // ---- END: CẢI TIẾN LOGIC ĐẶT TRỤ ----
            }
            else {
                handleTowerSelection(mouseWorldPos);
            }
        }
    }
}


void cgame::handleTowerSelection(const sf::Vector2f& mousePos) {
    _selectedTower = nullptr;
    _isUpgradePanelVisible = false;

    for (auto& tower : _towers) {
        if (tower.getGlobalBounds().contains(mousePos)) {
            _selectedTower = &tower;
            _isUpgradePanelVisible = true;
            std::cout << "Da chon thap cap " << tower.getLevel() << std::endl;

            sf::Vector2f towerPos = tower.getPosition();

            _upgradeButton.setSize({ 110, 40 });
            _upgradeButton.setOrigin(55, 20);
            _upgradeButton.setPosition(towerPos.x, towerPos.y - 90);

            if (tower.canUpgrade()) {

                _upgradeButton.setFillColor(sf::Color(30, 144, 255));
                _upgradeButton.setOutlineColor(sf::Color(0, 191, 255));
                _upgradeButton.setOutlineThickness(2.f);

                std::stringstream ss;
                ss << "UPGRADE";
                _upgradeText.setString(ss.str());

                _costText.setFont(_gameFont);
                _costText.setString(std::to_string(tower.getUpgradeCost()) + " G");
                _costText.setCharacterSize(14);
                _costText.setFillColor(sf::Color::Yellow);

                sf::FloatRect costBounds = _costText.getLocalBounds();
                _costText.setOrigin(costBounds.width / 2.f, 0);
                _costText.setPosition(_upgradeButton.getPosition().x,
                    _upgradeButton.getPosition().y + 25);
            }
            else {
                _upgradeButton.setFillColor(sf::Color(80, 80, 80));
                _upgradeButton.setOutlineColor(sf::Color(120, 120, 120));
                _upgradeButton.setOutlineThickness(1.f);
                _upgradeText.setString("MAX LEVEL");
                _costText.setString("");
            }

            _upgradeText.setFont(_gameFont);
            _upgradeText.setCharacterSize(16);
            _upgradeText.setStyle(sf::Text::Bold);
            _upgradeText.setFillColor(sf::Color::White);

            sf::FloatRect textBounds = _upgradeText.getLocalBounds();
            _upgradeText.setOrigin(textBounds.width / 2.f, textBounds.height / 2.f);
            _upgradeText.setPosition(_upgradeButton.getPosition());

            return;
        }
    }
}

void cgame::handleUpgrade() {
    if (_selectedTower && _selectedTower->canUpgrade()) {
        int cost = _selectedTower->getUpgradeCost();
        if (_money >= cost) {
            _money -= cost;
            _selectedTower->upgrade();
            _isUpgradePanelVisible = false;
            _selectedTower = nullptr;
        }
        else {
            std::cout << "Khong du tien de nang cap!" << std::endl;
        }
    }
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

        if (!_levelWon) {
            startNextWave();
        }
    }
}

// <<< SỬA ĐỔI: Toàn bộ hàm update được sửa đổi để áp dụng tốc độ game
void cgame::update(sf::Time deltaTime) {
    sf::Time modifiedDeltaTime = deltaTime;
    if (_isFastForward) {
        modifiedDeltaTime *= _gameSpeedMultiplier;
    }

    if (_levelWon) {
        _messageText.setString("VICTORY!");
        _timerText.setString("");
        return;
    }

    if (_isPaused || _isGameOver) return;

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

            if (_currentWave >= 5) {
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

    // Khối cập nhật UI tập trung
    std::stringstream ssLives, ssMoney, ssWave;
    ssLives << _lives;
    ssMoney << _money;
    if (_currentWave > 0) ssWave << "Wave " << _currentWave;
    _livesText.setString(ssLives.str());
    _moneyText.setString(ssMoney.str());
    _waveText.setString(ssWave.str());

    // Logic hiển thị thông báo chính
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
    _map.render(window);

    for (auto& tower : _towers) {
        tower.render(window);
    }
    for (auto& enemy : _enemies) {
        enemy.render(window);
    }
    for (auto& bullet : _bullets) {
        bullet.render(window);
    }

    // Draw UI panel and elements
    window.draw(_uiPanel);

    // Draw lives UI with icon
    if (_heartIcon.getSize().x > 0) {
        window.draw(_livesIconSprite);
    }
    window.draw(_livesText);

    // Draw money UI with icon
    if (_coinIcon.getSize().x > 0) {
        window.draw(_moneyIconSprite);
    }
    window.draw(_moneyText);

    // Draw wave UI with icon
    if (_waveIcon.getSize().x > 0) {
        window.draw(_waveIconSprite);
    }
    window.draw(_waveText);

    // <<< SỬA ĐỔI: Định vị và vẽ cả hai nút
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
    if (_selectedTower) {
        float radius = _selectedTower->getCurrentLevelData().range;

        sf::CircleShape rangeCircle(radius);
        rangeCircle.setFillColor(sf::Color(100, 100, 100, 30));
        rangeCircle.setOutlineColor(sf::Color(255, 255, 255, 150));
        rangeCircle.setOutlineThickness(1.5f);
        rangeCircle.setOrigin(radius, radius);
        rangeCircle.setPosition(_selectedTower->getPosition());
        window.draw(rangeCircle);

        // Draw tower UI elements
        if (_isUpgradePanelVisible) {
            sf::RectangleShape buttonShadow(_upgradeButton.getSize() + sf::Vector2f(4, 4));
            buttonShadow.setOrigin(_upgradeButton.getOrigin() + sf::Vector2f(2, 2));
            buttonShadow.setPosition(_upgradeButton.getPosition() + sf::Vector2f(3, 3));
            buttonShadow.setFillColor(sf::Color(0, 0, 0, 100));
            window.draw(buttonShadow);

            window.draw(_upgradeButton);

            sf::RectangleShape highlight({ _upgradeButton.getSize().x - 4, 2 });
            highlight.setOrigin(_upgradeButton.getOrigin().x - 2, _upgradeButton.getOrigin().y - 19);
            highlight.setPosition(_upgradeButton.getPosition());
            highlight.setFillColor(sf::Color(255, 255, 255, 100));
            window.draw(highlight);

            window.draw(_upgradeText);

            if (_selectedTower->canUpgrade()) {
                window.draw(_costText);
            }

            std::stringstream levelText;
            levelText << "Level " << _selectedTower->getLevel();
            sf::Text towerLevel;
            towerLevel.setFont(_gameFont);
            towerLevel.setString(levelText.str());
            towerLevel.setCharacterSize(14);
            towerLevel.setFillColor(sf::Color::White);
            towerLevel.setOutlineColor(sf::Color::Black);
            towerLevel.setOutlineThickness(1.f);

            sf::FloatRect levelBounds = towerLevel.getLocalBounds();
            towerLevel.setOrigin(levelBounds.width / 2.f, 0);
            towerLevel.setPosition(_selectedTower->getPosition().x,
                _selectedTower->getPosition().y + 30);
            window.draw(towerLevel);
        }
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
            std::cerr << "Khong the tai texture trong getTexture: " << texturePath << std::endl;
            return _textureManager[texturePath];
        }
    }
}

void cgame::setPaused(bool paused) {
    _isPaused = paused;
}

bool cgame::isPaused() const {
    return _isPaused;
}