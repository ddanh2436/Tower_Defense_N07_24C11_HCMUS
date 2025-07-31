#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>


struct ScoreEntry {
    std::string playerName;
    long score = 0; 
    int enemiesDefeated = 0; 
    float timeTaken = 0.0f; 

    ScoreEntry() = default;

    ScoreEntry(const std::string& name, long s, int enemies, float time)
        : playerName(name), score(s), enemiesDefeated(enemies), timeTaken(time) {
    }
};

class Leaderboard {
public:
    Leaderboard();
    void addScore(const std::string& playerName, long score, int enemies, float time);
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;
    const std::vector<ScoreEntry>& getScores() const;
    void draw(sf::RenderWindow& window); 

private:
    std::vector<ScoreEntry> _scores;
    sf::Font _font;
    const size_t _maxEntries = 10; 
    void sortScores(); 
};

#endif // LEADERBOARD_H