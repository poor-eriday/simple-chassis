#include "Remote_type.h"
#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "NRF24L01.h"
#include "OLED.h"
#include <stdint.h>

uint8_t remote_state=3;//记录接收失败次数
Remote RemoteData_temp;//保存上一次发送数据

/**
 * @brief AskData_task任务函数
 * @param argument 任务参数（未使用）
 * @note 该任务负责从NRF24L01无线模块接收遥控器数据，处理后通过队列发送给Move_task
 *       实现了数据备份、接收失败处理和超时重置机制，确保系统稳定性
 */

void AskData_task(void *argument)
{
  osDelay(100);
  for(;;)
  {
     Remote *RemoteData = pvPortMalloc(sizeof(Remote));//动态分配内存，注意要在使用后清除
     if (RemoteData == NULL) 
     {
        // 内存分配失败，跳过本次循环
        osDelay(10);
        continue;
     }
   
    if(NRF24L01_Receive()==1)//如果接收成功
    {
      remote_state=0;
         RemoteData->id=NRF24L01_RxPacket[0];
         RemoteData->ch1=NRF24L01_RxPacket[1]*100+NRF24L01_RxPacket[2];  //nrf模块一次只能转运32个uint8_t数，所以摇杆的adc转运结果要拆开发送
         RemoteData->ch2=NRF24L01_RxPacket[3]*100+NRF24L01_RxPacket[4];
         RemoteData->key1=NRF24L01_RxPacket[5];
         RemoteData->key2=NRF24L01_RxPacket[6];

         RemoteData_temp.id=RemoteData->id;
         RemoteData_temp.ch1=RemoteData->ch1;
         RemoteData_temp.ch2=RemoteData->ch2;//保存状态预防接收失败
         RemoteData_temp.key1=RemoteData->key1;
         RemoteData_temp.key2=RemoteData->key2;
        
    }
    else 
    {
        remote_state+=1;
        RemoteData->id=RemoteData_temp.id;
        RemoteData->ch1=RemoteData_temp.ch1;
        RemoteData->ch2=RemoteData_temp.ch2;//接收失败时，延续状态
        RemoteData->key1=RemoteData_temp.key1;
        RemoteData->key2=RemoteData_temp.key2;
       
    }
    if (remote_state>=3) 
    {
         RemoteData->ch1=2074;//接收失败次数过多，默认摇杆中间位置
         RemoteData->ch2=2048;
         RemoteData->key1=0;
         RemoteData->key2=0;
    }
    if (xQueueSend(RemoteQueueHandle, &RemoteData, 0) != pdPASS)
    {
        // 如果队列已满，释放内存避免泄漏
        vPortFree(RemoteData);
    }
    
    osDelay(100);//延时100ms，防止频繁接收
  }
}
