#ifndef __NRF24L01_H__
#define __NRF24L01_H__

#include "NRF24L01_Define.h"


extern uint8_t NRF24L01_TxAddress[];
extern uint8_t NRF24L01_TxPacket[];

extern uint8_t NRF24L01_RxAddress[];
extern uint8_t NRF24L01_RxPacket[];


void NRF24L01_W_CSN(uint8_t BitValue);
uint8_t NRF24L01_SPI_SwapByte(uint8_t Byte);
uint8_t NRF24L01_ReadReg(uint8_t RegAddress);
void NRF24L01_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count);
void NRF24L01_WriteReg(uint8_t RegAddress, uint8_t Data);
void NRF24L01_WriteRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count);
void NRF24L01_ReadRxPayload(uint8_t *DataArray, uint8_t Count);
void NRF24L01_WriteTxPayload(uint8_t *DataArray, uint8_t Count);
void NRF24L01_FlushTx(void);
void NRF24L01_FlushRx(void);
uint8_t NRF24L01_ReadStatus(void);

/*功能函数*/
void NRF24L01_PowerDown(void);
void NRF24L01_StandbyI(void);
void NRF24L01_Rx(void);
void NRF24L01_Tx(void);

void NRF24L01_Init(void);
uint8_t NRF24L01_Send(void);
uint8_t NRF24L01_Receive(void);
void NRF24L01_UpdateRxAddress(void);

#endif
