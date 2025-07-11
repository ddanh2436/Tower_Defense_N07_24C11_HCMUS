#include "Leaderboard.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>

Leaderboard::Leaderboard() {
    if (!_font.loadFromFile("assets/pixel_font.ttf")) {
        std::cerr << "Error: Could not load font for leaderboard!" << std::endl;
    }
    loadFromFile("data/leaderboards.txt");
}

void Leaderboard::addScore(const std::string& playerName, long score, int enemies, float time) {
    _scores.push_back({ playerName, score, enemies, time });
    sortScores();
    if (_scores.size() > _maxEntries) {
        _scores.resize(_maxEntries);
    }
    saveToFile("data/leaderboards.txt");
}

void Leaderboard::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Info: No leaderboard file found at '" << filename << "'. A new one will be created." << std::endl;
        return;
    }
    _scores.clear();
    std::string name;
    long score;
    int enemies;
    float time;
    while (file >> name >> score >> enemies >> time) {
        _scores.push_back({ name, score, enemies, time });
    }
    sortScores();
}

void Leaderboard::saveToFile(const std::string& filename) const {
    // Quan trọng: Đảm bảo bạn đã tạo thư mục "data" trong thư mục dự án của mình!
    std::ofstream file(filename);
    if (!file.is_open()) {
        // Thêm cảnh báo chi tiết hơn để dễ dàng gỡ lỗi
        std::cerr << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        std::cerr << "FATAL ERROR: Could not open leaderboard file for writing at: " << filename << std::endl;
        std::cerr << "POSSIBLE FIX: Please create a folder named 'data' in your project's root directory." << std::endl;
        std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl;
        return;
    }
    for (const auto& entry : _scores) {
        file << entry.playerName << " " << entry.score << " " << entry.enemiesDefeated << " " << entry.timeTaken << std::endl;
    }
    std::cout << "Leaderboard data successfully saved to " << filename << std::endl;
}

const std::vector<ScoreEntry>& Leaderboard::getScores() const {
    return _scores;
}

void Leaderboard::sortScores() {
    std::sort(_scores.begin(), _scores.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;
        });
}

void Leaderboard::draw(sf::RenderWindow& window) {
    sf::Text title("High Scores", _font, 50);
    title.setFillColor(sf::Color::Yellow);
    title.setOrigin(title.getLocalBounds().width / 2.f, title.getLocalBounds().height / 2.f);
    title.setPosition(window.getSize().x / 2.f, 80);
    window.draw(title);

    sf::Text header("Rank  Name       Score   Kills   Time", _font, 24);
    header.setFillColor(sf::Color::Cyan);
    header.setPosition(150, 160);
    window.draw(header);

    float yPos = 210.f;
    int rank = 1;
    for (const auto& entry : _scores) {
        std::stringstream ss;
        ss << std::left
            << std::setw(6) << std::to_string(rank) + "."
            << std::setw(11) << entry.playerName
            << std::setw(8) << entry.score
            << std::setw(8) << entry.enemiesDefeated
            << std::fixed << std::setprecision(2) << entry.timeTaken << "s";

        sf::Text text(ss.str(), _font, 22);
        text.setPosition(150, yPos);
        window.draw(text);
        yPos += 35.f;
        rank++;
    }

    sf::Text returnHint("Press Esc to go back", _font, 20);
    returnHint.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect hintRect = returnHint.getLocalBounds();
    returnHint.setOrigin(hintRect.left + hintRect.width / 2.f, hintRect.top + hintRect.height / 2.f);
    returnHint.setPosition(window.getSize().x / 2.f, window.getSize().y - 50.f);
    window.draw(returnHint);
}