#ifndef _PID_H
#define _PID_H

#include "stm32f10x.h"
#include "basicMath.h"

#define SPEED			0
#define POSITION_180	1
#define POSITION_360	2

typedef struct{
    //PID ������
    double Kp;			//����ϵ������Ҫ������Ӧ�ٶȣ��������׳���
    double Ki;			//����ϵ��������������̬������С��Χ�ڵķ�����
    double Kd;			//΢��ϵ����ʵ�־���Ԥ�⹦�ܣ���С����
    double Ka;			//��ͨ�˲�ϵ�� ����[0,1),��Խ���˲�Լ���ԣ������������𵴵�ʱ�򽫴˲�������

    s16 max_out;  		//������
    u16 dead_band;		//PIDƫ������
    u16 intergral_band;	//������
    u16 intergral_maxOut;//����������
    u16 max_input;		//�������
    u8 model;			//PID�������ͣ�0Ϊ�ٶȿ��ƣ�1Ϊλ�ÿ�������������Ӧ180�ȣ�ʵ�ʶ��������и�����2Ϊλ�ÿ�������������Ӧ360�ȣ�ʵ�ʶ�����Ϊ����

    //PID���ֵ
    s16 output;			//���
    double P_output;		//P���
    double I_output;		//I���
    double D_output;		//D���

    //���
    //u16 err_max;			//������
    double err_max;
    double err;				//��ǰ���
    double err_last;		//��һ�ε����
    double d_last;			//��һ�ε�ϵ��D
} PidTypeDef;//int

void PID_Init(PidTypeDef * pid, double kp, double ki, double kd, double ka, double max_out, double dead_band, double i_band, double max_input, double e_max, double i_maxout, u8 model); //PID��ʼ������
void PID_Calc(PidTypeDef * pid, double rel_val, double set_val);	//PID���㺯��

#endif
