# SKM53_MODBUS

## Projeto de leitura de dados do sensor SKM53

### Introdução

Esse projeto faz a leitura da latitude e longitude do sensor SKM53, que utiliza comunicação serial e protocolo NMEA 0183. Então esses dados são enviados ao ESP-01 através do protocolo Modbus RTU também via comunicação serial. Usou a UART do próprio ATMega (implementada via hardware) para a comunicação Modbus e outra UART implementada via software para comunicação com o sensor.

A leitura final dos dados pelos usuários acontece via o aplicativo MQTT Dash. O ESP-01 conecta-se a um broker via internet, então envia a esse broker os dados do sensor. O aplicativo comunica-se com o broker, o qual disponibiliza os dados do sensor. 

Para configurar esse cenário, necessita-se configurar o ESP-01 como station, acessando sua interface web via browser. Para acessar tal interface, liga-se o ESP-01 e conecta-se a sua Access Point (rede wi-fi).

### Instalação

#### Usando Makefile

Para compilar o programa via Makefile, basta executar o comando `make` dentro do diretório raiz do projeto e então será gerado um arquivo `.elf`. 

#### Usando o Eclipse

Basta criar o projeto com o plugin do AVR, copiar esse diretório para o projeto e dar o build na própria IDE.

##### Copiando o arquivo .elf/.hex para o microcontrolador

Conectar-se-a o microcontrolador via USB no computador. No Eclipse, basta configurar a porta e escolher o microcontrolador na janela de configuração do dispositivo: `Propriedades -> Avr -> Avr Dude`. Então para dar o upload, clicar-se-a com botão direito em cima da pasta do projeto e: `AVR -> Upload Project to Target Device`.

Outra opção é via linha de comando, onde deve-se passar os argumentos, seguindo o exemplo:

    avrdude -pm328p -carduino -PCOM5 -Uflash:w:SKM53_MODBUS_II.hex

### Colaboradores

- Mariéli Matos

- Rafael Nagel

O projeto foi desenvolvido utilizando o método `peer programming`.
