
//
// Created by yaorz2 on 25-12-1.
//

#include "receiveServe.h"
#include "config.h"
#include <iostream>
#include <cmath>

#include "mentalCalculation.h"


// 获取接一阵型
ReceiveFormation getReceiveFormation(const GameState& game, int teamID) {
    const int* rotate = (teamID == 0) ? game.rotateA : game.rotateB;
    const Player* team = (teamID == 0) ? teamA : teamB;

    // 检查接应是否在3号位（前排中间）
    // 轮转位置索引：0=1号位, 1=2号位, 2=3号位, 3=4号位, 4=5号位, 5=6号位
    int oppositePosition = -1;
    for (int i = 0; i < 6; i++) {
        if (team[rotate[i]].position == "OP" || team[rotate[i]].position == "接应") {
            oppositePosition = i;
            break;
        }
    }

    // 如果接应在3号位，采用3人接一；否则4人接一
    ReceiveFormation formation = (oppositePosition == 2) ? FORMATION_3_PLAYER : FORMATION_4_PLAYER;

    #if DEBUG_RECEIVE
    std::cout << "\n=== 接一阵型调试信息 ===" << std::endl;
    std::cout << "接应位置索引: " << oppositePosition << std::endl;
    if (oppositePosition != -1) {
        int actualPosition = oppositePosition + 1;
        std::cout << "接应球员: " << team[rotate[oppositePosition]].name
                  << " 在 " << actualPosition << "号位" << std::endl;
    }
    std::cout << "采用阵型: " << (formation == FORMATION_3_PLAYER ? "3人接一" : "4人接一") << std::endl;
    std::cout << "========================" << std::endl;
    #endif

    return formation;
}

// 获取接一球员列表
std::vector<int> getReceivePlayers(const GameState& game, int teamID, ReceiveFormation formation) {
    const int* rotate = (teamID == 0) ? game.rotateA : game.rotateB;
    const Player* team = (teamID == 0) ? teamA : teamB;
    std::vector<int> receivePlayers;

    // 定义场上位置对应的角色
    // 对于4人接一：两个主攻、自由人、接应（除了二传和前排副攻）
    // 对于3人接一：两个主攻、自由人（除了二传、前排副攻和接应）

    if (formation == FORMATION_4_PLAYER) {
        // 4人接一：除了二传和前排副攻的所有人
        for (int i = 0; i < 6; i++) {
            Player player = team[rotate[i]];
            if (player.position != "S" && player.position != "二传") { // 排除二传
                // 排除前排副攻（位置1、2、3中的副攻）
                if (!(i >= 1 && i <= 3 && (player.position == "MB" || player.position == "副攻"))) {
                    receivePlayers.push_back(i);
                }
            }
        }
    } else { // FORMATION_3_PLAYER
        // 3人接一：两个主攻和自由人
        for (int i = 0; i < 6; i++) {
            Player player = team[rotate[i]];
            if (player.position == "OH" || player.position == "主攻" ||
                player.position == "L" || player.position == "自由人") {
                receivePlayers.push_back(i);
            }
        }
    }

    #if DEBUG_RECEIVE
    std::cout << "\n=== 接一球员列表调试信息 ===" << std::endl;
    std::cout << "阵型: " << (formation == FORMATION_3_PLAYER ? "3人接一" : "4人接一") << std::endl;
    std::cout << "可接一球员数量: " << receivePlayers.size() << std::endl;
    std::cout << "可接一球员: ";
    for (int idx : receivePlayers) {
        std::cout << team[rotate[idx]].name << "(" << idx+1 << "号位) ";
    }
    std::cout << std::endl << "================================" << std::endl;
    #endif

    return receivePlayers;
}

// 选择接一球员
Player selectReceivePlayer(const GameState& game, int teamID, ReceiveFormation formation) {
    const int* rotate = (teamID == 0) ? game.rotateA : game.rotateB;
    const Player* team = (teamID == 0) ? teamA : teamB;

    std::vector<int> receivePlayers = getReceivePlayers(game, teamID, formation);

    // 90%概率发向后排接一球员，10%概率发向前排
    double frontRowProbability = 0.1;
    double randomValue = (rand() % 100) / 100.0;

    #if DEBUG_RECEIVE
    std::cout << "\n=== 选择接一球员调试信息 ===" << std::endl;
    std::cout << "随机值: " << randomValue << " 前排发球阈值: " << frontRowProbability << std::endl;
    #endif

    if (randomValue < frontRowProbability) {
        // 发向前排，由前排副攻接一
        for (int i = 1; i <= 3; i++) { // 前排位置：1,2,3
            Player player = team[rotate[i]];
            if (player.position == "MB" || player.position == "副攻") {
                #if DEBUG_RECEIVE
                std::cout << "发向前排，由前排副攻接一: " << player.name << std::endl;
                std::cout << "=================================" << std::endl;
                #endif
                return player;
            }
        }
        // 如果没有找到前排副攻，随机选择一个前排球员
        int randomFront = 1 + rand() % 3; // 1,2,3
        Player selected = team[rotate[randomFront]];
        #if DEBUG_RECEIVE
        std::cout << "无前排副攻，随机选择前排球员: " << selected.name << std::endl;
        std::cout << "=================================" << std::endl;
        #endif
        return selected;
    } else {
        // 发向后排，随机选择一个接一球员
        int randomIndex = rand() % receivePlayers.size();
        Player selected = team[rotate[receivePlayers[randomIndex]]];
        #if DEBUG_RECEIVE
        std::cout << "发向后排，随机选择后排接一球员: " << selected.name << std::endl;
        std::cout << "=================================" << std::endl;
        #endif
        return selected;
    }
}

// 计算接一调整系数
double calculateReceiveAdjustment(const Player& receiver, const GameState& game) {
    // 使用新的辅助函数
    auto adjustments = calculatePlayerStateAdjustments(receiver, game, 1.0, 1.0, 1.0, 1.0, 0.1);
    double finalAdjustment = std::max(0.3, adjustments.totalAdjustment);

#if DEBUG_RECEIVE
    std::cout << "\n=== 接一调整系数调试信息 ===" << std::endl;
    std::cout << "接一球员: " << receiver.name << " 防守属性: " << receiver.defense << std::endl;
    std::cout << "1. 耐力影响: " << adjustments.staminaEffect << std::endl;
    std::cout << "2. 心理素质影响: " << adjustments.mentalEffect << std::endl;
    std::cout << "3. 专注度影响: " << adjustments.concentrationEffect << std::endl;
    std::cout << "4. 沟通配合影响: " << adjustments.communicationEffect << std::endl;
    std::cout << "最终调整系数(限定范围): " << finalAdjustment << std::endl;
    std::cout << "============================" << std::endl;
#endif

    return finalAdjustment;
}

// 计算接一质量
ReceiveQuality calculateReceiveQuality(const Player& receiver, int serveEffectiveness, const GameState& game, int& qualityValue) {
    // 计算接一基础能力
    double adjustment = calculateReceiveAdjustment(receiver, game);
    double baseReceiveAbility = receiver.defense * (1 + 0.4 * adjustment);

    // 发球强度影响接一难度
    double serveDifficulty = serveEffectiveness / 100.0;

    // 计算接一成功率
    double receiveSuccessRate = baseReceiveAbility / 100.0 * (1.0 - serveDifficulty * 0.3);

    // 添加随机因素
    double randomFactor = (rand() % 5 - 10) / 100.0;//-2.5%到+2.5%
    receiveSuccessRate += randomFactor;
    receiveSuccessRate = std::max(0.0, std::min(1.0, receiveSuccessRate));

    // 根据成功率决定接一质量
    double randomValue = (rand() % 100) / 100.0;

    #if DEBUG_RECEIVE
    std::cout << "\n=== 接一质量计算调试信息 ===" << std::endl;
    std::cout << "发球效果值: " << serveEffectiveness << " => 发球难度系数: " << serveDifficulty << std::endl;
    std::cout << "基础接一能力: " << receiver.defense << " * (" << 0.6 << " + 0.4*" << adjustment << ") = " << baseReceiveAbility << std::endl;
    std::cout << "接一成功率计算: " << baseReceiveAbility/100.0 << " * (1.0 - " << serveDifficulty << "*0.3) = " << receiveSuccessRate-randomFactor << std::endl;
    std::cout << "随机因素: " << randomFactor << std::endl;
    std::cout << "最终接一成功率: " << receiveSuccessRate << std::endl;
    std::cout << "质量判定随机值: " << randomValue << std::endl;
    #endif

    ReceiveQuality quality;

    if (randomValue < receiveSuccessRate * 0.2) {
        qualityValue = 90 + rand() % 11;
        quality = RECEIVE_PERFECT;    // 20%的成功率部分中，完美接一
        #if DEBUG_RECEIVE
        std::cout << "判定: 完美接一 (质量值: " << qualityValue << ")" << std::endl;
        #endif
    } else if (randomValue < receiveSuccessRate * 0.7) {
        qualityValue = 70 + rand() % 20;
        quality = RECEIVE_GOOD;       // 接下来的50%，半到位
        #if DEBUG_RECEIVE
        std::cout << "判定: 半到位 (质量值: " << qualityValue << ")" << std::endl;
        #endif
    } else if (randomValue < receiveSuccessRate) {
        // 不到位：质量值40-69
        qualityValue = 40 + rand() % 30;
        quality = RECEIVE_BAD;        // 接下来的30%，不到位
        #if DEBUG_RECEIVE
        std::cout << "判定: 不到位 (质量值: " << qualityValue << ")" << std::endl;
        #endif
    } else {
        // 接飞：质量值0
        qualityValue = 0;
        quality = RECEIVE_FAULT;      // 失败，接飞
        #if DEBUG_RECEIVE
        std::cout << "判定: 接飞 (质量值: " << qualityValue << ")" << std::endl;
        #endif
    }

    #if DEBUG_RECEIVE
    std::cout << "==============================" << std::endl;
    #endif

    return quality;
}

// 模拟接一过程
ReceiveResult simulateReceive(const GameState& game, int receivingTeam, int serveEffectiveness) {
    ReceiveResult result;

    #if DEBUG_RECEIVE
    std::cout << "\n\n=== 接一模拟开始 ===" << std::endl;
    std::cout << "接一方: " << (receivingTeam == 0 ? "A队" : "B队") << std::endl;
    std::cout << "发球效果值: " << serveEffectiveness << std::endl;
    #endif

    // 确定接一阵型
    ReceiveFormation formation = getReceiveFormation(game, receivingTeam);

    // 选择接一球员
    result.receiver = selectReceivePlayer(game, receivingTeam, formation);

    // 计算接一质量
    result.quality = calculateReceiveQuality(result.receiver, serveEffectiveness, game, result.qualityValue);

    // 设置接一结果描述
    switch (result.quality) {
        case RECEIVE_PERFECT:
            result.description = "到位！完美的一传，可以组织快攻";
            break;
        case RECEIVE_GOOD:
            result.description = "半到位，可以组织强攻";
            break;
        case RECEIVE_BAD:
            result.description = "不到位，只能进行调整攻";
            break;
        case RECEIVE_FAULT:
            result.description = "接飞！直接失分";
            break;
    }

    #if DEBUG_RECEIVE
    std::cout << "\n=== 接一最终结果 ===" << std::endl;
    std::cout << "接一球员: " << result.receiver.name << std::endl;
    std::cout << "接一质量: ";
    switch(result.quality) {
        case RECEIVE_PERFECT: std::cout << "完美"; break;
        case RECEIVE_GOOD: std::cout << "半到位"; break;
        case RECEIVE_BAD: std::cout << "不到位"; break;
        case RECEIVE_FAULT: std::cout << "接飞"; break;
    }
    std::cout << " (质量值: " << result.qualityValue << ")" << std::endl;
    std::cout << "描述: " << result.description << std::endl;
    std::cout << "=== 接一模拟结束 ===\n" << std::endl;
    #endif

    return result;
}