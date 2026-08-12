#ifndef __TIMER_H
#define __TIMER_H


void Timer_Init(void);

void PWM_LF_SetCCR(uint16_t CCR);
void PWM_RF_SetCCR(uint16_t CCR);
void PWM_LB_SetCCR(uint16_t CCR);
void PWM_RB_SetCCR(uint16_t CCR);
void Serve_Angle_LF(float Angle);
void Serve_Angle_RF(float Angle);
void Serve_Angle_LB(float Angle);
void Serve_Angle_RB(float Angle);

#endif

