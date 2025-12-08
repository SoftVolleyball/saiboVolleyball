//
// Created by yaorz2 on 25-12-1.
//

#include "defense.h"
#include "config.h"
#include <algorithm>
#include <vector>
#include <cmath>
#include <iostream>


// 构造函数
Defender::Defender(const GameState& gameState, int defendingTeam, int attackingTeam)
    : gameState(gameState), defendingTeam(defendingTeam), attackingTeam(attackingTeam) {}

// 获取队伍球员数组
const Player* Defender::getTeamPlayers(int teamID) {
    return (teamID == 0) ? teamA : teamB;
}

// 获取轮转数组
const int* Defender::getRotation(int teamID) {
    return (teamID == 0) ? gameState.rotateA : gameState.rotateB;
}

// 判断球员是否在后排
bool Defender::isBackRowPlayer(const Player& player, int teamID) {
    const int* rotation = getRotation(teamID);
    const Player* team = getTeamPlayers(teamID);

    // 后排位置：5号位、6号位、1号位（数组索引4,5,0）
    for (int i : {4, 5, 0}) {
        if (team[rotation[i]].name == player.name) {
            return true;
        }
    }
    return false;
}

// 获取可用的防守球员（排除拦网球员）
std::vector<int> Defender::getAvailableDefenders(const BlockResultInfo& blockResult) {
    const int* rotation = getRotation(defendingTeam);
    const Player* team = getTeamPlayers(defendingTeam);
    std::vector<int> availableDefenders;

    // 所有后排球员都可以防守
    for (int i : {0, 4, 5}) {
        // 检查该球员是否参与拦网
        bool isBlocker = false;
        for (const auto& blocker : blockResult.blockers) {
            if (team[rotation[i]].name == blocker.name) {
                isBlocker = true;
                break;
            }
        }

        if (!isBlocker) {
            availableDefenders.push_back(i);
        }
    }

    // 如果可用防守球员太少，添加一些前排球员（除了拦网球员）
    if (availableDefenders.size() < 2) {
        for (int i : {1, 2, 3}) {
            bool isBlocker = false;
            for (const auto& blocker : blockResult.blockers) {
                if (team[rotation[i]].name == blocker.name) {
                    isBlocker = true;
                    break;
                }
            }

            if (!isBlocker && team[rotation[i]].position != "S" &&
                team[rotation[i]].position != "二传") {
                // 前排非二传球员也可以参与防守
                availableDefenders.push_back(i);
            }
        }
    }

    return availableDefenders;
}

// 选择防守球员
Player Defender::selectDefender(const BlockResultInfo& blockResult, const SpikeResult& spikeResult) {
    const int* rotation = getRotation(defendingTeam);
    const Player* team = getTeamPlayers(defendingTeam);

    // 获取可用防守球员
    std::vector<int> availableDefenders = getAvailableDefenders(blockResult);

    if (availableDefenders.empty()) {
        // 如果没有可用防守球员，返回自由人或第一个后排球员
        for (int i : {0, 4, 5}) {
            if (team[rotation[i]].position == "L" || team[rotation[i]].position == "自由人") {
                return team[rotation[i]];
            }
        }
        return team[rotation[0]]; // 返回第一个球员
    }

    // 根据扣球类型决定防守球员选择
    // 如果是吊球，前排球员防守可能性更大
    if (spikeResult.strategy == DROP_SHOT || spikeResult.strategy == SETTER_SPIKE) {
        // 优先选择前排非拦网球员
        for (int i : {1, 2, 3}) {
            bool isBlocker = false;
            for (const auto& blocker : blockResult.blockers) {
                if (team[rotation[i]].name == blocker.name) {
                    isBlocker = true;
                    break;
                }
            }

            if (!isBlocker &&
                (team[rotation[i]].position == "OH" ||
                 team[rotation[i]].position == "主攻" ||
                 team[rotation[i]].position == "OP" ||
                 team[rotation[i]].position == "接应")) {
                return team[rotation[i]];
            }
        }
    }

    // 否则优先选择自由人或防守好的球员
    // 1. 先找自由人
    for (int i : {0, 4, 5}) {
        if (team[rotation[i]].position == "L" || team[rotation[i]].position == "自由人") {
            // 检查是否是可用防守球员
            for (int idx : availableDefenders) {
                if (idx == i) {
                    return team[rotation[i]];
                }
            }
        }
    }

    // 2. 找防守属性高的球员
    int bestDefenderIdx = availableDefenders[0];
    int bestDefense = team[rotation[bestDefenderIdx]].defense;

    for (size_t i = 1; i < availableDefenders.size(); i++) {
        int idx = availableDefenders[i];
        if (team[rotation[idx]].defense > bestDefense) {
            bestDefense = team[rotation[idx]].defense;
            bestDefenderIdx = idx;
        }
    }

    #if DEBUG_DEFENSE
    std::cout << "\n=== 防守球员选择调试信息 ===" << std::endl;
    std::cout << "扣球类型: " << (spikeResult.strategy == DROP_SHOT ? "吊球" : "扣球") << std::endl;
    std::cout << "可用防守球员数量: " << availableDefenders.size() << std::endl;
    std::cout << "选择球员: " << team[rotation[bestDefenderIdx]].name << std::endl;
    std::cout << "防守属性: " << bestDefense << std::endl;
    std::cout << "===========================" << std::endl;
    #endif

    return team[rotation[bestDefenderIdx]];
}

// 计算防守调整系数
double Defender::calculateDefenseAdjustment(const Player& defender, DefenseType defenseType) {
    double adjustment = 1.0;

    // 耐力影响
    double staminaEffect = sqrt(sqrt(defender.stamina / 100.0));
    double setFatigue = 1.0 - (gameState.setNum - 1) * 0.1;
    double staminaAdjustment = staminaEffect * setFatigue;
    adjustment *= staminaAdjustment;

    // 心理素质影响
    double pressureEffect = defender.mental.pressureResist / 100.0;
    double pressureAdjustment = (0.85 + 0.3 * pressureEffect);
    adjustment *= pressureAdjustment;

    // 专注度影响
    double concentrationEffect = defender.mental.concentration / 100.0;
    double concentrationAdjustment = (0.8 + 0.4 * concentrationEffect);
    adjustment *= concentrationAdjustment;

    // 团队协作影响（防守中很重要）
    double teamworkEffect = defender.mental.commu_and_teamwork / 100.0;
    double teamworkAdjustment = (0.8 + 0.4 * teamworkEffect);
    adjustment *= teamworkAdjustment;

    // 防守拦回球需要更快的反应
    if (defenseType == DEFENSE_BLOCK_BACK) {
        // 拦回球速度很快，需要更高的专注度和反应速度
        adjustment *= (0.6 + 0.6 * concentrationEffect);
    }

    #if DEBUG_DEFENSE
    std::cout << "\n=== 防守调整系数调试信息 ===" << std::endl;
    std::cout << "防守球员: " << defender.name << " 防守类型: " <<
        (defenseType == DEFENSE_BLOCK_BACK ? "拦回球防守" : "扣球防守") << std::endl;
    std::cout << "1. 耐力影响: " << staminaEffect << " * 疲劳系数 " << setFatigue << " = " << staminaAdjustment << std::endl;
    std::cout << "2. 心理素质影响: " << pressureEffect << " => " << pressureAdjustment << std::endl;
    std::cout << "3. 专注度影响: " << concentrationEffect << " => " << concentrationAdjustment << std::endl;
    std::cout << "4. 团队协作影响: " << teamworkEffect << " => " << teamworkAdjustment << std::endl;
    if (defenseType == DEFENSE_BLOCK_BACK) {
        std::cout << "5. 拦回球专注度加成: " << (0.6 + 0.6 * concentrationEffect) << std::endl;
    }
    std::cout << "最终调整系数: " << adjustment << std::endl;
    std::cout << "===========================" << std::endl;
    #endif

    return std::max(0.3, adjustment);
}

// 计算防守质量
DefenseQuality Defender::calculateDefenseQuality(const Player& defender, int ballPower, DefenseType defenseType, int& qualityValue) {


    // 计算调整系数
    double adjustment = calculateDefenseAdjustment(defender, defenseType);

    // 计算防守基础能力
    double baseDefenseAbility = defender.defense * (0.8 + 0.4 * adjustment);

    // 球的力量影响防守难度
    double ballDifficulty = ballPower / 100.0;

    // 计算防守成功率
    // 非线性难度影响，对高难度球的防守惩罚更强
    double difficultyPenalty = ballDifficulty < 1.0 ? ballDifficulty * 0.4 : 0.4 + (ballDifficulty - 1.0) * 0.8;
    double defenseSuccessRate = baseDefenseAbility / 100.0 * adjustment * (1.0 - difficultyPenalty);

    // 添加随机因素
    double randomEffect = (rand() % 20 - 10) / 100.0;
    defenseSuccessRate += randomEffect;
    defenseSuccessRate = std::max(0.0, std::min(1.0, defenseSuccessRate));

    // 根据成功率决定防守质量
    double randomValue = (rand() % 100) / 100.0;

    #if DEBUG_DEFENSE
    std::cout << "\n=== 防守质量计算调试信息 ===" << std::endl;
    std::cout << "防守球员: " << defender.name << " 基础防守能力: " << baseDefenseAbility << std::endl;
    std::cout << "球的力量: " << ballPower << " 球难度系数: " << ballDifficulty << std::endl;
    std::cout << "调整系数: " << adjustment << std::endl;
    std::cout << "计算成功率: " << baseDefenseAbility / 100.0 << " * " << adjustment << " * " << (1.0 - ballDifficulty * 0.4) << " = " << defenseSuccessRate - randomEffect << std::endl;
    std::cout << "随机影响: " << randomEffect << std::endl;
    std::cout << "最终成功率: " << defenseSuccessRate << std::endl;
    std::cout << "随机判定值: " << randomValue << std::endl;
    #endif

    if (defenseType == DEFENSE_BLOCK_BACK) {
        // 拦回球更难防守，质量分布会向下偏移
        if (randomValue < defenseSuccessRate * 0.2) {
            qualityValue = 80 + rand() % 16; // 80-95
            #if DEBUG_DEFENSE
            std::cout << "防守类型: 拦回球" << std::endl;
            std::cout << "完美防守阈值: " << defenseSuccessRate * 0.2 << std::endl;
            std::cout << "防守质量: 完美防守" << std::endl;
            std::cout << "质量值: " << qualityValue << std::endl;
            #endif
            return DEFENSE_PERFECT;
        } else if (randomValue < defenseSuccessRate * 0.5) {
            qualityValue = 60 + rand() % 20; // 60-79
            #if DEBUG_DEFENSE
            std::cout << "防守类型: 拦回球" << std::endl;
            std::cout << "良好防守阈值: " << defenseSuccessRate * 0.5 << std::endl;
            std::cout << "防守质量: 良好防守" << std::endl;
            std::cout << "质量值: " << qualityValue << std::endl;
            #endif
            return DEFENSE_GOOD;
        } else if (randomValue < defenseSuccessRate) {
            qualityValue = 30 + rand() % 30; // 30-59
            #if DEBUG_DEFENSE
            std::cout << "防守类型: 拦回球" << std::endl;
            std::cout << "较差防守阈值: " << defenseSuccessRate << std::endl;
            std::cout << "防守质量: 较差防守" << std::endl;
            std::cout << "质量值: " << qualityValue << std::endl;
            #endif
            return DEFENSE_BAD;
        } else {
            qualityValue = 0;
            #if DEBUG_DEFENSE
            std::cout << "防守类型: 拦回球" << std::endl;
            std::cout << "防守质量: 失误" << std::endl;
            std::cout << "质量值: " << qualityValue << std::endl;
            #endif
            return DEFENSE_FAULT;
        }
    } else {
        // 正常扣球防守
        if (randomValue < defenseSuccessRate * 0.3) {
            qualityValue = 90 + rand() % 11; // 90-100
            #if DEBUG_DEFENSE
            std::cout << "防守类型: 扣球" << std::endl;
            std::cout << "完美防守阈值: " << defenseSuccessRate * 0.3 << std::endl;
            std::cout << "防守质量: 完美防守" << std::endl;
            std::cout << "质量值: " << qualityValue << std::endl;
            #endif
            return DEFENSE_PERFECT;
        } else if (randomValue < defenseSuccessRate * 0.7) {
            qualityValue = 70 + rand() % 20; // 70-89
            #if DEBUG_DEFENSE
            std::cout << "防守类型: 扣球" << std::endl;
            std::cout << "良好防守阈值: " << defenseSuccessRate * 0.7 << std::endl;
            std::cout << "防守质量: 良好防守" << std::endl;
            std::cout << "质量值: " << qualityValue << std::endl;
            #endif
            return DEFENSE_GOOD;
        } else if (randomValue < defenseSuccessRate) {
            qualityValue = 40 + rand() % 30; // 40-69
            #if DEBUG_DEFENSE
            std::cout << "防守类型: 扣球" << std::endl;
            std::cout << "较差防守阈值: " << defenseSuccessRate << std::endl;
            std::cout << "防守质量: 较差防守" << std::endl;
            std::cout << "质量值: " << qualityValue << std::endl;
            #endif
            return DEFENSE_BAD;
        } else {
            qualityValue = 0;
            #if DEBUG_DEFENSE
            std::cout << "防守类型: 扣球" << std::endl;
            std::cout << "防守质量: 失误" << std::endl;
            std::cout << "质量值: " << qualityValue << std::endl;
            #endif
            return DEFENSE_FAULT;
        }
    }

    #if DEBUG_DEFENSE
    std::cout << "===========================" << std::endl;
    #endif
}

// 模拟防守扣球
DefenseResult Defender::simulateDefenseAgainstSpike(const SpikeResult& spikeResult, const BlockResultInfo& blockResult) {
    DefenseResult result;
    result.isSetterDump = false;

    // 确定防守的球的力量
    switch (blockResult.result) {
    case BLOCK_BREAK:
        // 拦网破坏后，使用增加后的扣球强度
        result.ballPower = blockResult.increasedSpikePower;
        break;
    case LIMIT_PATH:
        // 限制球路后，使用略微削减后的扣球强度
        result.ballPower = blockResult.reducedSpikePower;
        break;
    case BLOCK_TOUCH:
        // 撑起后，使用削弱后的扣球强度
        result.ballPower = blockResult.reducedSpikePower;
        break;
    case NO_TOUCH:
        // 无接触，使用原始扣球强度
        result.ballPower = spikeResult.spikePower;
        break;
    case BLOCK_BACK:
        // 拦回情况不会进入此函数，由simulateDefenseAgainstBlockBack处理
        result.ballPower = spikeResult.spikePower;
        break;
    }

#if DEBUG_DEFENSE
    std::cout << "\n\n=== 扣球防守模拟开始 ===" << std::endl;
    std::cout << "扣球力量: " << spikeResult.spikePower << std::endl;
    std::cout << "拦网结果: ";
    switch(blockResult.result) {
    case BLOCK_BREAK: std::cout << "破坏"; break;
    case NO_TOUCH: std::cout << "无接触"; break;
    case LIMIT_PATH: std::cout << "限制球路"; break;
    case BLOCK_TOUCH: std::cout << "撑起"; break;
    case BLOCK_BACK: std::cout << "拦回"; break;
    }
    std::cout << std::endl;
    std::cout << "实际防守球力量: " << result.ballPower << std::endl;
#endif

    // 选择防守球员
    result.defender = selectDefender(blockResult, spikeResult);

    // 计算防守质量
    result.quality = calculateDefenseQuality(result.defender, result.ballPower, DEFENSE_SPIKE, result.qualityValue);

    // 设置防守结果描述
    switch (result.quality) {
        case DEFENSE_PERFECT:
            result.description = "完美防守！可以组织快攻";
            break;
        case DEFENSE_GOOD:
            result.description = "好防守，可以组织进攻";
            break;
        case DEFENSE_BAD:
            result.description = "防守不到位，只能调整攻";
            break;
        case DEFENSE_FAULT:
            result.description = "防守失误！直接失分";
            break;
    }

    #if DEBUG_DEFENSE
    std::cout << "\n=== 扣球防守结果 ===" << std::endl;
    std::cout << "防守球员: " << result.defender.name << std::endl;
    std::cout << "防守质量: " <<
        (result.quality == DEFENSE_PERFECT ? "完美防守" :
         result.quality == DEFENSE_GOOD ? "良好防守" :
         result.quality == DEFENSE_BAD ? "较差防守" : "失误") << std::endl;
    std::cout << "质量值: " << result.qualityValue << std::endl;
    std::cout << "结果描述: " << result.description << std::endl;
    std::cout << "=== 扣球防守模拟结束 ===\n" << std::endl;
    #endif

    return result;
}

// 模拟防守拦回球
DefenseResult Defender::simulateDefenseAgainstBlockBack(const BlockResultInfo& blockResult) {
    DefenseResult result;
    result.isSetterDump = true; // 标记为拦回球

    // 使用拦回强度
    result.ballPower = blockResult.blockBackPower;

    #if DEBUG_DEFENSE
    std::cout << "\n\n=== 拦回球防守模拟开始 ===" << std::endl;
    std::cout << "拦回球力量: " << result.ballPower << std::endl;
    #endif

    // 创建虚拟的扣球结果用于选择防守球员
    SpikeResult dummySpikeResult;
    dummySpikeResult.strategy = STRONG_ATTACK; // 拦回球类似强攻
    dummySpikeResult.spikePower = result.ballPower;

    // 选择防守球员
    result.defender = selectDefender(blockResult, dummySpikeResult);

    // 计算防守质量（拦回球更难防守）
    result.quality = calculateDefenseQuality(result.defender, result.ballPower, DEFENSE_BLOCK_BACK, result.qualityValue);

    // 设置防守结果描述
    switch (result.quality) {
        case DEFENSE_PERFECT:
            result.description = "漂亮！防起拦回球";
            break;
        case DEFENSE_GOOD:
            result.description = "防起拦回球";
            break;
        case DEFENSE_BAD:
            result.description = "勉强防起拦回球";
            break;
        case DEFENSE_FAULT:
            result.description = "拦回球防守失误！直接失分";
            break;
    }

    #if DEBUG_DEFENSE
    std::cout << "\n=== 拦回球防守结果 ===" << std::endl;
    std::cout << "防守球员: " << result.defender.name << std::endl;
    std::cout << "防守质量: " <<
        (result.quality == DEFENSE_PERFECT ? "完美防守" :
         result.quality == DEFENSE_GOOD ? "良好防守" :
         result.quality == DEFENSE_BAD ? "较差防守" : "失误") << std::endl;
    std::cout << "质量值: " << result.qualityValue << std::endl;
    std::cout << "结果描述: " << result.description << std::endl;
    std::cout << "=== 拦回球防守模拟结束 ===\n" << std::endl;
    #endif

    return result;
}

// 统一的防守模拟函数
DefenseResult Defender::simulateDefense(const SpikeResult& spikeResult, const BlockResultInfo& blockResult) {
    #if DEBUG_DEFENSE
    std::cout << "\n\n=== 统一防守模拟开始 ===" << std::endl;
    std::cout << "防守方: " << defendingTeam << " 进攻方: " << attackingTeam << std::endl;
    std::cout << "当前局数: " << gameState.setNum << std::endl;
    std::cout << "拦网结果: " <<
        (blockResult.result == BLOCK_TOUCH ? "撑起" :
         blockResult.result == BLOCK_BACK ? "拦回" :
         "无接触") << std::endl;
    #endif

    if (blockResult.result == BLOCK_BACK) {
        // 防守拦回球
        return simulateDefenseAgainstBlockBack(blockResult);
    } else {
        // 防守扣球
        return simulateDefenseAgainstSpike(spikeResult, blockResult);
    }
}