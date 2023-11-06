#include <asf.h>
#include <string.h>

#include "conf_board.h"

//#define DEBUG_SERIAL

#ifdef DEBUG_SERIAL
#define USART_COM USART1
#define USART_COM_ID ID_USART1
#else
#define USART_COM USART0
#define USART_COM_ID ID_USART0
#endif

// LEDs
#define LED_PIO      PIOC
#define LED_PIO_ID   ID_PIOC
#define LED_IDX      8
#define LED_IDX_MASK (1 << LED_IDX)

// Botão
#define BUT_PIO      PIOA
#define BUT_PIO_ID   ID_PIOA
#define BUT_IDX      11
#define BUT_IDX_MASK (1 << BUT_IDX)

// Botões
// PC13 - C
// PD30 - C#
// PD11 - D
// PA6 - D#
// PD26 - E
// PC19 - F
// PA24 - F#
// PA2 - G
// PA4 - G#
// PA3 - A
// PB4 - A#
// PA21 - B

#define BUT_C_PIO      PIOC
#define BUT_C_PIO_ID   ID_PIOC
#define BUT_C_IDX      13
#define BUT_C_IDX_MASK (1 << BUT_C_IDX)

#define BUT_CS_PIO      PIOD
#define BUT_CS_PIO_ID   ID_PIOD
#define BUT_CS_IDX      30
#define BUT_CS_IDX_MASK (1 << BUT_CS_IDX)

#define BUT_D_PIO      PIOD
#define BUT_D_PIO_ID   ID_PIOD
#define BUT_D_IDX      11
#define BUT_D_IDX_MASK (1 << BUT_D_IDX)

#define BUT_DS_PIO      PIOA
#define BUT_DS_PIO_ID   ID_PIOA
#define BUT_DS_IDX      6
#define BUT_DS_IDX_MASK (1 << BUT_DS_IDX)

#define BUT_E_PIO      PIOD
#define BUT_E_PIO_ID   ID_PIOD
#define BUT_E_IDX      26
#define BUT_E_IDX_MASK (1 << BUT_E_IDX)

#define BUT_F_PIO      PIOC
#define BUT_F_PIO_ID   ID_PIOC
#define BUT_F_IDX      19
#define BUT_F_IDX_MASK (1 << BUT_F_IDX)

#define BUT_FS_PIO      PIOA
#define BUT_FS_PIO_ID   ID_PIOA
#define BUT_FS_IDX      24
#define BUT_FS_IDX_MASK (1 << BUT_FS_IDX)

#define BUT_G_PIO      PIOA
#define BUT_G_PIO_ID   ID_PIOA
#define BUT_G_IDX      2
#define BUT_G_IDX_MASK (1 << BUT_G_IDX)

#define BUT_GS_PIO      PIOA
#define BUT_GS_PIO_ID   ID_PIOA
#define BUT_GS_IDX      4
#define BUT_GS_IDX_MASK (1 << BUT_GS_IDX)

#define BUT_A_PIO      PIOA
#define BUT_A_PIO_ID   ID_PIOA
#define BUT_A_IDX      3
#define BUT_A_IDX_MASK (1 << BUT_A_IDX)

#define BUT_AS_PIO      PIOB
#define BUT_AS_PIO_ID   ID_PIOB
#define BUT_AS_IDX      4
#define BUT_AS_IDX_MASK (1 << BUT_AS_IDX)

#define BUT_B_PIO      PIOA
#define BUT_B_PIO_ID   ID_PIOA
#define BUT_B_IDX      21
#define BUT_B_IDX_MASK (1 << BUT_B_IDX)


/** RTOS  */
#define TASK_OLED_STACK_SIZE                (1024*6/sizeof(portSTACK_TYPE))
#define TASK_OLED_STACK_PRIORITY            (tskIDLE_PRIORITY)

#define TASK_BLUETOOTH_STACK_SIZE            (4096/sizeof(portSTACK_TYPE))
#define TASK_BLUETOOTH_STACK_PRIORITY        (tskIDLE_PRIORITY)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,  signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

QueueHandle_t xQueueBut;

/** prototypes */
void but_C_callback(void);
void but_CS_callback(void);
void but_D_callback(void);
void but_DS_callback(void);
void but_E_callback(void);
void but_F_callback(void);
void but_FS_callback(void);
void but_G_callback(void);
void but_GS_callback(void);
void but_A_callback(void);
void but_AS_callback(void);
void but_B_callback(void);


static void BUT_init(void);
void io_init(void);

/************************************************************************/
/* RTOS application funcs                                               */
/************************************************************************/

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	for (;;) {	}
}

extern void vApplicationIdleHook(void) { }

extern void vApplicationTickHook(void) { }

extern void vApplicationMallocFailedHook(void) {
	configASSERT( ( volatile void * ) NULL );
}

/************************************************************************/
/* handlers / callbacks                                                 */
/************************************************************************/

void but_C_callback(void) {
	uint32_t botao = 1;
	xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
	botao = 0;
}

void but_CS_callback(void) {
  uint32_t botao = 2;
  xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
  botao = 0;
}

void but_D_callback(void) {
  uint32_t botao = 3;
  xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
  botao = 0;
}

void but_DS_callback(void) {
  uint32_t botao = 4;
  xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
  botao = 0;
}

void but_E_callback(void) {
  uint32_t botao = 5;
  xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
  botao = 0;
}

void but_F_callback(void) {
  uint32_t botao = 6;
  xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
  botao = 0;
}

void but_FS_callback(void) {
  uint32_t botao = 7;
  xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
  botao = 0;
}

void but_G_callback(void) {
  uint32_t botao = 8;
  xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
  botao = 0;
}

void but_GS_callback(void) {
  uint32_t botao = 9;
  xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
  botao = 0;
}

void but_A_callback(void) {
  uint32_t botao = 10;
  xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
  botao = 0;
}

void but_AS_callback(void) {
  uint32_t botao = 11;
  xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
  botao = 0;
}

void but_B_callback(void) {
  uint32_t botao = 12;
  xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
  botao = 0;
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

static void configure_console(void) {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.charlength = CONF_UART_CHAR_LENGTH,
		.paritytype = CONF_UART_PARITY,
		.stopbits = CONF_UART_STOP_BITS,
	};

	/* Configure console UART. */
	stdio_serial_init(CONF_UART, &uart_serial_options);

	/* Specify that stdout should not be buffered. */
	setbuf(stdout, NULL);
}

void io_init(void) {

  // Ativa PIOs
  pmc_enable_periph_clk(LED_PIO_ID);

  // Configura Pinos
  pio_configure(LED_PIO, PIO_OUTPUT_0, LED_IDX_MASK, PIO_DEFAULT | PIO_DEBOUNCE);

  pio_configure(BUT_C_PIO, PIO_INPUT, BUT_C_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_C_PIO,
                  BUT_C_PIO_ID,
                  BUT_C_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  but_C_callback);
  pio_enable_interrupt(BUT_C_PIO, BUT_C_IDX_MASK);
  pio_get_interrupt_status(BUT_C_PIO);
  NVIC_EnableIRQ(BUT_C_PIO_ID);
  NVIC_SetPriority(BUT_C_PIO_ID, 4);

  pio_configure(BUT_CS_PIO, PIO_INPUT, BUT_CS_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_CS_PIO,
                  BUT_CS_PIO_ID,
                  BUT_CS_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  but_CS_callback);
  pio_enable_interrupt(BUT_CS_PIO, BUT_CS_IDX_MASK);
  pio_get_interrupt_status(BUT_CS_PIO);
  NVIC_EnableIRQ(BUT_CS_PIO_ID);
  NVIC_SetPriority(BUT_CS_PIO_ID, 4);

  pio_configure(BUT_D_PIO, PIO_INPUT, BUT_D_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_D_PIO,
                  BUT_D_PIO_ID,
                  BUT_D_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  but_D_callback);
  pio_enable_interrupt(BUT_D_PIO, BUT_D_IDX_MASK);
  pio_get_interrupt_status(BUT_D_PIO);
  NVIC_EnableIRQ(BUT_D_PIO_ID);
  NVIC_SetPriority(BUT_D_PIO_ID, 4);

  pio_configure(BUT_DS_PIO, PIO_INPUT, BUT_DS_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_DS_PIO,
                  BUT_DS_PIO_ID,
                  BUT_DS_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  but_DS_callback);
  pio_enable_interrupt(BUT_DS_PIO, BUT_DS_IDX_MASK);
  pio_get_interrupt_status(BUT_DS_PIO);
  NVIC_EnableIRQ(BUT_DS_PIO_ID);
  NVIC_SetPriority(BUT_DS_PIO_ID, 4);

  pio_configure(BUT_E_PIO, PIO_INPUT, BUT_E_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_E_PIO,
                  BUT_E_PIO_ID,
                  BUT_E_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  but_E_callback);
  pio_enable_interrupt(BUT_E_PIO, BUT_E_IDX_MASK);
  pio_get_interrupt_status(BUT_E_PIO);
  NVIC_EnableIRQ(BUT_E_PIO_ID);
  NVIC_SetPriority(BUT_E_PIO_ID, 4);

  pio_configure(BUT_F_PIO, PIO_INPUT, BUT_F_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_F_PIO,
                  BUT_F_PIO_ID,
                  BUT_F_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  but_F_callback);
  pio_enable_interrupt(BUT_F_PIO, BUT_F_IDX_MASK);
  pio_get_interrupt_status(BUT_F_PIO);
  NVIC_EnableIRQ(BUT_F_PIO_ID);
  NVIC_SetPriority(BUT_F_PIO_ID, 4);

  pio_configure(BUT_FS_PIO, PIO_INPUT, BUT_FS_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_FS_PIO,
                  BUT_FS_PIO_ID,
                  BUT_FS_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  but_FS_callback);
  pio_enable_interrupt(BUT_FS_PIO, BUT_FS_IDX_MASK);
  pio_get_interrupt_status(BUT_FS_PIO);
  NVIC_EnableIRQ(BUT_FS_PIO_ID);
  NVIC_SetPriority(BUT_FS_PIO_ID, 4);

  pio_configure(BUT_G_PIO, PIO_INPUT, BUT_G_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_G_PIO,
                  BUT_G_PIO_ID,
                  BUT_G_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  but_G_callback);
  pio_enable_interrupt(BUT_G_PIO, BUT_G_IDX_MASK);
  pio_get_interrupt_status(BUT_G_PIO);
  NVIC_EnableIRQ(BUT_G_PIO_ID);
  NVIC_SetPriority(BUT_G_PIO_ID, 4);

  pio_configure(BUT_GS_PIO, PIO_INPUT, BUT_GS_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_GS_PIO,
                  BUT_GS_PIO_ID,
                  BUT_GS_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  but_GS_callback);
  pio_enable_interrupt(BUT_GS_PIO, BUT_GS_IDX_MASK);
  pio_get_interrupt_status(BUT_GS_PIO);
  NVIC_EnableIRQ(BUT_GS_PIO_ID);
  NVIC_SetPriority(BUT_GS_PIO_ID, 4);

  pio_configure(BUT_A_PIO, PIO_INPUT, BUT_A_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_A_PIO,
                  BUT_A_PIO_ID,
                  BUT_A_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  but_A_callback);
  pio_enable_interrupt(BUT_A_PIO, BUT_A_IDX_MASK);
  pio_get_interrupt_status(BUT_A_PIO);
  NVIC_EnableIRQ(BUT_A_PIO_ID);
  NVIC_SetPriority(BUT_A_PIO_ID, 4);

  pio_configure(BUT_AS_PIO, PIO_INPUT, BUT_AS_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_AS_PIO,
                  BUT_AS_PIO_ID,
                  BUT_AS_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  but_AS_callback);
  pio_enable_interrupt(BUT_AS_PIO, BUT_AS_IDX_MASK);
  pio_get_interrupt_status(BUT_AS_PIO);
  NVIC_EnableIRQ(BUT_AS_PIO_ID);
  NVIC_SetPriority(BUT_AS_PIO_ID, 4);

  pio_configure(BUT_B_PIO, PIO_INPUT, BUT_B_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_B_PIO,
                  BUT_B_PIO_ID,
                  BUT_B_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  but_B_callback);
  pio_enable_interrupt(BUT_B_PIO, BUT_B_IDX_MASK);
  pio_get_interrupt_status(BUT_B_PIO);
  NVIC_EnableIRQ(BUT_B_PIO_ID);
  NVIC_SetPriority(BUT_B_PIO_ID, 4);
}

uint32_t usart_puts(uint8_t *pstring) {
  uint32_t i;

  while(*(pstring + i))
  if(uart_is_tx_empty(USART_COM))
  usart_serial_putchar(USART_COM, *(pstring+i++));
}

void usart_put_string(Usart *usart, char str[]) {
  usart_serial_write_packet(usart, str, strlen(str));
}

int usart_get_string(Usart *usart, char buffer[], int bufferlen, uint timeout_ms) {
  uint timecounter = timeout_ms;
  uint32_t rx;
  uint32_t counter = 0;

  while( (timecounter > 0) && (counter < bufferlen - 1)) {
    if(usart_read(usart, &rx) == 0) {
      buffer[counter++] = rx;
    }
    else{
      timecounter--;
      vTaskDelay(1);
    }
  }
  buffer[counter] = 0x00;
  return counter;
}

void usart_send_command(Usart *usart, char buffer_rx[], int bufferlen,
char buffer_tx[], int timeout) {
  usart_put_string(usart, buffer_tx);
  usart_get_string(usart, buffer_rx, bufferlen, timeout);
}

void config_usart0(void) {
  sysclk_enable_peripheral_clock(ID_USART0);
  usart_serial_options_t config;
  config.baudrate = 9600;
  config.charlength = US_MR_CHRL_8_BIT;
  config.paritytype = US_MR_PAR_NO;
  config.stopbits = false;
  usart_serial_init(USART0, &config);
  usart_enable_tx(USART0);
  usart_enable_rx(USART0);

  // RX - PB0  TX - PB1
  pio_configure(PIOB, PIO_PERIPH_C, (1 << 0), PIO_DEFAULT);
  pio_configure(PIOB, PIO_PERIPH_C, (1 << 1), PIO_DEFAULT);
}

int hc05_init(void) {
  char buffer_rx[128];
  usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
  vTaskDelay( 500 / portTICK_PERIOD_MS);
  usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
  vTaskDelay( 500 / portTICK_PERIOD_MS);
  usart_send_command(USART_COM, buffer_rx, 1000, "AT+NAMEATecladinho", 100);
  vTaskDelay( 500 / portTICK_PERIOD_MS);
  usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
  vTaskDelay( 500 / portTICK_PERIOD_MS);
  usart_send_command(USART_COM, buffer_rx, 1000, "AT+PIN1234", 100);
  vTaskDelay( 500 / portTICK_PERIOD_MS);
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/


void task_bluetooth(void) {
  printf("Task Bluetooth started \n");
  
  printf("Inicializando HC05 \n");

  #ifndef DEBUG_SERIAL
  config_usart0();
  #endif
  
  hc05_init();
 
  // configura LEDs e Botões
  io_init();
  printf("AAAA\n");
  

  uint32_t botao = 0;

  char button = botao + '0';
  char eof = 'X';

  // Task não deve retornar.
  while(1) {
	  printf("A\n");
    // atualiza valor do botão
    if (xQueueReceive(xQueueBut, &botao, (TickType_t) 0)) {
		printf("B\n");
		button = botao + '0';
    }

    // envia status botão

    while(!usart_is_tx_ready(USART_COM)) {
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    usart_write(USART_COM, button);
    
    // envia fim de pacote
    while(!usart_is_tx_ready(USART_COM)) {
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    usart_write(USART_COM, eof);

    // dorme por 500 ms
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/


int main(void) {
	  xQueueBut = xQueueCreate(32, sizeof(uint32_t));
	  if (xQueueBut == NULL)
		printf("falha em criar a queue \n");
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Initialize the console uart */
	configure_console();

	/* Create task to make led blink */
	xTaskCreate(task_bluetooth, "BLT", TASK_BLUETOOTH_STACK_SIZE, NULL,	TASK_BLUETOOTH_STACK_PRIORITY, NULL);

	/* Start the scheduler. */
	vTaskStartScheduler();

  /* RTOS não deve chegar aqui !! */
	while(1){}

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
