#include "control.h"
#include "controlSignal.h"
#include "timer6.h"
#include "tickDelay.h"
#include "ws2812b.h"
#include "hx711.h"
#include "can1.h"
#include "usart2.h"

u8 ctrlMode, enAct, enActChanged, ledFlashFlag;
extern u32 onPowerTime, countDownTime;

extern u8 ledStat[5];

uint8_t red2[][3] = {{255,0,0}};
uint8_t off2[][3] = {{0,0,0}};
uint8_t green2[][3] = {{0,255,0}};

int main() {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    TIM6_Init();
    SysTick_Delay_Init();
    onBoardLedInit();
    ws2812Init();
    USART2_Init();
    hx711_Init();
    //CAN1_Init();
    while(1) {
        if(ledFlashFlag) {
            ledFlashFlag = 0;
            ledFlash();
        }
        /*ws2812_led5_send(&red2[0],4);
        delay_ms(1000);
        ws2812_led5_send(&off2[0],1);
        delay_ms(1000);
        ws2812_led5_send(&off2[0],4);
        delay_ms(1000);*/
        //ws2812_led5_send(&green2[0],2);
        //checkRemoteCMD();
        ledStat[0] = 2;
				ledStat[1] = 2;
				ledStat[2] = 2;
				ledStat[3] = 2;
				ledStat[4] = 2;
				checkSensors();
        /*checkSensors();
        if(enActChanged) {//切换模式
        enActChanged = 0;
        if(enAct) //进入正在激活状态
        ctrlMode = 1;
        else {//进入取消激活状态
        ctrlMode = 0;
        ledOFF();
        }
        }
        if(ctrlMode == 1) {//重生(击错/超时)
        		ctrlMode = 0;
        ledReborn();
        countDownTime = onPowerTime;
        }
        else if(ctrlMode == 2) {//击中
        		ctrlMode = 0;
        ledAdd();
        countDownTime = onPowerTime;
        }
        else if(ctrlMode == 3) {//击满
        		ctrlMode = 0;
        		countDownTime = onPowerTime;
        }*/
    }
}

