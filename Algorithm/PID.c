#include "PID.h"

/**
  * @name 	PID_Init()
  * @brief 	PID参数初始化
  * @param	pid:所要初始化的结构体
  * @param	kp:系数p
  * @param	ki:系数i
  * @param	kd:系数d
  * @param	ka:系数a
  * @param	max_out:最大输出
  * @param	dead_band:死区，在此范围内不进行PID计算直接输出0
  * @param	i_band:积分区间，在此范围内进行积分
  * @param	max_input:期望值的最大输入
  * @param	e_max:计算时的最大误差
  * @param	i_maxout:i的最大输出
  * @return None
  */
void PID_Init(PidTypeDef *pid, double kp, double ki, double kd, double ka, double max_out, double dead_band, double i_band, double max_input, double e_max, double i_maxout, u8 model)
{
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->Ka = ka;
    pid->max_out = max_out;
    pid->intergral_maxOut = i_maxout;
    pid->dead_band = dead_band;
    pid->intergral_band = i_band;
    pid->max_input = max_input;
    //PID输出值
    pid->output = 0;
    //误差
    pid->err_max = e_max;
    pid->d_last = 0;
    pid->model = model;
}

/**
  * @name 	PID_Calc()
  * @brief 	PID计算函数，将输出结果保存在pid->output变量中
  * @param	pid:含有PID计算参数的结构体
  * @param	rel_val:PID计算的实际值值
  * @param	set_val:PID计算的期望值
  * @return None
  */
void PID_Calc(PidTypeDef *pid, double rel_val, double set_val)
{
    if(set_val > pid->max_input)
        set_val = pid->max_input;
    else if(set_val < -(pid->max_input))
        set_val = -(pid->max_input);

    pid->err = set_val - rel_val; //当前误差

    /*360转的PID位置计算过0处理*/
    if(pid->model == POSITION_360)
    {
        if(pid->err > (pid->max_input / 2))
        {
            pid->err = pid->err - pid->max_input;
        }
        else if((-pid->err) > (pid->max_input / 2))
        {
            pid->err = pid->max_input + pid->err;
        }
    }
    else if(pid->model == POSITION_180)
    {
        if(pid->err > (pid->max_input))
        {
            pid->err = pid->err - 2 * pid->max_input;
        }
        else if((-pid->err) > (pid->max_input))
        {
            pid->err = pid->max_input * 2 + pid->err;
        }
    }

    //限制误差大小
    if(pid->err > pid->err_max)
        pid->err = pid->err_max;
    else if(pid->err < -pid->err_max)
        pid->err = -pid->err_max;


    if(f_abs(pid->err) > pid->dead_band) //MyAbs(pid->e[2]) >= pid->dead_band
    {   //判断是否进入死区，没有进入则进行PID运算，进入则输出0

        //判断是否在积分区间内，如果在就进行积分，不在就使得积分逐渐归0
        if(f_abs(pid->err) <= pid->intergral_band)
            pid->I_output = pid->I_output + (pid->Ki) * (pid->err);
        else
            pid->I_output = pid->I_output * 0.99;

        //控制积分最大值
        if(pid->I_output > pid->intergral_maxOut)
            pid->I_output = pid->intergral_maxOut;
        else if(pid->I_output < -pid->intergral_maxOut)
            pid->I_output = -pid->intergral_maxOut;

        //计算PID输出
        pid->P_output = pid->Kp * (pid->err);
        pid->D_output = pid->Kd * (1 - pid->Ka) * (pid->err - pid->err_last) + (pid->Ka) * (pid->d_last);
        if(pid->I_output < 0)
            pid->I_output = 0;
        pid->d_last = pid->D_output;

        pid->output = pid->P_output + pid->I_output + pid->D_output;

        //是否超出最大输出
        if(pid->output > pid->max_out)
            pid->output = pid->max_out;
        if(pid->output < -(pid->max_out))
            pid->output = -(pid->max_out);
    }
    else
    {
        pid->output *= 0.99;
        pid->I_output *= 0.99;
    }

    pid->err_last = pid->err;
}
