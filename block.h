//
// Created by yaorz2 on 25-12-1.
//

#ifndef BLOCK_H
#define BLOCK_H

#include "player.h"
#include "game.h"
#include "spike.h"

// 拦网结果枚举
enum BlockResult {
    BLOCK_BREAK,     // 破坏
    NO_TOUCH,        // 无接触
    LIMIT_PATH,      // 限制球路
    BLOCK_TOUCH,     // 撑起
    BLOCK_BACK       // 拦回
};

// 拦网结果结构体
struct BlockResultInfo {
    BlockResult result;            // 拦网结果
    int blockPower;                // 拦网强度（0-100）
    double blockEffect;            // 拦网效果值（0-1）
    int increasedSpikePower;       // 增加后的扣球强度（如果是破坏）
    int reducedSpikePower;         // 削减后的扣球强度（如果是撑起）
    int blockBackPower;            // 拦回强度（如果是拦回）
    std::vector<Player> blockers;  // 拦网球员列表
    std::string description;       // 描述
};

// 拦网类型枚举（根据进攻类型决定拦网人数）
enum BlockType {
    SINGLE_BLOCK,    // 单人拦网
    DOUBLE_BLOCK,    // 双人拦网
    TRIPLE_BLOCK     // 三人拦网
};

// 拦网类
class Blocker {
public:
    // 构造函数
    Blocker(const GameState& gameState, int blockingTeam, int attackingTeam);

    // 根据进攻类型确定拦网人数
    BlockType determineBlockType(const SpikeResult& spikeResult);

    // 获取拦网球员
    std::vector<Player> getBlockers(BlockType blockType, const SpikeResult& spikeResult);

    // 计算单个拦网球员的拦网强度
    double calculateSingleBlockPower(const Player& blocker, const SpikeResult& spikeResult);

    // 计算组合拦网强度
    int calculateCombinedBlockPower(const std::vector<Player>& blockers, const SpikeResult& spikeResult);

    // 计算拦网效果
    double calculateBlockEffect(int blockPower, int spikePower, double blockCoefficient);

    // 确定拦网结果
    BlockResult determineBlockResult(double blockEffect);

    // 计算破坏时增加的扣球强度
    int calculateIncreasedSpikePower(int spikePower, double blockEffect);

    // 计算撑起时削减的扣球强度
    int calculateReducedSpikePower(int spikePower, double blockEffect);

    // 计算拦回强度
    int calculateBlockBackPower(int blockPower, int spikePower);

    // 模拟拦网过程
    BlockResultInfo simulateBlock(const SpikeResult& spikeResult);

private:
    GameState gameState;           // 比赛状态
    int blockingTeam;              // 拦网方队伍ID（0=A队，1=B队）
    int attackingTeam;             // 进攻方队伍ID

    // 辅助函数
    const Player* getTeamPlayers(int teamID);
    const int* getRotation(int teamID);
    bool isFrontRowPlayer(const Player& player, int teamID);
    bool isBackRowPlayer(const Player& player, int teamID);
    Player getPlayerAtPosition(int teamID, int positionIndex);

    // 获取特定位置的球员
    Player getFrontSpiker(int teamID);   // 前排主攻
    Player getFrontBlocker(int teamID);  // 前排副攻
    Player getOpposite(int teamID);      // 接应
    Player getSetter(int teamID);        // 二传
};

#endif // BLOCK_H