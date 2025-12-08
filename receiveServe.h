//
// Created by yaorz2 on 25-12-1.
//

#include "player.h"
#include "game.h"

#ifndef RECEIVESERVE_H
#define RECEIVESERVE_H


// 接一质量枚举
enum ReceiveQuality {
    RECEIVE_PERFECT,    // 到位
    RECEIVE_GOOD,       // 半到位
    RECEIVE_BAD,        // 不到位
    RECEIVE_FAULT       // 接飞
};

// 接一结果结构体
struct ReceiveResult {
    ReceiveQuality quality;     // 接一质量
    int qualityValue;           // 接一质量（数值）
    Player receiver;           // 接一球员
    int position;              // 接一球员在场上的位置索引
    std::string description;   // 接一结果描述
};

// 接一阵型枚举
enum ReceiveFormation {
    FORMATION_4_PLAYER,  // 4人接一
    FORMATION_3_PLAYER   // 3人接一
};

// 函数声明
ReceiveResult simulateReceive(const GameState& game, int receivingTeam, int serveEffectiveness);
ReceiveFormation getReceiveFormation(const GameState& game, int teamID);
std::vector<int> getReceivePlayers(const GameState& game, int teamID, ReceiveFormation formation);
Player selectReceivePlayer(const GameState& game, int teamID, ReceiveFormation formation);
ReceiveQuality calculateReceiveQuality(const Player& receiver, int serveEffectiveness, const GameState& game, int& qualityValue);



#endif //RECEIVESERVE_H
