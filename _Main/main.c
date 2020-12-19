#include "control.h"
#include "controlSignal.h"
#include "timer6.h"
#include "tickDelay.h"
#include "ws2812b.h"
#include "hx711.h"
#include "can1.h"
#include "usart2.h"

u8 ctrlMode, enAct = 1, enActChanged = 1, ledFlashFlag;
u32 sensorDelayTime;
extern u32 onPowerTime, countDownTime;

extern u8 ledStat[5];

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
        //checkRemoteCMD();
				if(!sensorDelayTime) checkSensors();
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
						sensorDelayTime = 200;
        }
        else if(ctrlMode == 2) {//击中
            ctrlMode = 0;
            ledAdd();
            countDownTime = onPowerTime;
						sensorDelayTime = 200;
        }
        else if(ctrlMode == 3) {//击满
            ctrlMode = 0;
						//ledReborn();
            countDownTime = onPowerTime;
						sensorDelayTime = 200;
        }
    }
}

