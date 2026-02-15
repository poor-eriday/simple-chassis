#ifndef __REMOTE_TYPE_H__
#define __REMOTE_TYPE_H__

#include <stdint.h>
typedef struct //遥控器数据结构体，id为指令id（默认为0，用于后续拓展），ch1，ch2为摇杆xy轴数据，key1，key2为按键数据
{
  uint8_t id;
  uint32_t ch1;
  uint32_t ch2;
  uint8_t key1;
  uint8_t key2;
} Remote;

#endif
