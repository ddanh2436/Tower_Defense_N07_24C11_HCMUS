#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

// Struct để lưu một mục trong bảng xếp hạng
struct ScoreEntry {
    std::string playerName;
    long score = 0; // Initialize to 0
    int enemiesDefeated = 0; // Initialize to 0
    float timeTaken = 0.0f; // Initialize to 0.0f

    // Default constructor
    ScoreEntry() = default;

    // Parameterized constructor for convenience
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
    void draw(sf::RenderWindow& window); // Hàm để vẽ bảng xếp hạng

private:
    std::vector<ScoreEntry> _scores;
    sf::Font _font;
    const size_t _maxEntries = 10; // Giới hạn 10 người đứng đầu

    void sortScores(); // Sắp xếp điểm từ cao đến thấp
};

#endif // LEADERBOARD_H