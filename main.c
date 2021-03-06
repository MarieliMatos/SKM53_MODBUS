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

#include "avr_usart.h"
#include "softuart.h"
#include "modbus_rtu.h"

#define TRUE 1
#define FALSE 0

static uint8_t char_to_int(char ch) {
	if (ch >= 48 && ch <= 57)
		return (uint8_t)ch - '0';

	return 0;
}

static uint16_t str_nibble_to_uint16(char *str_nibble) {
	return 	char_to_int(str_nibble[3]) +
			char_to_int(str_nibble[2])*10 +
			char_to_int(str_nibble[1])*100 +
			char_to_int(str_nibble[0])*1000;
}

static uint16_t str_to_uint(char *str) {
	uint16_t nibble_ms;
	nibble_ms = str_nibble_to_uint16(&str[1]);
	return nibble_ms*10 + char_to_int(str[6]);
}

int main(void) {
	char byte;
	char protocolo[6];
	protocolo[5] = '\0';

	char latitude[11], longitude[11];
	latitude[9]   = '\0';
	longitude[9] = '\0';

	uint8_t lat_i = 0, long_i = 0;
	uint8_t protocolo_i = 0, virgula_counter = 0, mensagem_que_eu_quero = 0;

	modbus_rtu_init();
	//FILE *debug = get_usart_stream();

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
			latitude[lat_i++] = 0; // pra ficar igual tamanho da longitude (10 bytes)
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

			if (long_i >= 11 && lat_i >= 11) {
				//fprintf(debug, "\r\n%s %s", latitude, longitude);

				modbus_rtu_tx(0x15, 0x01, 0x05, str_to_uint(latitude));
				if (!modbus_rtu_get_reply())  {
					// mensagem OK! Enviada com sucesso.
				}
				else {
					// mensagem c/ ERRO! Enviada sem sucesso.
				}
				modbus_rtu_tx(0x15, 0x01, 0x06, str_to_uint(longitude));
				if (!modbus_rtu_get_reply())  {
					// mensagem OK! Enviada com sucesso.
				}
				else {
					// mensagem c/ ERRO! Enviada sem sucesso.
				}

			}

			// zera tudo pra ler proxima mensagem
			mensagem_que_eu_quero = FALSE;
			protocolo_i = 0;

			for (int i = 0; i <= 4; i++)
				protocolo[i++] = 0;
			for (int i = 0; i <= 9; i++) {
				latitude[i] = 0;
				longitude[i] = 0;
			}
		}
	}

	return 0;
}
