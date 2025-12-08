//
// Created by yaorz2 on 25-12-1.
//

#include "supportCal.h"
#include <cmath>

using namespace std;

//计算耐力影响
double calStaminaEffect(int stamina, int gameNum) {
    return sqrt(sqrt(stamina / 100)) * (1.0 - (gameNum - 1) * 0.1);
}

