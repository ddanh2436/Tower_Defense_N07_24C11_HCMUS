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

    while (file >> score >> enemies >> time) { 
        std::getline(file, name);
        if (!name.empty() && name[0] == ' ') {
            name.erase(0, 1);
        }

        _scores.push_back({ name, score, enemies, time });
    }

    sortScores();
}

void Leaderboard::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        std::cerr << "FATAL ERROR: Could not open leaderboard file for writing at: " << filename << std::endl;
        std::cerr << "POSSIBLE FIX: Please create a folder named 'data' in your project's root directory." << std::endl;
        std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl;
        return;
    }
    for (const auto& entry : _scores) {
        file << entry.score << " " << entry.enemiesDefeated << " " << entry.timeTaken << " " << entry.playerName << std::endl;
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
    sf::Text title("High Scores", _font, 72);
    title.setFillColor(sf::Color::Yellow);
    title.setOrigin(title.getLocalBounds().width / 2.f, title.getLocalBounds().height / 2.f);
    title.setPosition(window.getSize().x / 2.f, 80);
    title.setOutlineColor(sf::Color::Black);
    title.setOutlineThickness(2.f);
    window.draw(title);

    sf::Text rankHeader("Rank", _font, 48);
    sf::Text nameHeader("Name", _font, 48);
    sf::Text scoreHeader("Score", _font, 48);
    sf::Text killsHeader("Kills", _font, 48);
    sf::Text timeHeader("Time", _font, 48);

    int rankHeaderWidth = static_cast<int>(rankHeader.getLocalBounds().width);
	int nameHeaderWidth = static_cast<int>(nameHeader.getLocalBounds().width);
	int scoreHeaderWidth = static_cast<int>(scoreHeader.getLocalBounds().width);
	int killsHeaderWidth = static_cast<int>(killsHeader.getLocalBounds().width);
    int timeHeaderWidth = static_cast<int>(timeHeader.getLocalBounds().width);


    int AlignScoreHeaderMid = static_cast<int>(window.getSize().x / 2.f - rankHeaderWidth / 2.f);


    rankHeader.setPosition(AlignScoreHeaderMid - 500.f, 160.f);
    nameHeader.setPosition(AlignScoreHeaderMid - 350.f, 160.f);
    scoreHeader.setPosition(static_cast<float>(AlignScoreHeaderMid), 160.f);
    killsHeader.setPosition(AlignScoreHeaderMid + 350.f, 160.f);
    timeHeader.setPosition(AlignScoreHeaderMid + 500.f, 160.f);


    rankHeader.setFillColor(sf::Color::Cyan);
    rankHeader.setOutlineColor(sf::Color::Black);
    rankHeader.setOutlineThickness(2.f);
    nameHeader.setFillColor(sf::Color::Cyan);
    nameHeader.setOutlineColor(sf::Color::Black);
    nameHeader.setOutlineThickness(2.f);
    scoreHeader.setFillColor(sf::Color::Cyan);
    scoreHeader.setOutlineColor(sf::Color::Black);
    scoreHeader.setOutlineThickness(2.f);
    killsHeader.setFillColor(sf::Color::Cyan);
    killsHeader.setOutlineColor(sf::Color::Black);
    killsHeader.setOutlineThickness(2.f);
    timeHeader.setFillColor(sf::Color::Cyan);
    timeHeader.setOutlineColor(sf::Color::Black);
    timeHeader.setOutlineThickness(2.f);
    window.draw(rankHeader);
    window.draw(nameHeader);
    window.draw(scoreHeader);
    window.draw(killsHeader);
    window.draw(timeHeader);

    float yPos = 245.f;
    int rank = 1;
    for (const auto& entry : _scores) {
        sf::Text rankText(std::to_string(rank), _font, 36);
        sf::Text nameText(entry.playerName, _font, 36); 
        sf::Text scoreText(std::to_string(entry.score), _font, 36);  
        sf::Text killsText(std::to_string(entry.enemiesDefeated), _font, 36);
        sf::Text timeText(std::to_string(static_cast<int>(entry.timeTaken)) + "s", _font, 36);

        rankText.setPosition(static_cast<float>(AlignScoreHeaderMid - 500 + rankHeaderWidth / 2) - rankText.getLocalBounds().width / 2.f, yPos);

        nameText.setPosition(static_cast<float>(AlignScoreHeaderMid - 350), yPos);
        scoreText.setPosition(static_cast<float>(AlignScoreHeaderMid), yPos);
        killsText.setPosition(AlignScoreHeaderMid + 350 + static_cast<float>(killsHeaderWidth) / 2.f - killsText.getLocalBounds().width / 2.f, yPos);
        timeText.setPosition(static_cast<float>(AlignScoreHeaderMid + 500), yPos);
   
        if (rank == 1) {
            rankText.setFillColor(sf::Color(255, 40, 1));
            nameText.setFillColor(sf::Color(255, 40, 1));
            scoreText.setFillColor(sf::Color(255, 40, 1));
            killsText.setFillColor(sf::Color(255, 40, 1));
            timeText.setFillColor(sf::Color(255, 40, 1));
		}
		if (rank == 2) {
            rankText.setFillColor(sf::Color(206, 255, 4));
            nameText.setFillColor(sf::Color(206, 255, 4));
            scoreText.setFillColor(sf::Color(206, 255, 4));
            killsText.setFillColor(sf::Color(206, 255, 4));
            timeText.setFillColor(sf::Color(206, 255, 4));
		}
        if (rank == 3) {
            rankText.setFillColor(sf::Color(254, 229, 199));
            nameText.setFillColor(sf::Color(254, 229, 199));
            scoreText.setFillColor(sf::Color(254, 229, 199));
            killsText.setFillColor(sf::Color(254, 229, 199));
            timeText.setFillColor(sf::Color(254, 229, 199));
        }

        window.draw(rankText);
        window.draw(nameText);
        window.draw(scoreText);
        window.draw(killsText);
        window.draw(timeText);

        yPos += 50.f;
        rank++;
    }

    sf::Text returnHint("Press Esc to go back", _font, 20);
    returnHint.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect hintRect = returnHint.getLocalBounds();
    returnHint.setOrigin(hintRect.left + hintRect.width / 2.f, hintRect.top + hintRect.height / 2.f);
    returnHint.setPosition(window.getSize().x / 2.f, window.getSize().y - 50.f);
    window.draw(returnHint);
}