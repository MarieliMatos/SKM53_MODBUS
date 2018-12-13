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
	char protocolo[128];
	char latitude[10], longitude[10];
	uint8_t lat_i = 0, long_i = 0;
	uint8_t protocolo_i = 0,
			virgula_counter = 0,
			mensagem_que_eu_quero = 0,
			protocolo_iniciado = FALSE;

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
		byte = softuart_getchar();

		if (byte == '$') {
			fprintf(debug, "\n\r'$'");
			// começa a ler protocolo blabla
			protocolo_iniciado = TRUE;
		}

		if (protocolo_iniciado) {
			protocolo[protocolo_i++] = byte;

			if (protocolo_i == 5) {
				protocolo[6] = '\0';
				fprintf(debug, "protocolo: %s\n\r", &protocolo[1]);
				if (!strcmp("GPGGA", &protocolo[1])) {
					mensagem_que_eu_quero = TRUE;
					fprintf(debug, "\n\rmensagem_que_eu_quero");
				}
				else
					protocolo_i = 0;
			}

			if (mensagem_que_eu_quero) {
				fprintf(debug, "\n\rGPGGA!");
				if (byte == ',') {
					virgula_counter++;
				}
				if (virgula_counter == 3) {
					// lê a latitude aqui
					latitude[lat_i++] = byte;
				} else if (virgula_counter == 5) {
					// le a longitude
					longitude[long_i++] = byte;
				} else if (virgula_counter > 5) {
					if (lat_i == 8 && long_i == 8) {
						// latitude e longitude com tamanhos corretos
						// envia p/ uart p/ debug
						latitude[9] = '\0';
						longitude[9] = '\0';
						fprintf(debug, "\n\r%s", latitude);
						fprintf(debug, "\n\r%s", longitude);

						// zera tudo pra ler proxima mensagem
						mensagem_que_eu_quero = FALSE;
						protocolo_i = 0;
						virgula_counter = 0;
						lat_i = 0;
						long_i = 0;

					}
				}
			}
		}

	}

	return 0;
}
