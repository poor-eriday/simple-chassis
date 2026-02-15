#include "cmsis_os2.h"
#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "portable.h"
#include "tim.h"
#include "OLED.h"
#include <stdint.h>

typedef enum{
  LF=0,
  RF=1,
  LB=2,
  RB=3,
}Motor_Enum;//LF左前轮,RF右前轮,LB左后轮,RB右后轮（塑料英语www）left，right，forward，backward

void Motor_move(Motor_Enum motor,int16_t speed);
int Map(int val,int in_min,int in_max,int out_min,int out_max);//将输入值映射到输出值范围（未使用）

/*
  *@brief Move_task任务函数
  *@param argument 任务参数（未使用）
  *@note 该任务负责接收AskData_task发送的遥控器数据，计算电机速度并调用Motor_move函数控制电机运动
  */


void Move_task(void *argument)
{

  for(;;)
  {
    int16_t speed_LF = 0;
    int16_t speed_RF = 0;
    int16_t speed_LB = 0;
    int16_t speed_RB = 0;
     Remote *RemoteData = NULL;
     if (xQueueReceive(RemoteQueueHandle, &RemoteData, portMAX_DELAY) == pdPASS)
        {
          
          int16_t temp_1=0;//水平速度值换算
          int16_t temp_2=0;//垂直速度值换算
          
          if (RemoteData->ch1<2054){RemoteData->ch1-=6;}
          if (RemoteData->ch1>2094){RemoteData->ch1-=46;}//死区补偿，防止大范围变化
          if (RemoteData->ch1<=2094&&RemoteData->ch1>=2054)  //通道一的中心在2074，做一点小偏移到2048
          {
            RemoteData->ch1=2048;
          }

          if (RemoteData->ch2<=2068&&RemoteData->ch2>=2028)
          {
            RemoteData->ch2=2048;
          }
          if (RemoteData->ch2<2028){RemoteData->ch2+=20;}
          if (RemoteData->ch2>2068){RemoteData->ch2-=20;}//死区补偿，防止大范围变化


          temp_1=(RemoteData->ch1-2048)*450/2048;//水平速度值换算
          temp_2=-(RemoteData->ch2-2048)*450/2048;//垂直速度值换算
          OLED_ShowNum(3, 1, temp_1, 3);
          OLED_ShowNum(3, 5, temp_2, 3);

          if (RemoteData->key1==1)//左转弯
          {
            speed_LF-=450;
            speed_RF+=450;
            speed_LB-=450;
            speed_RB+=450;
          }
          
          else if (RemoteData->key2==1)//右转弯
          {
            speed_LF+=450;
            speed_RF-=450;
            speed_LB+=450;
            speed_RB-=450;
          }
          else 
          {
            speed_LF=0;
            speed_RF=0;
            speed_LB=0;
            speed_RB=0;
          }

          speed_LF=speed_LF+temp_2-temp_1;//麦轮运动解算
          speed_RF=speed_RF+temp_2+temp_1;
          speed_LB=speed_LB+temp_2+temp_1;  
          speed_RB=speed_RB+temp_2-temp_1;
          
      

          Motor_move(LF, speed_LF);
          Motor_move(RF, speed_RF);
          Motor_move(LB, speed_LB);
          Motor_move(RB, speed_RB);

          OLED_ShowNum(1, 1, speed_LF, 3);
          OLED_ShowNum(1, 5, speed_RF, 3);  //显示必要数据便于debug
          OLED_ShowNum(1, 9, speed_LB, 3);
          OLED_ShowNum(1, 13, speed_RB, 3);

          OLED_ShowNum(2, 1, RemoteData->ch1, 4);
          OLED_ShowNum(2, 6, RemoteData->ch2, 4);
          OLED_ShowNum(2, 11, RemoteData->key1, 2);
          OLED_ShowNum(2, 14, RemoteData->key2, 2);

          vPortFree(RemoteData);
        }
     else
    {
          Motor_move(LF, 0);
          Motor_move(RF, 0);
          Motor_move(LB, 0);
          Motor_move(RB, 0);
          osDelay(20);
    }
    osDelay(1);
  }

}

/*
  *@brief Motor_move函数
  *@param motor 电机枚举值，指定要控制的电机
  *@param speed 电机速度值，范围[-999,999]，正值表示前进，负值表示后退
  *@note 该函数根据输入的电机枚举值和速度值，通过设置定时器的比较值来控制电机的运动
*/


void Motor_move(Motor_Enum motor,int16_t speed)
{
  if (speed>=1000)     //这个speed并非真实速度，实际代表自动重装载值ARR，999时电机满转速
  {
    speed=999;
  }
  if (speed<=-1000)
  {
    speed=-999;
  }
  switch (motor)
  {
  case LF:
    if (speed>=0)
    {
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed); //控制一个电机时要同时设置两个通道，只设置一个代码会相互干涉导致轮子停不下来或者不转
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
    }
    else
    {
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, -speed);
    }
    break;
  case RF:
    if (speed>=0)
    {
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, speed);
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
    }
    else
    {
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, -speed);
    }
    break;
  case LB:
    if (speed>=0)
    {
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
    }
    else
    {
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, -speed);
    }
    break;
  case RB:
    if (speed>=0)
    {
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, speed);
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
    }
    else
    {
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, -speed);
    }
    break;
  default:
    break;
  }
}
/*
  *@brief Map函数
  *@param val 输入值，要进行映射的数值
  *@param in_min 输入范围最小值，输入值的有效范围下限
  *@param in_max 输入范围最大值，输入值的有效范围上限
  *@param out_min 输出范围最小值，映射后数值的有效范围下限
  *@param out_max 输出范围最大值，映射后数值的有效范围上限
  *@return 映射后的数值，范围在[out_min,out_max]之间
  *@note 该函数将输入值val从一个数值范围映射到另一个数值范围，常用于将传感器输出或用户输入映射到电机速度或其他控制参数
*/
int Map(int val,int in_min,int in_max,int out_min,int out_max)
{
	return (int)(val-in_min)*(out_max-out_min)/(in_max-in_min)+out_min;
}
