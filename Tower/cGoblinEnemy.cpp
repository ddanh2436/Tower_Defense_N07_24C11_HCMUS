#include "cGoblinEnemy.h"

cGoblinEnemy::cGoblinEnemy(cgame* gameInstance, const EnemyType& type, int typeIndex, const std::vector<cpoint>& path)
    : cbasicenemy(gameInstance, type, typeIndex, path)
{
    // Lớp này không có hành vi đặc biệt, chỉ cần gọi hàm khởi tạo của lớp cha.
}