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

int main(void){

  uint8_t buffer[1];
  char protocolo[128];
  char latitude[10], longitude[10];
  uint8_t lat_i = 0, long_i = 0;
  uint8_t protocolo_i = 0, virgula_counter = 0, mensagem_que_eu_quero = 0;

	softuart_init();
	softuart_turn_rx_on(); /* redundant - on by default */
	USART_init(_9600, buffer, 1);

  /* Obtem o stream de depuração */
	FILE *debug = get_usart_stream();
  USART_Init(B9600);

  /* Mensagem incial: terminal do Proteus
	 * utiliza final de linha com '\r' */
	fprintf(debug,"Teste de debug\n\r");

	sei();

	while (1){

    if (USART_rx_complete() && buffer[0] == '$') {
      fprintf(debug, "'$' recebido\n");
      // começa a ler protocolo blabla
      protocolo_i++;
      protocolo[0] = buffer[0];
    }

    if (protocolo_i && protocolo_i == 5) {
        protocolo[6] = '\0';
        if (strcmp("GPGGA", &protocolo[1]))
          mensagem_que_eu_quero = TRUE;
      }
    }

    if (mensagem_que_eu_quero) {
      fprintf(debug, "mensagem que eu quero!\n");
      if (buffer[0] == ',') {
        virgula_counter++;
      }
      if (virgula_counter == 3) {
        // lê a latitude aqui
        latitude[lat_i++] = buffer[0];
      }
      else if (virgula_counter == 5) {
        // le a longitude
        longitude[long_i++] = buffer[0];
      }
      else if (virgula_counter > 5) {
        if (lat_i == 8 && long_i == 8) {
          // latitude e longitude com tamanhos corretos
          
          // envia p/ uart p/ debug
          latitude[9] = "\n";
          longitude[9] = "\n";
          fprintf(debug, &latitude);
          fprintf(debug, &longitude);

          // zera tudo pra ler proxima mensagem
          mensagem_que_eu_quero = FALSE;
          protocolo_i = 0;
          virgula_counter = 0;
          lat_i = 0;
          long_i = 0;

        }
      }
    }

	return 0;
}
