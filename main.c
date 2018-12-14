/*
 * main.c
 *
 *  Created on: Dec 13, 2018
 *      Author: Mariéli Matos
 *      		Rafael Nagel
 */

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <string.h>

#include "softuart.h"
#include "modbus_rtu.h"

#define TRUE 1
#define FALSE 0

static uint8_t char_to_int(char ch) {
	return (uint8_t)ch - '0';
}

static uint8_t str_nibble_to_uint8(char *str_nibble) {
	return 	char_to_int(str_nibble[0]) +
			char_to_int(str_nibble[1])*10 +
			char_to_int(str_nibble[2])*100 +
			char_to_int(str_nibble[3])*1000;
}

static uint16_t str_to_uint(char *str) {
	uint8_t nibble_ms, nibble_ls;
	nibble_ms = str_nibble_to_uint8(&str[5]);
	nibble_ls = str_nibble_to_uint8(&str[0]);

	return nibble_ms*1000 + nibble_ls;
}

int main(void) {
	char byte;
	char protocolo[6];
	protocolo[5] = '\0';

	char latitude[10], longitude[10];
	latitude[9]   = '\0';
	longitude[9] = '\0';

	uint8_t lat_i = 0, long_i = 0;
	uint8_t protocolo_i = 0, virgula_counter = 0, mensagem_que_eu_quero = 0;

	softuart_init();
	softuart_turn_rx_on(); /* redundant - on by default */

	sei();

	while (1) {

		if (softuart_getchar() == '$') {
			while (protocolo_i <= 4)
				protocolo[protocolo_i++] = softuart_getchar();

			if (strstr(protocolo, "GPGGA")) {
				mensagem_que_eu_quero = TRUE;
			} else {
				protocolo_i = 0;
				mensagem_que_eu_quero = FALSE;
			}
		}

		if (mensagem_que_eu_quero) {
			virgula_counter = 0;
			lat_i = 0;
			long_i = 0;

			while (virgula_counter < 2) {
				byte = softuart_getchar();
				if (byte == ',')
					virgula_counter++;
			}

			// virgula counter sempre igual a dois aqui
			byte = 0;
			while (byte != ',') {
				byte = softuart_getchar();
				latitude[lat_i++] = byte;
			}
			latitude[lat_i-1] = '\0'; // sobreescreve ',' da lat
			virgula_counter++;

			while (softuart_getchar() != ','){};

			// virgula counter sempre igual a quatro aqui
			byte = 0;
			while (byte != ',') {
				byte = softuart_getchar();
				longitude[long_i++] = byte;
			}
			longitude[long_i-1] = '\0';
			virgula_counter++;

			if (long_i >= 10 && lat_i >= 10) {
				modbus_rtu_tx(0x15, 0x01, 0x05, str_to_uint(latitude));
				modbus_rtu_tx(0x15, 0x01, 0x06, str_to_uint(longitude));
			}

			// zera tudo pra ler proxima mensagem
			mensagem_que_eu_quero = FALSE;
			protocolo_i = 0;

			for (int i = 0; i <= 4; i++)
				protocolo[i++] = 0;
			for (int i = 0; i <= 8; i++) {
				latitude[i] = 0;
				longitude[i] = 0;
			}
		}
	}

	return 0;
}
