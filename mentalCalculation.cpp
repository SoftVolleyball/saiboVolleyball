// mentalCalculations.cpp
#include "mentalCalculation.h"
#include <iostream>
#include <algorithm>

// 计算耐力影响
double calculateStaminaEffect(const Player& player, const GameState& game, double fatiguePerSet) {
    // 基础耐力影响：使用四次方根来平滑影响
    double baseStaminaEffect = sqrt(sqrt(player.stamina / 100.0));

    // 疲劳系数：随着局数增加，耐力影响减弱
    double fatigue = 1.0 - (game.setNum - 1) * fatiguePerSet;
    fatigue = std::max(0.1, fatigue); // 确保最低耐力影响

    return baseStaminaEffect * fatigue;
}

// 计算心理素质影响
double calculateMentalEffect(const Player& player) {
    // 心理素质影响公式：0.85 + 0.3 * 心理素质百分比
    return 0.85 + 0.3 * (player.mental.pressureResist / 100.0);
}

// 计算专注度影响
double calculateConcentrationEffect(const Player& player) {
    // 专注度影响公式：0.9 + 0.2 * 专注度百分比
    return 0.9 + 0.2 * (player.mental.concentration / 100.0);
}

// 计算沟通配合影响
double calculateCommunicationEffect(const Player& player) {
    // 沟通配合影响公式：0.9 + 0.2 * 沟通配合百分比
    return 0.9 + 0.2 * (player.mental.commu_and_teamwork / 100.0);
}

// 计算球员状态调整系数
PlayerStateAdjustments calculatePlayerStateAdjustments(
    const Player& player,
    const GameState& game,
    double staminaWeight,
    double mentalWeight,
    double concentrationWeight,
    double communicationWeight,
    double fatiguePerSet) {

    PlayerStateAdjustments adjustments;

    // 计算各个因素的影响
    adjustments.staminaEffect = calculateStaminaEffect(player, game, fatiguePerSet);
    adjustments.mentalEffect = calculateMentalEffect(player);
    adjustments.concentrationEffect = calculateConcentrationEffect(player);
    adjustments.communicationEffect = calculateCommunicationEffect(player);

    // 计算局数疲劳
    adjustments.setFatigue = 1.0 - (game.setNum - 1) * fatiguePerSet;
    adjustments.setFatigue = std::max(0.1, adjustments.setFatigue);

    // 计算总调整系数（应用权重）
    double total = 1.0;
    total *= std::pow(adjustments.staminaEffect, staminaWeight);
    total *= std::pow(adjustments.mentalEffect, mentalWeight);
    total *= std::pow(adjustments.concentrationEffect, concentrationWeight);
    total *= std::pow(adjustments.communicationEffect, communicationWeight);

    adjustments.totalAdjustment = total;

    return adjustments;
}

// 计算综合调整系数（基础版本）
double calculateBaseAdjustment(const Player& player, const GameState& game) {
    auto adjustments = calculatePlayerStateAdjustments(player, game);
    return std::max(0.3, adjustments.totalAdjustment);
}

// 计算综合调整系数（可配置版本）
double calculateAdjustmentWithWeights(
    const Player& player,
    const GameState& game,
    const std::vector<double>& weights,
    double fatiguePerSet) {

    // 设置默认权重
    double staminaWeight = weights.size() > 0 ? weights[0] : 1.0;
    double mentalWeight = weights.size() > 1 ? weights[1] : 1.0;
    double concentrationWeight = weights.size() > 2 ? weights[2] : 1.0;
    double communicationWeight = weights.size() > 3 ? weights[3] : 1.0;

    auto adjustments = calculatePlayerStateAdjustments(
        player, game,
        staminaWeight, mentalWeight, concentrationWeight, communicationWeight,
        fatiguePerSet
    );

    return std::max(0.1, adjustments.totalAdjustment);
}

// 调试信息函数
#if DEBUG_MENTAL_CALC
void printPlayerStateDebugInfo(const Player& player, const GameState& game,
                               const PlayerStateAdjustments& adjustments) {
    std::cout << "\n=== 球员状态计算调试信息 ===" << std::endl;
    std::cout << "球员: " << player.name << std::endl;
    std::cout << "耐力: " << player.stamina << " -> 耐力影响: " << adjustments.staminaEffect << std::endl;
    std::cout << "心理素质: " << player.mental.pressureResist << " -> 心理影响: " << adjustments.mentalEffect << std::endl;
    std::cout << "专注度: " << player.mental.concentration << " -> 专注影响: " << adjustments.concentrationEffect << std::endl;
    std::cout << "沟通配合: " << player.mental.commu_and_teamwork << " -> 沟通影响: " << adjustments.communicationEffect << std::endl;
    std::cout << "局数疲劳系数: " << adjustments.setFatigue << std::endl;
    std::cout << "总调整系数: " << adjustments.totalAdjustment << std::endl;
    std::cout << "===========================" << std::endl;
}
#endif