/*
 * modbus.c
 *
 *  Created on: 14 de dez de 2018
 *      Author: Mariéli Matos
 *      		Rafael Nagel
 */

#include "modbus_rtu.h"
#include "avr_usart.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

static volatile uint8_t pkg_rx[8];
static uint8_t rx_j = 0;

/*
 * modbus_rts_reply:
 * 0: mensagem correta
 * 1: crc invalido
 * 2: reg invalido
 */
static uint8_t modbus_rtu_reply = 0;

ISR(USART_RX_vect) {
	pkg_rx[rx_j++] = USART_0->UDR_;
	if (rx_j == 1) {
		switch (pkg_rx[rx_j]) {
		case 0x01:
			modbus_rtu_reply = 0;
			break;
		case 0xff:
			modbus_rtu_reply = 1;
			break;
		case 0xfe:
			modbus_rtu_reply = 2;
			break;
		}
		rx_j = 0;
	}

	return -1;
}

uint8_t modbus_rtu_get_reply()
{
	return modbus_rtu_reply;
}

void modbus_rtu_init() {
	USART_Init(B9600);
}

static uint16_t CRC16_2(uint8_t *buf, int len)
{
  uint32_t crc = 0xFFFF;
  int i;

  for (i = 0; i < len; i++)
  {
    crc ^= (uint16_t)buf[i];          // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}

//static void le_to_be(char *buffer_le, uint8_t size){
//	uint8_t i,j = size-1;
//	char buffer_aux[8];
//	for(i=0; i<size; i++)
//		buffer_aux[i] = buffer_le[j--];
//	for(i=0; i<size;  i++)
//		buffer_le[i] = buffer_aux[i];
//}

void modbus_rtu_tx(uint8_t addr, uint8_t cmd, uint16_t reg, uint16_t data){
	uint8_t pkg[8];
	uint16_t crc = 0;

	pkg[0] = addr;
	pkg[1] = cmd;

	pkg[2] = reg >> 8;
	pkg[3] = reg & 0x00FF;

	pkg[4] = data >> 8;
	pkg[5] = data & 0x00FF;

	crc = CRC16_2(pkg, 6);

	pkg[6] = crc >> 8;
	pkg[7] = crc & 0x00FF;

	for(uint8_t i = 0; i<8; i++)
		USART_tx(pkg[i]);

	_delay_ms(1000);
}



