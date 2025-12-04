#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "ultrasonic.h"
#include <util/delay.h>
#include <math.h>
#include "./uart.h"

#define TRIG_PIN  PD2
#define ECHO_PIN  PD3

#define TRIG_DDR  DDRD
#define TRIG_PORT PORTD

#define ECHO_DDR  DDRD
#define ECHO_PINR PIND

// 物理过滤参数
#define MIN_ECHO_TICKS  300     // ~2cm
#define MAX_ECHO_TICKS  50000   // ~400cm

// 时间调度参数 (基于 Timer3 溢出)
// Timer3 在 2MHz (Prescaler 8) 下，65536 ticks 溢出一次
// 1次溢出 = 65536 * 0.5us = 32.768 ms
#define TICKS_PER_OVF   65536
#define MS_PER_OVF      32.768f 

// 调度间隔
#define INTERVAL_TRIGGER_MS  30.0f   // 每 30ms 触发一次测量
#define INTERVAL_SPEED_MS    100.0f  // 每 100ms 计算一次速度

#define STATE_HOLD_MS  2500.0f // 表情维持

// 全局变量
volatile uint16_t echo_start_ticks = 0;
volatile uint16_t echo_end_ticks   = 0;
volatile uint8_t  measuring        = 0;
volatile uint8_t  echo_captured    = 0;

// 新增：Timer3 溢出计数器，用于系统长时间计时
volatile uint32_t timer3_overflow_count = 0;

volatile float current_distance_cm = 0.0f;
static float last_valid_distance = 0.0f;

// 缓存当前的计算结果，供外部瞬间读取
static SpeedLevel current_speed_level = SPEED_NONE;

extern uint8_t angry_mode;

// =========================================================
//  Helper: 获取当前系统时间 (单位：近似 ms)
//  注意：这里不需要极高精度，用于任务调度即可
// =========================================================
float get_system_time_ms(void) {
    uint32_t ovf;
    uint16_t tcnt;
    
    // 关中断读取原子变量，防止读取过程中发生中断导致数据错乱
    uint8_t sreg = SREG;
    cli();
    ovf = timer3_overflow_count;
    tcnt = TCNT3;
    SREG = sreg;
    sei();

    // 计算总毫秒数: (ovf * 32.768) + (tcnt * 0.0005)
    return (ovf * MS_PER_OVF) + (tcnt * 0.0005f);
}

// =========================================================
//  ISR: Timer3 Overflow (用于长时计时)
// =========================================================
ISR(TIMER3_OVF_vect)
{
    timer3_overflow_count++;
}

// =========================================================
//  ISR: INT1 (超声波回波测量 - 保持不变)
// =========================================================
ISR(INT1_vect)
{
    uint8_t echo_high = PIND & (1 << ECHO_PIN);

    if (echo_high) {
        echo_start_ticks = TCNT3;
        measuring = 1;
        EICRA |=  (1 << ISC11);
        EICRA &= ~(1 << ISC10);
    } 
    else {
        if (!measuring) {
            EICRA |= (1 << ISC11) | (1 << ISC10);
            return;
        }

        echo_end_ticks = TCNT3;
        measuring = 0;

        uint16_t start = echo_start_ticks;
        uint16_t end   = echo_end_ticks;
        uint16_t delta;

        if (end >= start) delta = end - start;
        else delta = (uint16_t)(end + 65536u - start);

        if (delta < MIN_ECHO_TICKS || delta > MAX_ECHO_TICKS) {
            EICRA |= (1 << ISC11) | (1 << ISC10);
            return;
        }

        float pulse_us = delta * 0.5f;
        float d = (pulse_us * 0.0343f) / 2.0f;

        if (last_valid_distance > 0.1f) {
            float jump = d - last_valid_distance;
            if (fabsf(jump) > 40.0f) {
                EICRA |= (1 << ISC11) | (1 << ISC10);
                return;
            }
            current_distance_cm = 0.3f * last_valid_distance + 0.7f * d;
        } else {
            current_distance_cm = d;
        }

        last_valid_distance = current_distance_cm;
        echo_captured = 1; // 标记：新数据好了

        EICRA |= (1 << ISC11) | (1 << ISC10);
    }
}

// =========================================================
//  Trigger Function (内部调用)
// =========================================================
static void ultrasonic_trigger(void)
{
    if (measuring) return; // 如果上次还没回波，不要重发

    TRIG_PORT |= (1 << TRIG_PIN);
    _delay_us(10); // 10us 极短，阻塞无所谓
    TRIG_PORT &= ~(1 << TRIG_PIN);
    
    measuring = 1;
}

// =========================================================
//  Initialization
// =========================================================
void ultrasonic_init(void)
{
    TRIG_DDR |=  (1 << TRIG_PIN);
    TRIG_PORT &= ~(1 << TRIG_PIN);
    ECHO_DDR &= ~(1 << ECHO_PIN);

    // Timer3: Normal mode, Prescaler 8
    TCCR3A = 0;
    TCCR3B = (1 << CS31); 
    TCNT3 = 0;

    // 开启 Timer3 溢出中断，用于系统计时
    TIMSK3 |= (1 << TOIE3); 

    EICRA |= (1<<ISC11) | (1<<ISC10);
    EIMSK |= (1<<INT1);

    current_distance_cm = 0;
    timer3_overflow_count = 0;
    
    // 开启全局中断 (很重要，否则 Timer3 OVF 不工作)
    sei(); 
}

// =========================================================
//  Core Logic: 必须在 Main Loop 中频繁调用
//  这取代了原来的 _delay_ms 阻塞逻辑
// =========================================================
void ultrasonic_update(void)
{
    static float time_last_trigger = 0;
    static float time_last_speed_calc = 0;
    static float dist_at_last_speed_calc = 0;
    
    // 新增：记录最后一次“有情绪”的时间点
    static float time_last_emotion = 0; 
    
    static uint8_t first_run = 1;

    float now = get_system_time_ms();

    // ... (任务1: 触发测量 代码不变) ...
    if ((now - time_last_trigger) > INTERVAL_TRIGGER_MS) {
        if (!measuring) {
            ultrasonic_trigger();
            time_last_trigger = now;
        }
    }

    // ... (任务2: 数据处理 代码不变) ...
    if (echo_captured) {
        echo_captured = 0; 
    }

    // --- 任务 3: 定时计算速度 (每 300ms) ---
    if ((now - time_last_speed_calc) > INTERVAL_SPEED_MS) {
        float d = current_distance_cm;
        
        if (first_run) {
            dist_at_last_speed_calc = d;
            time_last_speed_calc = now;
            first_run = 0;
            return;
        }

        float dt = (now - time_last_speed_calc) / 1000.0f; 
        if (dt <= 0) dt = 0.3f;

        float delta_d = d - dist_at_last_speed_calc;
        float speed = delta_d / dt;


        // ==========================================
        // [DEBUG] 打印原始数据，用于调试阈值
        // ==========================================
        char debug_buf[64];
        // 打印格式：距离 | 位移变化 | 计算出的速度
        // D: Current Distance, dD: Delta Distance, S: Speed
        sprintf(debug_buf, "D:%.1f cm | dD:%.1f | Spd:%.1f\r\n", d, delta_d, speed);
        UART_putstring(debug_buf); 
        // ==========================================

        time_last_speed_calc = now;
        dist_at_last_speed_calc = d;

        // 异常过滤
        if (fabsf(delta_d) > 20.0f) { 
            return; 
        }
        if (fabsf(speed) < 8.0f) {
            speed = 0.0f;
        }

        // === 1. 先计算出“原本应该”是什么状态 (Raw State) ===
        SpeedLevel raw_state = SPEED_NONE;

        if (speed < -5.0f && speed > -15.0f) {
            if (angry_mode == 1) raw_state = SPEED_NONE;
            else raw_state = SPEED_SLOW;
        } else if (speed <= -15.0f) {
            raw_state = SPEED_FAST;
        } else if (speed > 4.0f) {
            raw_state = SPEED_BACK;
        } else {
            raw_state = SPEED_NONE;
        }

        // === 2. 状态保持逻辑 (State Holding) ===
        
        if (raw_state != SPEED_NONE) {
            // Case A: 有新刺激！(FAST, SLOW, BACK)
            // 动作：立即响应，并刷新“最后激动时间”
            current_speed_level = raw_state;
            time_last_emotion = now;
        } 
        else {
            // Case B: 传感器觉得没动静了 (NONE)
            // 动作：检查是否还在“余韵”期内
            if ((now - time_last_emotion) > STATE_HOLD_MS) {
                // 只有过了保持期，才允许变回平静
                current_speed_level = SPEED_NONE;
            }
            // 否则，什么都不做，继续维持上一次的 current_speed_level
        }
    }
}

// =========================================================
//  API: 获取当前计算好的状态 (非阻塞，极快)
// =========================================================
SpeedLevel ultrasonic_get_speed_status(void)
{
    return current_speed_level;
}

// 保留这个API以兼容旧代码，但现在它是非阻塞的了
float ultrasonic_get_distance(void)
{
    return current_distance_cm;
}