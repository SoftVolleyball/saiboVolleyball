#ifndef GAME_H
#define GAME_H

#include "player.h"

// 比赛状态结构体
struct GameState {
    int setNum;               // 当前局数（1/2/3）
    int scoreA, scoreB;       // 两队当前局比分
    int serveSide;            // 发球方（0=A队，1=B队）
    int rotateA[6];           // A队轮转位置（存储索引，0-5）
    int rotateB[6];           // B队轮转位置（存储索引，0-5）
    int liberoReplaceA;       // A队当前被自由人换下的副攻索引
    int liberoReplaceB;       // B队当前被自由人换下的副攻索引

    int scoredA[7] = {0}, scoredB[7] = {0};//得分统计
    int faultA[7] = {0}, faultB[7] = {0};  //失误统计

};

// 函数声明
void newGame();
void rotateTeam(GameState& game, int teamID);  //轮转
int playSet(int target, GameState& game);      //一局比赛

#endif