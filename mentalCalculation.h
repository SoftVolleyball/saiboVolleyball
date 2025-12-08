// mentalCalculations.h
#ifndef MENTAL_CALCULATIONS_H
#define MENTAL_CALCULATIONS_H

#include "game.h"
#include "player.h"
#include "config.h"
#include <cmath>

// 球员状态计算结果结构体
struct PlayerStateAdjustments {
    double staminaEffect;      // 耐力影响
    double mentalEffect;       // 心理素质影响
    double concentrationEffect; // 专注度影响
    double communicationEffect; // 沟通配合影响
    double setFatigue;         // 局数疲劳
    double totalAdjustment;    // 总调整系数
};

// 计算球员状态调整系数
PlayerStateAdjustments calculatePlayerStateAdjustments(
    const Player& player,
    const GameState& game,
    double staminaWeight = 1.0,
    double mentalWeight = 1.0,
    double concentrationWeight = 1.0,
    double communicationWeight = 1.0,
    double fatiguePerSet = 0.1
);

// 计算耐力影响
double calculateStaminaEffect(const Player& player, const GameState& game, double fatiguePerSet = 0.1);

// 计算心理素质影响
double calculateMentalEffect(const Player& player);

// 计算专注度影响
double calculateConcentrationEffect(const Player& player);

// 计算沟通配合影响
double calculateCommunicationEffect(const Player& player);

// 计算综合调整系数（基础版本）
double calculateBaseAdjustment(const Player& player, const GameState& game);

// 计算综合调整系数（可配置版本）
double calculateAdjustmentWithWeights(
    const Player& player,
    const GameState& game,
    const std::vector<double>& weights = {1.0, 1.0, 1.0, 1.0},
    double fatiguePerSet = 0.1
);

#endif // MENTAL_CALCULATIONS_H