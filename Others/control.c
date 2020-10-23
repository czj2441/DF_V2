#include "control.h"

u8 ledStat[5], fullOnLedNum, halfOnLed;//LED模式（0关、1半开、2全开）；全亮的LED数量；点亮一半的LED即期望LED（传感器交互）
//以下为能量机关各边灯数量，从底部右侧顺时针方向绕一周后从底部向上写T字
/*u8 ledPara[5][13] = {//7
{27,7,7,15,7,7,27,4,9,18,18,9,4},
{27,7,7,15,7,7,27,4,9,18,18,9,4},
{27,7,7,15,7,7,27,4,9,18,18,9,4},
{27,7,7,15,7,7,27,4,9,18,18,9,4},
{27,7,7,15,7,7,27,4,9,18,18,9,4}
};*/
u8 ledEZPara[5] = {100,100,100,100,100};//能量机关锤子灯数量
extern u8 ctrlMode, trigStat[5], ledFlashFlag;
extern u32 onPowerTime, lastDataTime;

u8 getRand(u8 low, u8 high) {//获取随机数
    u8 random;
    srand(onPowerTime);
    do random = rand() % 10;
    while (random < low || random > high);
    delay_ms(2);
    return random;
}

void ledCtrl(u8 ledID, u8 ledMode) {
    ledStat[ledID] = ledMode;
		ledFlashFlag = 1;
}

void ledOFF() {
    for(u8 i = 0; i < 5; i++) ledCtrl(i, 0);
}

void ledAdd() {
    u8 randomLedNum;
    ledCtrl(halfOnLed, 2);//全开上一个半开LED
    fullOnLedNum++;
    if(fullOnLedNum == 5) {//若LED已经全开
        ctrlMode = 3;
        return;
    }
    do randomLedNum = getRand(0, 4);//随机找没开的LED
    while (ledStat[randomLedNum] != 0);
    halfOnLed = randomLedNum;
    ledCtrl(randomLedNum, 1);
}

void ledReborn() {
    u8 randomLedNum;
    ledOFF();
    do randomLedNum = getRand(0, 4);//随机找没开的LED
    //while (ledStat[randomLedNum] != 0);
		while (randomLedNum == halfOnLed);
    halfOnLed = randomLedNum;
    ledCtrl(randomLedNum, 1);
}

uint8_t red[][3] = {{0,255,255}}, off[][3] = {{0,0,0}};

void ledFlash() {//LED控制
    for(u8 i = 0; i < 5; i++)
				if(ledStat[i] == 0) ws2812Ctrl(i,&off[0],180);
				else if(ledStat[i] == 1) {
					ws2812Ctrl(i,&off[0],180);
					ws2812Ctrl(i,&red[0],ledEZPara[i]);
				}
				else if(ledStat[i] == 2) ws2812Ctrl(i,&red[0],180);
}

void checkSensors(){
		hx711_Detect();
		for(u8 i=0;i<5;i++){
				if(trigStat[i] == 1){
					trigStat[i] = 0;
					if(i != halfOnLed) ctrlMode = 1;
					else ctrlMode = 2;
				}
		}
}
