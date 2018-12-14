/*
 * modbus_rtu.h
 *
 *  Created on: 14 de dez de 2018
 *      Author: Marieli Matos
 *      		Rafael Matos
 *
 */

#ifndef MODBUS_RTU_H_
#define MODBUS_RTU_H_

#include <stdio.h>

void modbus_rtu_init();
void modbus_rtu_tx(uint8_t addr, uint8_t cmd, uint16_t reg, uint16_t data);

#endif /* MODBUS_RTU_H_ */
