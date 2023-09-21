/*******************************************************************************
*
* Copyright (C) 2023 Sharopin Yuri
*
* File              : modbus_s.h
* Compiler          : IAR EW ARM 8.32
* Version           : 0.0
* Created File      : 30.08.2023
* Last modified     : 30.08.2023
*
* Support mail      : yshar@ngs.ru
*
* Target MCU        : MCU @ (Xtal = MHz | CPUclk = MHz)
* Description       : Описание MB-регистров  подчиненного 
*                   : 
* Hardware          : .sch .pcb
*
********************************************************************************/
/*===============================[ REDEFINITION DEFENCE ]======================*/ 
#ifndef __MB_REG_s_H__ 
#define __MB_REG_s_H__
/*===============================[ SPECIAL ]===================================*/ 

/* ----------------------- Defines ------------------------------------------*/

#define REG_HOLDING_START   0x0000      // 

#define REG_INPUT_START     0xA500      // TODO начальный адрес регистра
#define REG_INPUT_NREGS     1           // TODO количество регистров в контроллере

#define REG_COILS_START     0xA600      // TODO начальный адрес булевых данных
#define REG_COILS_SIZE      8 

#define MB_SLAVE_ADDR       0x0A

#define  MB_DIR_HARDWARE_RS485   1

//
/*
 * MB_MASTER_TOTAL_SLAVE_NUM
 * https://plc247.com/fx3u-modbus-rtu-ls-ig5a-vfd-tutorial/
 * ЧП отдает только по 8 регистров
 */
typedef enum _MB_REG
{
	REG_R_MODEL_INF			= 0x0000 ,
	REG_R_INV_POW			= 0x0001 ,
	REG_R_INV_VOLT			= 0x0002 ,

	REG_LAST_S                               //
} eMBRegS_t;
/*===============================[ END REDEFINITION DEFENCE]===================*/ 
                         // Окончание однократно включаемого h-файла
                       
#endif
/** (END OF FILE  : modbus_s.h) 
*******************************/
 
