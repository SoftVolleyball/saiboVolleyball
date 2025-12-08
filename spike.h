//
// Created by yaorz2 on 25-12-1.
//

#ifndef SPIKE_H
#define SPIKE_H

#include "player.h"
#include "game.h"
#include "setBall.h"

// 扣球策略枚举
enum SpikeStrategy {
    STRONG_ATTACK,      // 强攻
    AVOID_BLOCK,        // 避手
    DROP_SHOT,          // 吊球
    QUICK_ATTACK,       // 快球
    ADJUST_SPIKE,      // 调整攻
    TRANSITION_ATTACK,   // 过渡
    SETTER_SPIKE         // 二传二次进攻（新增）
};

// 扣球结果结构体
struct SpikeResult {
    SpikeStrategy strategy;          // 扣球策略
    int spikePower;                  // 扣球强度（0-100）
    double blockCoefficient;         // 拦网系数（0-2，越低越难拦）
    bool isError;                    // 是否失误
    bool isOut;                      // 是否出界
    std::string description;         // 扣球结果描述
    Player attacker;                 // 扣球球员
    bool isSetterDump;               // 是否为二次进攻（新增）
};

// 策略属性结构体
struct StrategyAttributes {
    double powerFactor;        // 强度系数
    double blockFactor;        // 拦网系数（基础值）
    double errorRate;          // 基础失误率
    double passQualityEffect;  // 受二传质量影响系数（0-1，越高受二传影响越大）
    double adjustmentEffect;   // 调整属性影响系数
    std::string description;   // 策略描述
};

// 扣球类
class Spiker {
public:
    // 构造函数
    Spiker(const Player& attacker, const GameState& gameState, int teamID);

    // 选择扣球策略
    SpikeStrategy chooseSpikeStrategy(const PassResult& passResult);

    // 获取策略属性
    StrategyAttributes getStrategyAttributes(SpikeStrategy strategy);

    // 计算扣球调整系数
    double calculateSpikeAdjustment(const PassResult& passResult, SpikeStrategy strategy);

    // 计算扣球强度
    int calculateSpikePower(const PassResult& passResult, SpikeStrategy strategy, double adjustment);

    // 计算拦网系数
    double calculateBlockCoefficient(const PassResult& passResult, SpikeStrategy strategy, double adjustment);

    // 计算失误率
    double calculateErrorRate(const PassResult& passResult, SpikeStrategy strategy, double adjustment);

    // 模拟扣球
    SpikeResult simulateSpike(const PassResult& passResult);

    // 创建二次进攻扣球结果（新增）
    static SpikeResult createSetterDumpResult(const Player& setter, int dumpEffectiveness);

private:
    Player attacker;            // 扣球球员
    GameState gameState;        // 比赛状态
    int teamID;                 // 队伍ID（0=A队，1=B队）

    // 辅助函数
    bool isFrontRowAttack(const Player& player);  // 是否是前排进攻
    bool isBackRowAttack(const Player& player);   // 是否是后排进攻
    double getFatigueFactor();                    // 获取疲劳因子
};

#endif // SPIKE_H