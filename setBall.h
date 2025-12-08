//
// Created by yaorz2 on 25-12-1.
//

#ifndef SETBALL_H
#define SETBALL_H

#include "player.h"
#include "game.h"
#include "receiveServe.h"

// 传球目标类型枚举
enum PassTarget {
    FRONT_SPIKER,      // 前排主攻
    FRONT_BLOCKER,     // 前排副攻
    BACK_SPIKER,       // 后排主攻
    OPPOSITE,          // 接应（无论前后排）
    SETTER_DUMP,       // 二传二次进攻
    ADJUST_ATTACK      // 调整攻（默认给主攻）
};

// 传球质量枚举
enum PassQuality {
    PERFECT_PASS,      // 完美传球（能打快攻）
    GOOD_PASS,         // 好球（能打强攻）
    DECENT_PASS,       // 一般（能打调整攻）
    POOR_PASS          // 差球（难处理）
};

// 传球结果结构体
struct PassResult {
    PassTarget target;          // 传球目标
    PassQuality quality;        // 传球质量
    int qualityValue;           // 传球质量数值（0-100）
    Player targetPlayer;        // 目标球员
    std::string description;    // 传球结果描述
    bool isSetterDump;          // 是否为二次进攻
    int dumpEffectiveness;      // 二次进攻效果值（如果是二次进攻）
};

// 二次进攻类型枚举
enum DumpType {
    SPIKE_DUMP,  // 二次扣球
    TIP_DUMP     // 二次吊球
};

// 二传类
class Setter {
public:
    // 构造函数
    Setter(const Player& setterPlayer, const GameState& gameState, int teamID);

    // 决定传球目标
    PassTarget decidePassTarget(const ReceiveResult& receiveResult);

    // 获取目标球员
    Player getTargetPlayer(PassTarget target);

    // 计算传球质量
    PassQuality calculatePassQuality(const ReceiveResult& receiveResult, PassTarget target, int& qualityValue);

    // 计算二次进攻质量
    int calculateDumpQuality(const ReceiveResult& receiveResult, DumpType& dumpType);

    // 模拟传球过程
    PassResult simulateSet(const ReceiveResult& receiveResult);

    //评估每个攻手的有效性
    double calculateAttackerEffectiveness(const Player& attacker, int position);

private:
    Player setter;              // 二传球员
    GameState gameState;        // 比赛状态
    int teamID;                 // 队伍ID（0=A队，1=B队）

    // 获取场上位置球员
    const Player* getTeamPlayers();
    const int* getRotation();

    // 辅助函数
    bool isSetterInFrontRow();  // 二传是否在前排
    double calculatePassAdjustment(const ReceiveResult& receiveResult);
    double calculateDumpAdjustment(const ReceiveResult& receiveResult);
};

#endif // SETBALL_H