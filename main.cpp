#include <iostream>
#include <cstdlib>
#include "player.h"
#include "game.h"
#include <windows.h>

int in;

void readme() {

    printf("(备注：目前只实现了单自由人，且单自由人必定替换双副攻\n");
    printf(" 输入请确保包含 1个二传1个接应 2副攻 2主攻，且|相互对位|)\n\n");
    printf("对已有球员的位置/数值不满意可以去 /cmake-build-debug/players.txt 里面直接修改\n\n");
    printf("你可以打开config.h来对部分比赛过程中的参数进行调整，或开启调试模式\n\n");
    printf("你可以将config.h中的 USE_PRESET_INPUT 属性值设置为1，以使用预设输入：\n");
    printf("此情况下，程序将自动读取players.txt中1-7位作为A队1-6号位&自由人，8-14位为B队1-6为&自由人\n\n");
    printf("目前的players.txt已经按正确格式预设了两支队伍\n\n");
    system("pause");

}

void mainPage() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    while(1) {
        printf("\n\n-------------\n");
        printf("输入1：导入新球员数据\n");
        printf("输入2：进行一次比赛模拟\n");
        printf("输入3：查看已有球员数据\n");
        printf("输入4：备注(请先阅读此处)\n");
        printf("输入0：结束\n");
        printf("-------------\n");

        std::cin >> in;
        if(in == 1) {
            inputPlayerData();
        } else if(in == 2) {
            newGame();
        } else if(in == 3) {
            showAllPlayer();
        } else if(in == 4) {
            readme();
        } else if(in == 0) {
            exit(0);
        }
    }
}

int main() {
    readData();
    mainPage();
    return 0;
}