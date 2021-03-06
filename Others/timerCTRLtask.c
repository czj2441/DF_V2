#include "timerCTRLtask.h"

extern u8 enAct, ctrlMode, ledFlashFlag;
extern u32 onPowerTime, countDownTime, lastDataTime, lastCANTime, sensorDelayTime;

void timeTaskControl() {
		if(enAct && (onPowerTime >= countDownTime + TimeoutTimeLenth)) {//如果使能 才开启定时器超时重生功能
            ctrlMode = 1;
            countDownTime = onPowerTime;
		}
		if(onPowerTime % 300 == 0){
				if(onPowerTime - lastDataTime >= 200){
						if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)) GPIO_ResetBits(GPIOB, GPIO_Pin_10);
						else GPIO_SetBits(GPIOB, GPIO_Pin_10);
				}else GPIO_ResetBits(GPIOB, GPIO_Pin_10);
				if(onPowerTime - lastCANTime >= 200){
						if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)) GPIO_ResetBits(GPIOB, GPIO_Pin_11);
						else GPIO_SetBits(GPIOB, GPIO_Pin_11);
				}
		}
		if(sensorDelayTime) sensorDelayTime --;
		//if(onPowerTime % 3000 == 0) ledFlashFlag = 1;
		if(enAct && (onPowerTime >= countDownTime + TimeoutTimeLenth - 200)) {//自动加灯
            ctrlMode = 2;
            countDownTime = onPowerTime;
		}
}
