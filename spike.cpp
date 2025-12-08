
//
// Created by yaorz2 on 25-12-1.
//

#include "spike.h"
#include "config.h"
#include <algorithm>
#include <cmath>
#include <iostream>


// 构造函数
Spiker::Spiker(const Player& attacker, const GameState& gameState, int teamID)
    : attacker(attacker), gameState(gameState), teamID(teamID) {}

// 判断是否是前排进攻
bool Spiker::isFrontRowAttack(const Player& player) {
    const int* rotation = (teamID == 0) ? gameState.rotateA : gameState.rotateB;
    const Player* team = (teamID == 0) ? teamA : teamB;

    // 前排位置：4号位、3号位、2号位（数组索引3,2,1）
    for (int i : {3, 2, 1}) {
        if (team[rotation[i]].name == player.name) {
            return true;
        }
    }
    return false;
}

// 判断是否是后排进攻
bool Spiker::isBackRowAttack(const Player& player) {
    const int* rotation = (teamID == 0) ? gameState.rotateA : gameState.rotateB;
    const Player* team = (teamID == 0) ? teamA : teamB;

    // 后排位置：5号位、6号位、1号位（数组索引4,5,0）
    for (int i : {4, 5, 0}) {
        if (team[rotation[i]].name == player.name) {
            return true;
        }
    }
    return false;
}


// 获取疲劳因子
double Spiker::getFatigueFactor() {
    double baseFatigue = 1.0 - (gameState.setNum - 1) * 0.1; // 每局疲劳增加10%
    double staminaEffect = sqrt(sqrt(attacker.stamina / 100.0));
    double result = baseFatigue * staminaEffect;

    #if DEBUG_SPIKE
    std::cout << "\n疲劳因子: 基础" << baseFatigue << " * 耐力" << staminaEffect << " = " << result << std::endl;
    #endif

    return result;
}

// 选择扣球策略
SpikeStrategy Spiker::chooseSpikeStrategy(const PassResult& passResult) {
    // 根据传球质量和球员特点选择策略
    double randomValue = (rand() % 100) / 100.0;

    // 判断进攻位置
    bool isFrontRow = isFrontRowAttack(attacker);
    bool isBackRow = isBackRowAttack(attacker);

    // 判断传球质量
    PassQuality passQuality = passResult.quality;
    int passQualityValue = passResult.qualityValue;

    // 球员特点：扣球能力、心理素质等
    double spikeSkill = attacker.spike / 100.0;
    double mentalToughness = attacker.mental.pressureResist / 100.0;
    double confidence = attacker.mental.confidence / 100.0;

    #if DEBUG_SPIKE
    std::cout << "\n=== 扣球策略选择调试信息 ===" << std::endl;
    std::cout << "扣球球员: " << attacker.name << " 扣球属性: " << attacker.spike << std::endl;
    std::cout << "进攻位置: " << (isFrontRow ? "前排" : (isBackRow ? "后排" : "未知")) << std::endl;
    std::cout << "传球质量: ";
    switch(passQuality) {
        case PERFECT_PASS: std::cout << "完美传球"; break;
        case GOOD_PASS: std::cout << "好球"; break;
        case DECENT_PASS: std::cout << "一般传球"; break;
        case POOR_PASS: std::cout << "差球"; break;
    }
    std::cout << " (质量值: " << passQualityValue << ")" << std::endl;
    std::cout << "随机值: " << randomValue << std::endl;
    #endif

    SpikeStrategy strategy;

    // 根据不同传球质量选择策略
    switch (passQuality) {
        case PERFECT_PASS:  // 完美传球
            if (isFrontRow) {
                // 前排可以打快球
                if (randomValue < 0.4) { // 40%概率快球
                    strategy = QUICK_ATTACK;
                    #if DEBUG_SPIKE
                    std::cout << "概率区间: [0, 0.4) => 策略: 快球" << std::endl;
                    #endif
                } else if (randomValue < 0.7) { // 30%概率强攻
                    strategy = STRONG_ATTACK;
                    #if DEBUG_SPIKE
                    std::cout << "概率区间: [0.4, 0.7) => 策略: 强攻" << std::endl;
                    #endif
                } else if (randomValue < 0.85) { // 15%概率避手
                    strategy = AVOID_BLOCK;
                    #if DEBUG_SPIKE
                    std::cout << "概率区间: [0.7, 0.85) => 策略: 避手" << std::endl;
                    #endif
                } else { // 15%概率吊球
                    strategy = DROP_SHOT;
                    #if DEBUG_SPIKE
                    std::cout << "概率区间: [0.85, 1.0) => 策略: 吊球" << std::endl;
                    #endif
                }
            } else {
                // 后排进攻
                if (randomValue < 0.7) { // 70%概率强攻
                    strategy = STRONG_ATTACK;
                    #if DEBUG_SPIKE
                    std::cout << "概率区间: [0, 0.7) => 策略: 强攻" << std::endl;
                    #endif
                } else if (randomValue < 0.9) { // 20%概率避手
                    strategy = AVOID_BLOCK;
                    #if DEBUG_SPIKE
                    std::cout << "概率区间: [0.7, 0.9) => 策略: 避手" << std::endl;
                    #endif
                } else { // 10%概率过渡
                    strategy = TRANSITION_ATTACK;
                    #if DEBUG_SPIKE
                    std::cout << "概率区间: [0.9, 1.0) => 策略: 过渡" << std::endl;
                    #endif
                }
            }
            break;

        case GOOD_PASS:  // 好球
            if (isFrontRow) {
                if (randomValue < 0.5) { // 50%概率强攻
                    strategy = STRONG_ATTACK;
                    #if DEBUG_SPIKE
                    std::cout << "概率区间: [0, 0.5) => 策略: 强攻" << std::endl;
                    #endif
                } else if (randomValue < 0.75) { // 25%概率避手
                    strategy = AVOID_BLOCK;
                    #if DEBUG_SPIKE
                    std::cout << "概率区间: [0.5, 0.75) => 策略: 避手" << std::endl;
                    #endif
                } else if (randomValue < 0.9) { // 15%概率吊球
                    strategy = DROP_SHOT;
                    #if DEBUG_SPIKE
                    std::cout << "概率区间: [0.75, 0.9) => 策略: 吊球" << std::endl;
                    #endif
                } else { // 10%概率调整攻
                    strategy = ADJUST_SPIKE;
                    #if DEBUG_SPIKE
                    std::cout << "概率区间: [0.9, 1.0) => 策略: 调整攻" << std::endl;
                    #endif
                }
            } else {
                if (randomValue < 0.5) { // 50%概率强攻
                    strategy = STRONG_ATTACK;
                    #if DEBUG_SPIKE
                    std::cout << "概率区间: [0, 0.5) => 策略: 强攻" << std::endl;
                    #endif
                } else if (randomValue < 0.7) { // 20%概率避手
                    strategy = AVOID_BLOCK;
                    #if DEBUG_SPIKE
                    std::cout << "概率区间: [0.5, 0.7) => 策略: 避手" << std::endl;
                    #endif
                } else if (randomValue < 0.85) { // 15%概率过渡
                    strategy = TRANSITION_ATTACK;
                    #if DEBUG_SPIKE
                    std::cout << "概率区间: [0.7, 0.85) => 策略: 过渡" << std::endl;
                    #endif
                } else { // 15%概率调整攻
                    strategy = ADJUST_SPIKE;
                    #if DEBUG_SPIKE
                    std::cout << "概率区间: [0.85, 1.0) => 策略: 调整攻" << std::endl;
                    #endif
                }
            }
            break;

        case DECENT_PASS:  // 一般传球
            if (randomValue < 0.4) { // 40%概率调整攻
                strategy = ADJUST_SPIKE;
                #if DEBUG_SPIKE
                std::cout << "概率区间: [0, 0.4) => 策略: 调整攻" << std::endl;
                #endif
            } else if (randomValue < 0.7) { // 30%概率过渡
                strategy = TRANSITION_ATTACK;
                #if DEBUG_SPIKE
                std::cout << "概率区间: [0.4, 0.7) => 策略: 过渡" << std::endl;
                #endif
            } else if (randomValue < 0.85) { // 15%概率吊球
                strategy = DROP_SHOT;
                #if DEBUG_SPIKE
                std::cout << "概率区间: [0.7, 0.85) => 策略: 吊球" << std::endl;
                #endif
            } else { // 15%概率强攻（搏杀）
                strategy = STRONG_ATTACK;
                #if DEBUG_SPIKE
                std::cout << "概率区间: [0.85, 1.0) => 策略: 强攻(搏杀)" << std::endl;
                #endif
            }
            break;

        case POOR_PASS:  // 差球
            if (randomValue < 0.5) { // 50%概率过渡
                strategy = TRANSITION_ATTACK;
                #if DEBUG_SPIKE
                std::cout << "概率区间: [0, 0.5) => 策略: 过渡" << std::endl;
                #endif
            } else if (randomValue < 0.8) { // 30%概率调整攻
                strategy = ADJUST_SPIKE;
                #if DEBUG_SPIKE
                std::cout << "概率区间: [0.5, 0.8) => 策略: 调整攻" << std::endl;
                #endif
            } else { // 20%概率吊球
                strategy = DROP_SHOT;
                #if DEBUG_SPIKE
                std::cout << "概率区间: [0.8, 1.0) => 策略: 吊球" << std::endl;
                #endif
            }
            break;

        default:
            strategy = ADJUST_SPIKE;
            #if DEBUG_SPIKE
            std::cout << "默认策略: 调整攻" << std::endl;
            #endif
    }

    #if DEBUG_SPIKE
    std::cout << "最终扣球策略: ";
    switch(strategy) {
        case STRONG_ATTACK: std::cout << "强攻"; break;
        case AVOID_BLOCK: std::cout << "避手"; break;
        case DROP_SHOT: std::cout << "吊球"; break;
        case QUICK_ATTACK: std::cout << "快球"; break;
        case ADJUST_SPIKE: std::cout << "调整攻"; break;
        case TRANSITION_ATTACK: std::cout << "过渡"; break;
        case SETTER_SPIKE: std::cout << "二次进攻"; break;
    }
    std::cout << std::endl << "===============================" << std::endl;
    #endif

    return strategy;
}

// 获取策略属性
StrategyAttributes Spiker::getStrategyAttributes(SpikeStrategy strategy) {
    StrategyAttributes attr;

    switch (strategy) {
        case STRONG_ATTACK:      // 强攻
            attr.powerFactor = STRONG_ATTACK_POWER;
            attr.blockFactor = STRONG_ATTACK_BLOCK;  // 标准拦网难度
            attr.errorRate = STRONG_ATTACK_FAULT;   // 20%基础失误率
            attr.passQualityEffect = STRONG_ATTACK_PASS;  // 受二传质量影响大
            attr.adjustmentEffect = STRONG_ATTACK_ADJUST;   // 调整属性影响一般
            attr.description = "强力扣杀";
            break;

        case AVOID_BLOCK:        // 避手
            attr.powerFactor = AVOID_BLOCK_POWER;
            attr.blockFactor = AVOID_BLOCK_BLOCK;  // 更难拦网
            attr.errorRate = AVOID_BLOCK_FAULT;   // 25%基础失误率（更高）
            attr.passQualityEffect = AVOID_BLOCK_PASS;  // 受二传质量影响中等
            attr.adjustmentEffect = AVOID_BLOCK_ADJUST;   // 调整属性影响较大
            attr.description = "避手线";
            break;

        case DROP_SHOT:          // 吊球
            attr.powerFactor = DROP_SHOT_POWER;
            attr.blockFactor = DROP_SHOT_BLOCK;  // 很难拦网
            attr.errorRate = DROP_SHOT_FAULT;   // 15%基础失误率
            attr.passQualityEffect = DROP_SHOT_PASS;  // 受二传质量影响小
            attr.adjustmentEffect = DROP_SHOT_ADJUST;   // 调整属性影响大
            attr.description = "轻吊";
            break;

        case QUICK_ATTACK:       // 快球
            attr.powerFactor = QUICK_ATTACK_POWER;
            attr.blockFactor = QUICK_ATTACK_BLOCK;  // 较难拦网
            attr.errorRate = QUICK_ATTACK_FAULT;   // 20%基础失误率
            attr.passQualityEffect = QUICK_ATTACK_PASS;  // 受二传质量影响很大
            attr.adjustmentEffect = QUICK_ATTACK_ADJUST;   // 调整属性影响较大
            attr.description = "快攻";
            break;

        case ADJUST_SPIKE:      // 调整攻
            attr.powerFactor = ADJUST_SPIKE_POWER;
            attr.blockFactor = ADJUST_SPIKE_BLOCK;  // 标准
            attr.errorRate = ADJUST_SPIKE_FAULT;   // 15%基础失误率
            attr.passQualityEffect = ADJUST_SPIKE_PASS;  // 受二传质量影响中等
            attr.adjustmentEffect = ADJUST_SPIKE_ADJUST;   // 调整属性影响很大
            attr.description = "调整攻";
            break;

        case TRANSITION_ATTACK:  // 过渡
            attr.powerFactor = TRANSITION_ATTACK_POWER;
            attr.blockFactor = TRANSITION_ATTACK_BLOCK;  // 不易拦网
            attr.errorRate = TRANSITION_ATTACK_FAULT;   // 10%基础失误率
            attr.passQualityEffect = TRANSITION_ATTACK_PASS;  // 受二传质量影响小
            attr.adjustmentEffect = TRANSITION_ATTACK_ADJUST;   // 调整属性影响大
            attr.description = "过渡球";
            break;

        case SETTER_SPIKE:       // 二次进攻
            attr.powerFactor = SETTER_SPIKE_POWER;
            attr.blockFactor = SETTER_SPIKE_BLOCK;  // 较难拦网
            attr.errorRate = SETTER_SPIKE_FAULT;   // 25%基础失误率
            attr.passQualityEffect = SETTER_SPIKE_PASS;  // 受二传质量影响中等
            attr.adjustmentEffect = SETTER_SPIKE_ADJUST;   // 调整属性影响中等
            attr.description = "二次进攻";
            break;
    }

    #if DEBUG_SPIKE
    std::cout << "\n=== 策略属性调试信息 ===" << std::endl;
    std::cout << "策略: ";
    switch(strategy) {
        case STRONG_ATTACK: std::cout << "强攻"; break;
        case AVOID_BLOCK: std::cout << "避手"; break;
        case DROP_SHOT: std::cout << "吊球"; break;
        case QUICK_ATTACK: std::cout << "快球"; break;
        case ADJUST_SPIKE: std::cout << "调整攻"; break;
        case TRANSITION_ATTACK: std::cout << "过渡"; break;
        case SETTER_SPIKE: std::cout << "二次进攻"; break;
    }
    std::cout << std::endl;
    std::cout << "强度系数: " << attr.powerFactor << std::endl;
    std::cout << "拦网系数(基础): " << attr.blockFactor << std::endl;
    std::cout << "基础失误率: " << attr.errorRate << std::endl;
    std::cout << "二传质量影响系数: " << attr.passQualityEffect << std::endl;
    std::cout << "调整属性影响系数: " << attr.adjustmentEffect << std::endl;
    std::cout << "描述: " << attr.description << std::endl;
    std::cout << "=========================" << std::endl;
    #endif

    return attr;
}

// 计算扣球调整系数
double Spiker::calculateSpikeAdjustment(const PassResult& passResult, SpikeStrategy strategy) {
    double adjustment = 1.0;

    // 获取策略属性
    StrategyAttributes attr = getStrategyAttributes(strategy);

    // 1. 疲劳影响
    adjustment *= getFatigueFactor();

    // 2. 心理素质影响
    double pressureEffect = attacker.mental.pressureResist / 100.0;
    adjustment *= (0.85 + 0.3 * pressureEffect);

    // 3. 自信心影响
    double confidenceEffect = attacker.mental.confidence / 100.0;
    adjustment *= (0.9 + 0.2 * confidenceEffect);

    // 4. 专注度影响
    double concentrationEffect = attacker.mental.concentration / 100.0;
    adjustment *= (0.9 + 0.2 * concentrationEffect);

    // 5. 团队压力（负面）
    double teamPressureEffect = attacker.mental.teampressure / 100.0;
    adjustment *= (1.0 - 0.05 * teamPressureEffect); // 团队压力越大，调整系数越低

    // 6. 调整属性影响：传球质量越差，调整属性影响越大
    double adjustEffect = sqrt(attacker.adjust / 100.0);
    double passQualityFactor = (100.0 - passResult.qualityValue) / 100.0; // 传球质量越差，值越大

    // 调整属性的影响权重随传球质量变化，并考虑策略本身的调整属性影响系数
    double adjustWeight = 0.3 + passQualityFactor * 0.5; // 基础30%，最大增加到80%
    adjustWeight *= attr.adjustmentEffect; // 乘以策略的调整属性影响系数

    adjustment *= (1.0 - adjustWeight + adjustWeight * adjustEffect);

    double finalAdjustment = std::max(0.3, adjustment);

    #if DEBUG_SPIKE
    std::cout << "\n=== 扣球调整系数调试信息 ===" << std::endl;
    std::cout << "1. 疲劳影响: " << getFatigueFactor() << std::endl;
    std::cout << "2. 心理素质影响: " << pressureEffect << " => " << (0.85 + 0.3 * pressureEffect) << std::endl;
    std::cout << "3. 自信心影响: " << confidenceEffect << " => " << (0.9 + 0.2 * confidenceEffect) << std::endl;
    std::cout << "4. 专注度影响: " << concentrationEffect << " => " << (0.9 + 0.2 * concentrationEffect) << std::endl;
    std::cout << "5. 团队压力影响: " << teamPressureEffect << " => " << (1.0 - 0.05 * teamPressureEffect) << std::endl;
    std::cout << "6. 调整属性影响:" << std::endl;
    std::cout << "   传球质量值: " << passResult.qualityValue << " => 传球质量因子: " << passQualityFactor << std::endl;
    std::cout << "   调整属性值: " << attacker.adjust << " => 调整效果: " << adjustEffect << std::endl;
    std::cout << "   调整权重: 基础0.3 + " << passQualityFactor << "*0.5 = " << 0.3 + passQualityFactor * 0.5 << std::endl;
    std::cout << "   策略调整属性系数: " << attr.adjustmentEffect << std::endl;
    std::cout << "   最终调整权重: " << (0.3 + passQualityFactor * 0.5) << " * " << attr.adjustmentEffect << " = " << adjustWeight << std::endl;
    std::cout << "   调整部分: (1 - " << adjustWeight << ") + " << adjustWeight << "*" << adjustEffect << " = " << (1.0 - adjustWeight + adjustWeight * adjustEffect) << std::endl;
    std::cout << "计算后调整系数: " << adjustment << std::endl;
    std::cout << "最终调整系数(限定范围): " << finalAdjustment << std::endl;
    std::cout << "=============================" << std::endl;
    #endif

    return finalAdjustment;
}

// 计算扣球强度
int Spiker::calculateSpikePower(const PassResult& passResult, SpikeStrategy strategy, double adjustment) {
    // 获取策略属性
    StrategyAttributes attr = getStrategyAttributes(strategy);

    // 基础扣球能力
    double baseSpikeAbility = (double)attacker.spike;

    // 传球质量影响
    double passQualityEffect = 1.0 - (1.0 - passResult.qualityValue / 100.0) * attr.passQualityEffect;
    double passInfluence = 0.5 + 0.5 * passQualityEffect; // 传球质量至少影响50%

    // 策略强度系数
    double powerFactor = attr.powerFactor;

    // 计算扣球强度
    double spikePower = baseSpikeAbility * adjustment * powerFactor * passInfluence;

    // 添加后排进攻补正：后排进攻扣球强度削弱15%
    bool isBackRow = isBackRowAttack(attacker);
    if (isBackRow) {
        spikePower *= BACK_ATTACK_POWER_ADJUST; // 后排进攻强度削弱15%
    }

    // 添加随机因素
    double randomFactor = (rand() % 30 - 15);
    spikePower += randomFactor;

    // 限制在合理范围
    spikePower = std::max(0.0, std::min(200.0, spikePower));

    int result = static_cast<int>(spikePower);

    #if DEBUG_SPIKE
    std::cout << "\n=== 扣球强度计算调试信息 ===" << std::endl;
    std::cout << "基础扣球能力: " << baseSpikeAbility << std::endl;
    std::cout << "调整系数: " << adjustment << std::endl;
    std::cout << "策略强度系数: " << powerFactor << std::endl;
    std::cout << "传球质量影响: " << passQualityEffect << std::endl;
    std::cout << "传球影响系数: 0.5 + 0.5*" << passQualityEffect << " = " << passInfluence << std::endl;
    std::cout << "基础计算: " << baseSpikeAbility << " * " << adjustment << " * " << powerFactor << " * " << passInfluence << " = " << baseSpikeAbility * adjustment * powerFactor * passInfluence << std::endl;
    std::cout << "计算后强度: " << spikePower - randomFactor << std::endl;
    std::cout << "后排进攻补正: " << (isBackRow ? "是(削弱15%)" : "否") << std::endl;
    if (isBackRow) {
        std::cout << "后排削弱后强度: " << (spikePower - randomFactor) * 0.85 << std::endl;
    }
    std::cout << "随机因素: " << randomFactor << std::endl;
    std::cout << "最终强度值: " << spikePower << " (四舍五入: " << result << ")" << std::endl;
    std::cout << "============================" << std::endl;
    #endif

    return result;
}

// 计算拦网系数
double Spiker::calculateBlockCoefficient(const PassResult& passResult, SpikeStrategy strategy, double adjustment) {
    // 获取策略属性
    StrategyAttributes attr = getStrategyAttributes(strategy);

    // 基础拦网系数
    double baseBlockCoefficient = attr.blockFactor;

    // 扣球球员的能力影响：扣球能力越强，拦网越难（系数越低）
    double spikeAbilityEffect = attacker.spike / 100.0;
    double blockDifficulty = baseBlockCoefficient * (1.0 - spikeAbilityEffect * 0.3); // 扣球能力最多降低30%

    // 传球质量影响：传球越好，拦网越难
    double passQualityEffect = passResult.qualityValue / 100.0;
    blockDifficulty *= (1.0 - passQualityEffect * 0.2); // 传球质量最多降低20%

    // 调整系数影响：状态越好，拦网越难
    blockDifficulty *= (1.0 - adjustment * 0.2 + 0.2); // 调整系数影响

    // 策略特异性：快球更难拦
    if (strategy == QUICK_ATTACK) {
        blockDifficulty *= 0.8; // 快球再降低20%
    }

    // 添加后排进攻补正：后排进攻更不容易被拦网（降低30%拦网系数）
    bool isBackRow = isBackRowAttack(attacker);
    if (isBackRow) {
        blockDifficulty *= BACK_ATTACK_BLOCK_ADJUST; // 后排进攻降低30%拦网系数
    }

    // 添加随机因素
    double randomFactor = ((rand() % 20) - 10) / 100.0;
    blockDifficulty += randomFactor;

    // 限制范围：0.3-1.5
    blockDifficulty = std::max(0.3, std::min(1.5, blockDifficulty));

    #if DEBUG_SPIKE
    std::cout << "\n=== 拦网系数计算调试信息 ===" << std::endl;
    std::cout << "基础拦网系数: " << baseBlockCoefficient << std::endl;
    std::cout << "扣球能力影响: " << spikeAbilityEffect << " => 降低: " << spikeAbilityEffect * 0.3 << std::endl;
    std::cout << "扣球影响后系数: " << baseBlockCoefficient << " * (1 - " << spikeAbilityEffect << "*0.3) = " << baseBlockCoefficient * (1.0 - spikeAbilityEffect * 0.3) << std::endl;
    std::cout << "传球质量影响: " << passQualityEffect << " => 降低: " << passQualityEffect * 0.2 << std::endl;
    std::cout << "传球影响后系数: " << blockDifficulty / (1.0 - adjustment * 0.2 + 0.2) << " * (1 - " << passQualityEffect << "*0.2) = " << blockDifficulty / (1.0 - adjustment * 0.2 + 0.2) * (1.0 - passQualityEffect * 0.2) << std::endl;
    std::cout << "调整系数影响: " << adjustment << " => 公式: (1 - " << adjustment << "*0.2 + 0.2) = " << (1.0 - adjustment * 0.2 + 0.2) << std::endl;
    std::cout << "策略特异性: " << (strategy == QUICK_ATTACK ? "快球(降低20%)" : "无") << std::endl;
    std::cout << "后排进攻补正: " << (isBackRow ? "是(降低30%)" : "否") << std::endl;
    std::cout << "计算后系数: " << blockDifficulty - randomFactor << std::endl;
    std::cout << "随机因素: " << randomFactor << std::endl;
    std::cout << "最终拦网系数: " << blockDifficulty << " (范围: 0.3-1.5)" << std::endl;
    std::cout << "============================" << std::endl;
    #endif

    return blockDifficulty;
}

// 计算失误率
double Spiker::calculateErrorRate(const PassResult& passResult, SpikeStrategy strategy, double adjustment) {
    // 获取进攻策略
    StrategyAttributes attr = getStrategyAttributes(strategy);

    // 基础失误率
    double baseErrorRate = attr.errorRate;

    // 扣球能力值降低失误率.扣球能力值100时，此值为0
    double errorReductionRate = (100 - attacker.spike) / 100.0;

    // 传球质量影响：传球越差，失误率越高
    double passQualityEffect = (100.0 - passResult.qualityValue) / 100.0;
    double passPenalty = passQualityEffect * 0.3; // 传球差最多增加30%失误率

    // 调整系数影响：状态越差，失误率越高
    double adjustmentPenalty = (1.0 - adjustment) * 0.4; // 调整差最多增加40%失误率

    // 计算最终失误率
    double errorRate = baseErrorRate * errorReductionRate + passPenalty + adjustmentPenalty;

    // 策略特异性：避手失误率更高，吊球失误率更低
    if (strategy == AVOID_BLOCK) {
        errorRate += 0.05; // 避手额外增加5%失误率
    } else if (strategy == DROP_SHOT) {
        errorRate -= 0.03; // 吊球减少3%失误率
    }

    // 添加后排进攻补正：后排进攻增加5%失误率（距离更远，线路更长）
    bool isBackRow = isBackRowAttack(attacker);
    if (isBackRow) {
        errorRate += 0.05; // 后排进攻额外增加5%失误率
    }

    // 添加随机因素
    double randomFactor = ((rand() % 10) - 5) / 100.0;
    errorRate += randomFactor;

    // 限制范围：5%-50%
    errorRate = std::max(0.05, std::min(0.5, errorRate));

    #if DEBUG_SPIKE
    std::cout << "\n=== 失误率计算调试信息 ===" << std::endl;
    std::cout << "基础失误率: " << baseErrorRate << std::endl;
    std::cout << "扣球技能降低失误率: 降低了" << (1-errorReductionRate) * 100 << "% "  << std::endl;
    std::cout << "传球质量影响: 传球质量 " << passResult.qualityValue << " => 质量因子 " << passQualityEffect << " => 增加失误率 " << passPenalty << std::endl;
    std::cout << "调整系数影响: 调整系数 " << adjustment << " => 惩罚 " << (1.0 - adjustment) << " * 0.4 = " << adjustmentPenalty << std::endl;
    std::cout << "初步计算失误率: " << baseErrorRate << " * " << errorReductionRate << " + " << passPenalty << " + " << adjustmentPenalty << " = " << baseErrorRate * errorReductionRate + passPenalty + adjustmentPenalty << std::endl;
    std::cout << "策略特异性: ";
    if (strategy == AVOID_BLOCK) {
        std::cout << "避手(+5%)" << std::endl;
    } else if (strategy == DROP_SHOT) {
        std::cout << "吊球(-3%)" << std::endl;
    } else {
        std::cout << "无" << std::endl;
    }
    std::cout << "后排进攻补正: " << (isBackRow ? "是(+5%)" : "否") << std::endl;
    std::cout << "随机因素: " << randomFactor << std::endl;
    std::cout << "最终失误率: " << errorRate << " (范围: 5%-50%)" << std::endl;
    std::cout << "===========================" << std::endl;
    #endif

    return errorRate;
}

// 模拟扣球
SpikeResult Spiker::simulateSpike(const PassResult& passResult) {
    SpikeResult result;
    result.attacker = attacker;

    #if DEBUG_SPIKE
    std::cout << "\n\n=== 扣球模拟开始 ===" << std::endl;
    std::cout << "扣球球员: " << attacker.name << " 扣球属性: " << attacker.spike << " 调整属性: " << attacker.adjust << std::endl;
    std::cout << "传球质量值: " << passResult.qualityValue << " (";
    switch(passResult.quality) {
        case PERFECT_PASS: std::cout << "完美传球"; break;
        case GOOD_PASS: std::cout << "好球"; break;
        case DECENT_PASS: std::cout << "一般传球"; break;
        case POOR_PASS: std::cout << "差球"; break;
    }
    std::cout << ")" << std::endl;
    #endif

    // 选择扣球策略
    result.strategy = chooseSpikeStrategy(passResult);

    // 判断进攻位置
    bool isFrontRow = isFrontRowAttack(attacker);
    bool isBackRow = isBackRowAttack(attacker);

    // 计算调整系数
    double adjustment = calculateSpikeAdjustment(passResult, result.strategy);

    // 计算扣球强度
    result.spikePower = calculateSpikePower(passResult, result.strategy, adjustment);

    // 计算拦网系数
    result.blockCoefficient = calculateBlockCoefficient(passResult, result.strategy, adjustment);

    // 计算失误率
    double errorRate = calculateErrorRate(passResult, result.strategy, adjustment);

    // 判断是否失误
    double randomValue = (rand() % 100) / 100.0;
    result.isError = (randomValue < errorRate);

    #if DEBUG_SPIKE
    std::cout << "\n=== 扣球结果判定 ===" << std::endl;
    std::cout << "失误率: " << errorRate << " 随机值: " << randomValue << std::endl;
    std::cout << "是否失误: " << (result.isError ? "是" : "否") << std::endl;
    #endif

    if (result.isError) {
        // 判断是出界还是下网
        double errorType = (rand() % 100) / 100.0;
        result.isOut = (errorType > 0.5); // 50%概率出界，50%概率下网

        if (result.isOut) {
            result.description = "出界！失误";
        } else {
            result.description = "下网！失误";
        }

        // 失误时扣球强度和拦网系数设为0
        result.spikePower = 0;
        result.blockCoefficient = 0;

        #if DEBUG_SPIKE
        std::cout << "失误类型: " << (result.isOut ? "出界" : "下网") << std::endl;
        std::cout << "描述: " << result.description << std::endl;
        #endif
    } else {
        // 成功扣球
        result.isOut = false;

        // 根据进攻位置添加描述前缀
        std::string positionPrefix = "";
        if (isBackRow) {
            positionPrefix = "后排";
        } else if (isFrontRow) {
            positionPrefix = "前排";
        }

        // 根据扣球强度给出描述
        if (result.spikePower >= 150) {
            result.description = positionPrefix + "暴力扣杀！";
        } else if (result.spikePower >= 120) {
            result.description = positionPrefix + "有力扣球！";
        } else if (result.spikePower >= 100) {
            result.description = positionPrefix + "标准扣球！";
        } else {
            result.description = positionPrefix + "保守处理！";
        }

        // 如果扣球强度很低但没失误，可能是吊球或过渡
        if (result.spikePower < 60) {
            result.description += "（轻处理）";
        }

        #if DEBUG_SPIKE
        std::cout << "扣球强度: " << result.spikePower << " => 描述: " << result.description << std::endl;
        std::cout << "拦网系数: " << result.blockCoefficient << std::endl;
        #endif
    }

    #if DEBUG_SPIKE
    std::cout << "\n=== 扣球最终结果 ===" << std::endl;
    std::cout << "扣球策略: ";
    switch(result.strategy) {
        case STRONG_ATTACK: std::cout << "强攻"; break;
        case AVOID_BLOCK: std::cout << "避手"; break;
        case DROP_SHOT: std::cout << "吊球"; break;
        case QUICK_ATTACK: std::cout << "快球"; break;
        case ADJUST_SPIKE: std::cout << "调整攻"; break;
        case TRANSITION_ATTACK: std::cout << "过渡"; break;
        case SETTER_SPIKE: std::cout << "二次进攻"; break;
    }
    std::cout << std::endl;
    std::cout << "扣球强度: " << result.spikePower << std::endl;
    std::cout << "拦网系数: " << result.blockCoefficient << std::endl;
    std::cout << "是否失误: " << (result.isError ? "是" : "否") << std::endl;
    if (result.isError) {
        std::cout << "失误类型: " << (result.isOut ? "出界" : "下网") << std::endl;
    }
    std::cout << "描述: " << result.description << std::endl;
    std::cout << "=== 扣球模拟结束 ===\n" << std::endl;
    #endif

    return result;
}

// 创建二次进攻扣球结果
SpikeResult Spiker::createSetterDumpResult(const Player& setter, int dumpEffectiveness) {
    SpikeResult result;
    result.attacker = setter;
    result.strategy = SETTER_SPIKE;
    result.spikePower = dumpEffectiveness;
    result.blockCoefficient = SETTER_SPIKE_BLOCK; // 二次进攻拦网系数较低
    result.isSetterDump = true;

    // 二次进攻的失误率计算
    // 基础失误率：二次进攻相对稳定，基础失误率较低
    double baseErrorRate = 0.15; // 15%基础失误率

    // 二传手的调整属性影响失误率
    double adjustmentEffect = setter.adjust / 100.0;
    double errorReductionRate = 1.0 - (adjustmentEffect * 0.3); // 调整属性最多减少30%失误率

    // 二次进攻效果值影响失误率
    double dumpEffect = dumpEffectiveness / 100.0;
    double effectivenessReduction = 1.0 - (dumpEffect * 0.4); // 效果值高最多减少40%失误率

    // 计算最终失误率
    double errorRate = baseErrorRate * errorReductionRate * effectivenessReduction;

    // 添加随机因素
    double randomFactor = ((rand() % 10) - 5) / 100.0;
    errorRate += randomFactor;

    // 限制范围：5%-30%（二次进攻相对稳定）
    errorRate = std::max(0.05, std::min(0.3, errorRate));

    // 判断是否失误
    double randomValue = (rand() % 100) / 100.0;
    result.isError = (randomValue < errorRate);

    #if DEBUG_SPIKE
    std::cout << "\n=== 二次进攻扣球结果 ===" << std::endl;
    std::cout << "扣球球员: " << setter.name << std::endl;
    std::cout << "扣球强度: " << dumpEffectiveness << std::endl;
    std::cout << "拦网系数: 0.6" << std::endl;
    std::cout << "失误率: " << errorRate << " 随机值: " << randomValue << std::endl;
    std::cout << "是否失误: " << (result.isError ? "是" : "否") << std::endl;
    #endif

    if (result.isError) {
        // 判断是出界还是下网
        double errorType = (rand() % 100) / 100.0;
        result.isOut = (errorType > 0.5); // 50%概率出界，50%概率下网

        if (result.isOut) {
            result.description = "二次进攻出界！失误";
        } else {
            result.description = "二次进攻下网！失误";
        }

        // 失误时扣球强度和拦网系数设为0
        result.spikePower = 0;
        result.blockCoefficient = 0;
    } else {
        // 成功二次进攻
        result.isOut = false;
        result.description = "二次进攻";

        // 根据二次进攻效果值给出描述
        if (dumpEffectiveness >= 150) {
            result.description = "精彩的二次吊球！";
        } else if (dumpEffectiveness >= 120) {
            result.description = "巧妙的二次进攻！";
        } else if (dumpEffectiveness >= 100) {
            result.description = "标准的二次进攻！";
        } else {
            result.description = "保守的二次处理！";
        }
    }

    #if DEBUG_SPIKE
    std::cout << "描述: " << result.description << std::endl;
    std::cout << "========================" << std::endl;
    #endif

    return result;
}