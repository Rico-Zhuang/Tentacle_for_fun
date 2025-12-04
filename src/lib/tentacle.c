#include <avr/io.h>
#include <util/delay.h>
#include "tentacle.h"
#include "PCA9685.h"
#include <math.h>
#define PI 3.1415926
#define DEGREE_TO_RAD(x) ((x)*PI/180.0)

// make sure val is within [min_val, max_val]
int clip(int val, int min_val, int max_val) {
    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}

void Tentacle_Move(float direction_angle, float bending_amount){

    if (bending_amount < 0.0) bending_amount = 0.0;
    if (bending_amount > 1.0) bending_amount = 1.0;

    float amplitude = MAX_BEND * bending_amount;
    float rad = DEGREE_TO_RAD(direction_angle);

    int servo0_angle = clip(CENTER_POS + (int)(amplitude * cos(rad)), 0, 170);
    int servo1_angle = clip(CENTER_POS + (int)(amplitude * cos(rad - 2.0 * PI / 3.0)), 0, 170);
    int servo2_angle = clip(CENTER_POS + (int)(amplitude * cos(rad + 2.0 * PI / 3.0)), 0, 170);

    SetServoAngle(0, servo0_angle);
    SetServoAngle(1, servo1_angle); 
    SetServoAngle(2, servo2_angle);

}

void Tentacle_Center(void) {
    SetServoAngle(0, 90);
    SetServoAngle(1, 90); 
    SetServoAngle(2, 90);
}

// Breathing motion 
void Motion_Breathing(float direction) {
    
    for (int i = 0; i < 360; i+=2) {
        // 使用 sin 生成平滑的起伏，+1 是为了让 sin 结果( -1~1 )变为 (0~2)
        float breath = 0.1 + 0.1 * sin(i * 3.14 / 180.0); 
        
        Tentacle_Move(direction, breath);
        _delay_ms(15); // 调节这个延时改变呼吸频率
    }
}


void Motion_Rotate(float bend_val, int speed_delay) {
    for (int phi = 0; phi < 360; phi += 5) {
        Tentacle_Move(phi, bend_val);
        
        // 可选：在这里调用 delay_ms_variable(speed_delay)
        // 为了简单，这里写死
        for(int k=0; k<speed_delay; k++) _delay_ms(1); 
    }
}
// 锥形扫描
// bend_val: 弯曲程度 (0.3 为小圈扫描，0.8 为大圈扫描)
// speed_delay: 旋转速度 (越小越快)
void Motion_ConicalScan(float bend_val, int speed_delay) {
    for (int phi = 0; phi < 360; phi += 5) {
        Tentacle_Move(phi, bend_val);
        
        // 可选：在这里调用 delay_ms_variable(speed_delay)
        // 为了简单，这里写死
        for(int k=0; k<speed_delay; k++) _delay_ms(1); 
    }
}

// 螺旋展开
void Motion_SpiralOut() {
    float bend = 0.0;
    int phi = 0;
    
    // 当弯曲度小于 1.0 时继续循环
    while (bend <= 1.0) {
        Tentacle_Move(phi, bend);
        
        phi += 10;        // 每次旋转 10 度
        if (phi >= 360) phi -= 360;
        
        bend += 0.005;    // 弯曲度缓慢增加
        _delay_ms(10);
    }
    
    // 动作做完后，慢慢收回中心，避免猛地弹回
    while (bend > 0) {
        bend -= 0.02;     // 快速收回
        Tentacle_Move(phi, bend); // 保持最后的方向收回
        _delay_ms(10);
    }
}

// 啄食动作
// target_angle: 攻击方向
void Motion_Strike(float target_angle) {
    // 1. 蓄力：先稍微向反方向弯一点点 (预备动作)
    Tentacle_Move(target_angle + 180, 0.2); 
    _delay_ms(500);
    
    // 2. 攻击：瞬间弯曲到最大
    Tentacle_Move(target_angle, 1.0);
    _delay_ms(300); // 咬住 300ms
    
    // 3. 撤退：慢慢恢复直立
    for (float b = 1.0; b >= 0; b -= 0.02) {
        Tentacle_Move(target_angle, b);
        _delay_ms(10);
    }
}

