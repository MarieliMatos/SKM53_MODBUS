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
#include "usart.h"
#include "avr_usart.h"

#define TRUE 1
#define FALSE 0

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

	/* Obtem o stream de depuração */
	FILE *debug = get_usart_stream();
	USART_Init(B9600);

	/* Mensagem incial: terminal do Proteus
	 * utiliza final de linha com '\r' */
	fprintf(debug, "Teste de debug\n\r");

	sei();

	while (1) {

		if (softuart_getchar() == '$') {
			while (protocolo_i <= 4)
				protocolo[protocolo_i++] = softuart_getchar();

			if (strstr(protocolo, "GPGGA")) {
				// msg q eu quero
				fprintf(debug, "\n\r%s", protocolo);
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

			fprintf(debug, "\n\r %i %i", long_i, lat_i);

			if (long_i == 10 && lat_i == 10) {
				fprintf(debug, "\n\r tamanhos OK");
			}

			fprintf(debug, "\n\r latitude: %s", latitude);
			fprintf(debug, "\n\r longtitude: %s", longitude);

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
