//
// Created by yaorz2 on 25-12-1.
//

#ifndef PLAYER_H
#define PLAYER_H


#include <string>
#include <vector>

// 技术维度 - 扣球属性
struct SpikeAttr {
    int power;          // 力量（0-100）
    int accuracy;       // 准确性（0-100）
    int variation;      // 变化（0-100）
    int netBreak;       // 突破篮网能力（0-100）
};

// 技术维度 - 拦网属性
struct BlockAttr {
    int timing;         // 时机（0-100）
    int prediction;     // 预判（0-100）
    int moveBlock;      // 移动拦网（0-100）
    int handShape;      // 手型（0-100）
};

// 技术维度 - 发球属性
struct ServeAttr {
    int speed;          // 速度（0-100）
    int spin;           // 旋转（0-100）
    int placement;      // 落点（0-100）
    int stability;      // 稳定性（0-100）
};

// 技术维度 - 传球属性
struct PassAttr {
    int accuracy;       // 准确性（0-100）
    int stability;      // 稳定性（0-100）
    int tacticAwareness;// 战术意识（0-100）
};

// 技术维度 - 防守属性
struct DefenseAttr {
    int firstPassQuality; // 一传质量（0-100）
    int mobility;          // 移动能力（0-100）
};

// 技术维度 - 调整属性
struct AdjustAttr {
    int ballHandling;   // 处理球能力（0-100）
    int adjustAttack;   // 调整攻能力（0-100）
};

// 身体素质属性
struct PhysicalAttr {
    int stamina;        // 耐力（0-100）
};

// 心理素质属性
struct MentalAttr {
    int pressureResist; // 抗压能力（0-100）
    int concentration;  // 专注度（0-100）
    int confidence;     // 自信心（0-100）
    int commu_and_teamwork;  // 沟通与配合能力（0-100）
    int teampressure;   // 对队友的压力（0-100）
};

// 智慧维度属性
struct WisdomAttr {
    int experience;     // 比赛经验（0-100）
    int positionUnderstanding; // 特定位置理解（0-100）
    int tacticAwareness;// 战术意识（0-100）
    int gameReading;    // 阅读比赛能力（0-100）
};

// 球员主结构体
struct Player {
    std::string name;       // 球员姓名
    std::string position;   // 球员位置（如主攻、副攻、二传等）
    int gender;       // 性别
    int spike;        // 扣球属性
    int block;        // 拦网属性
    int serve;        // 发球属性
    int pass;         // 传球属性
    int defense;      // 防守属性
    int adjust;       // 调整属性
    int stamina;      // 身体耐力素质
    MentalAttr mental;      // 心理素质
    int wisdom;      // 球商
};

// 全局球员数据
extern std::vector<Player> allPlayers;
extern Player teamA[7], teamB[7];
extern bool used[1000];

// 函数声明
void inputPlayerData();                                                 //输入一个新球员数据
void readData();                                                        //从txt中读取球员数据
void inputPlayer();                                                     //输入球员轮次
void inputPlayerByPreset();
void showAllPlayer();                                                   //显示所有球员
bool isPosition(const Player& player, std::string pos);
std::vector<std::string> split(const std::string& s, char delimiter);




#endif //PLAYER_H
