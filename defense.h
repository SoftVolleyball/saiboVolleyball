//
// Created by yaorz2 on 25-12-1.
//

#ifndef DEFENSE_H
#define DEFENSE_H

#include "player.h"
#include "game.h"
#include "block.h"

// 防守质量枚举
enum DefenseQuality {
    DEFENSE_PERFECT,    // 到位
    DEFENSE_GOOD,       // 半到位
    DEFENSE_BAD,        // 不到位
    DEFENSE_FAULT       // 防守失误
};

// 防守结果结构体
struct DefenseResult {
    DefenseQuality quality;     // 防守质量
    int qualityValue;           // 防守质量数值（0-100）
    Player defender;           // 防守球员
    std::string description;   // 防守结果描述
    bool isSetterDump;         // 是否为拦回球（需要额外处理）
    int ballPower;             // 球的力量（扣球强度或拦回强度）
};

// 防守类型枚举
enum DefenseType {
    DEFENSE_SPIKE,      // 防守扣球
    DEFENSE_BLOCK_BACK  // 防守拦回球
};

// 防守类
class Defender {
public:
    // 构造函数
    Defender(const GameState& gameState, int defendingTeam, int attackingTeam);

    // 选择防守球员
    Player selectDefender(const BlockResultInfo& blockResult, const SpikeResult& spikeResult);

    // 计算防守调整系数
    double calculateDefenseAdjustment(const Player& defender, DefenseType defenseType);

    // 计算防守质量
    DefenseQuality calculateDefenseQuality(const Player& defender, int ballPower, DefenseType defenseType, int& qualityValue);

    // 模拟防守扣球
    DefenseResult simulateDefenseAgainstSpike(const SpikeResult& spikeResult, const BlockResultInfo& blockResult);

    // 模拟防守拦回球
    DefenseResult simulateDefenseAgainstBlockBack(const BlockResultInfo& blockResult);

    // 统一的防守模拟函数
    DefenseResult simulateDefense(const SpikeResult& spikeResult, const BlockResultInfo& blockResult);

private:
    GameState gameState;           // 比赛状态
    int defendingTeam;             // 防守方队伍ID
    int attackingTeam;             // 进攻方队伍ID

    // 辅助函数
    const Player* getTeamPlayers(int teamID);
    const int* getRotation(int teamID);
    bool isBackRowPlayer(const Player& player, int teamID);
    std::vector<int> getAvailableDefenders(const BlockResultInfo& blockResult);
};

#endif // DEFENSE_H