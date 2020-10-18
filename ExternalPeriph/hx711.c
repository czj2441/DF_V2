#include "hx711.h"

struct hx711Info hx711PinData[6];

u8 trigStat[5];
#define Sensitivity 3000//全局普适灵敏度
u32 specificSensivity[6] = {0, 0, 0, 0, 1000, 0};
long long adValue1, adValue2, adValue3, adValue4, adValue5;//JSCOPE调试用

void hx711_GPIOconfig() {
    hx711PinData[1].DT_Port = GPIOB;
    hx711PinData[1].DT_Pin = GPIO_Pin_13;
    hx711PinData[1].SCK_Port = GPIOB;
    hx711PinData[1].SCK_Pin = GPIO_Pin_12;

    hx711PinData[2].DT_Port = GPIOB;
    hx711PinData[2].DT_Pin = GPIO_Pin_15;
    hx711PinData[2].SCK_Port = GPIOB;
    hx711PinData[2].SCK_Pin = GPIO_Pin_14;

    hx711PinData[3].DT_Port = GPIOC;
    hx711PinData[3].DT_Pin = GPIO_Pin_7;
    hx711PinData[3].SCK_Port = GPIOC;
    hx711PinData[3].SCK_Pin = GPIO_Pin_6;

    hx711PinData[4].DT_Port = GPIOC;
    hx711PinData[4].DT_Pin = GPIO_Pin_9;
    hx711PinData[4].SCK_Port = GPIOC;
    hx711PinData[4].SCK_Pin = GPIO_Pin_8;

    hx711PinData[5].DT_Port = GPIOA;
    hx711PinData[5].DT_Pin = GPIO_Pin_9;
    hx711PinData[5].SCK_Port = GPIOA;
    hx711PinData[5].SCK_Pin = GPIO_Pin_8;

    u8 t = 0;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
    //初始化DT_IN端口
    for(t = 1; t < 6; t++) {
        GPIO_InitStructure.GPIO_Pin = hx711PinData[t].DT_Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(hx711PinData[t].DT_Port, &GPIO_InitStructure);
    }
    //初始化SCK_OUT端口
    for(t = 1; t < 6; t++) {
        GPIO_InitStructure.GPIO_Pin = hx711PinData[t].SCK_Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//复用推挽输出
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(hx711PinData[t].SCK_Port, &GPIO_InitStructure);
    }
}

void hx711_Init() {
    hx711_GPIOconfig();
    hx711_readAll(3);
    delay_ms(130);
    hx711_readAll(4);
}

long long hx711_readOne(u8 sensorID, long long basicNumber) {
    long long num = 0;
    u8 i;

    GPIO_SetBits(hx711PinData[sensorID].DT_Port, hx711PinData[sensorID].DT_Pin); //DT置1

    GPIO_ResetBits(hx711PinData[sensorID].SCK_Port, hx711PinData[sensorID].SCK_Pin); //SCK置0

    while(GPIO_ReadInputDataBit(hx711PinData[sensorID].DT_Port, hx711PinData[sensorID].DT_Pin)); //转换完成

    //读取数据
    for(i = 0; i < 24; i++) //128倍增益 24个脉冲
    {
        GPIO_SetBits(hx711PinData[sensorID].SCK_Port, hx711PinData[sensorID].SCK_Pin);
        num = num << 1;

        GPIO_ResetBits(hx711PinData[sensorID].SCK_Port, hx711PinData[sensorID].SCK_Pin);

        if(GPIO_ReadInputDataBit(hx711PinData[sensorID].DT_Port, hx711PinData[sensorID].DT_Pin))
            num++;
    }

    GPIO_SetBits(hx711PinData[sensorID].SCK_Port, hx711PinData[sensorID].SCK_Pin); //第25个脉冲完成数据转换
    num = num ^ 0x800000;

    GPIO_ResetBits(hx711PinData[sensorID].SCK_Port, hx711PinData[sensorID].SCK_Pin);

    return num - basicNumber;
    //return s16_abs(num - basicNumber);
}

void hx711_readAll(u8 n) { //传入参数为第几次(3/4 OR 1/2)获取全部数值 3/4为初始化时的第一/二次 第二次均计算平均值（用于解决hx711的adc转换130ms延时问题） 5测试用
    u8 i;
    for(i = 1; i < 6; i++) {//ID:1-5
        switch(n) {
        case 1:
            hx711PinData[i].num1 = hx711_readOne(i, hx711PinData[i].basicNum);
            break;
        case 2:
            hx711PinData[i].num2 = hx711_readOne(i, hx711PinData[i].basicNum);
            hx711PinData[i].numAvg = (hx711PinData[i].num1 + hx711PinData[i].num2) / 2;
            break;
        case 3:
            hx711PinData[i].num1 = hx711_readOne(i, 0);
            break;
        case 4:
            hx711PinData[i].num2 = hx711_readOne(i, 0);
            hx711PinData[i].basicNum = (hx711PinData[i].num1 + hx711PinData[i].num2) / 2;
            break;
        }
        if(n == 2) {//JSCOPE调试用
            switch(i) {
            case 1:
                adValue1 = hx711PinData[i].numAvg;
            case 2:
                adValue2 = hx711PinData[i].numAvg;
            case 3:
                adValue3 = hx711PinData[i].numAvg;
            case 4:
                adValue4 = hx711PinData[i].numAvg;
            case 5:
                adValue5 = hx711PinData[i].numAvg;
            }
        }
    }
}

void hx711_Detect(){
    hx711_readAll(1);
    delay_ms(110);
    hx711_readAll(2);
		delay_ms(55);
    for(int i = 1; i < 6; i++) {
        if((hx711PinData[i].numAvg - hx711PinData[i].oldNumAvg) > (Sensitivity + specificSensivity[i])) {//没有基于初始值做判断 单纯靠最近两次数值
            trigStat[i - 1] = 1;
        }
        hx711PinData[i].oldNumAvg = hx711PinData[i].numAvg;
    }
}
