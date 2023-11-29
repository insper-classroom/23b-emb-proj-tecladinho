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
#define LED1_PIO      PIOD
#define LED1_PIO_ID   ID_PIOD
#define LED1_IDX      20
#define LED1_IDX_MASK (1 << LED1_IDX)

#define LED2_PIO      PIOD
#define LED2_PIO_ID   ID_PIOD
#define LED2_IDX      22
#define LED2_IDX_MASK (1 << LED2_IDX)

// Botão
#define BUT_PIO      PIOA
#define BUT_PIO_ID   ID_PIOA
#define BUT_IDX      11
#define BUT_IDX_MASK (1 << BUT_IDX)

// Botões
// PD28 - ON/OFF
// PC13 - C
// PD25 - C#
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
// PD30 - Fader Volume

#define BUT_ON_OFF_PIO             	PIOD
#define BUT_ON_OFF_PIO_ID          	ID_PIOD
#define BUT_ON_OFF_PIO_IDX         	28
#define BUT_ON_OFF_PIO_IDX_MASK     (1 << BUT_ON_OFF_PIO_IDX)

#define BUT_C_PIO      PIOC
#define BUT_C_PIO_ID   ID_PIOC
#define BUT_C_IDX      13
#define BUT_C_IDX_MASK (1 << BUT_C_IDX)

#define BUT_CS_PIO      PIOD
#define BUT_CS_PIO_ID   ID_PIOD
#define BUT_CS_IDX      25
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

#define AFEC_POT AFEC0
#define AFEC_POT_ID ID_AFEC0
#define AFEC_POT_CHANNEL 0 // Canal do pino PD30

/** RTOS  */
#define TASK_OLED_STACK_SIZE                (1024*6/sizeof(portSTACK_TYPE))
#define TASK_OLED_STACK_PRIORITY            (tskIDLE_PRIORITY)

#define TASK_BLUETOOTH_STACK_SIZE            (4096/sizeof(portSTACK_TYPE))
#define TASK_BLUETOOTH_STACK_PRIORITY        (tskIDLE_PRIORITY)

#define TASK_HANDSHAKE_STACK_SIZE            (4096/sizeof(portSTACK_TYPE))
#define TASK_HANDSHAKE_STACK_PRIORITY        (tskIDLE_PRIORITY)

#define TASK_ADC_STACK_SIZE (1024 * 10 / sizeof(portSTACK_TYPE))
#define TASK_ADC_STACK_PRIORITY (tskIDLE_PRIORITY)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,  signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

QueueHandle_t xQueueBut;
QueueHandle_t xQueueADC;
QueueHandle_t xQueuePROC;
QueueHandle_t xQueueVolume;

TaskHandle_t xTaskHandshakeHandle;

TimerHandle_t xTimer;

typedef struct {
	uint value;
} adcData;

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
static void AFEC_callback(void);

void io_init(void);
static void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel,
                            afec_callback_t callback);

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
  if (!pio_get(BUT_C_PIO, PIO_INPUT, BUT_C_IDX_MASK) == 0) {
    char botao = 0;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_clear(LED1_PIO, LED1_IDX_MASK);
  }
  else {
    char botao = 1;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_set(LED1_PIO, LED1_IDX_MASK);
  }
}

void but_CS_callback(void) {
  if (!pio_get(BUT_CS_PIO, PIO_INPUT, BUT_CS_IDX_MASK) == 0) {
    char botao = 0;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_clear(LED1_PIO, LED1_IDX_MASK);
  }
  else {
    char botao = 2;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_set(LED1_PIO, LED1_IDX_MASK);
  }
}

void but_D_callback(void) {
  if (!pio_get(BUT_D_PIO, PIO_INPUT, BUT_D_IDX_MASK) == 0) {
    char botao = 0;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_clear(LED1_PIO, LED1_IDX_MASK);
  }
  else {
    char botao = 3;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_set(LED1_PIO, LED1_IDX_MASK);
  }
}

void but_DS_callback(void) {
  if (!pio_get(BUT_DS_PIO, PIO_INPUT, BUT_DS_IDX_MASK) == 0) {
    char botao = 0;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_clear(LED1_PIO, LED1_IDX_MASK);
  }
  else {
    char botao = 4;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_set(LED1_PIO, LED1_IDX_MASK);
  }
}

void but_E_callback(void) {
  if (!pio_get(BUT_E_PIO, PIO_INPUT, BUT_E_IDX_MASK) == 0) {
    char botao = 0;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_clear(LED1_PIO, LED1_IDX_MASK);
  }
  else {
    char botao = 5;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_set(LED1_PIO, LED1_IDX_MASK);
  }
}

void but_F_callback(void) {
  if (!pio_get(BUT_F_PIO, PIO_INPUT, BUT_F_IDX_MASK) == 0) {
    char botao = 0;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_clear(LED1_PIO, LED1_IDX_MASK);
  }
  else {
    char botao = 6;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_set(LED1_PIO, LED1_IDX_MASK);
  }
}

void but_FS_callback(void) {
  if (!pio_get(BUT_FS_PIO, PIO_INPUT, BUT_FS_IDX_MASK) == 0) {
    char botao = 0;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_clear(LED1_PIO, LED1_IDX_MASK);
  }
  else {
    char botao = 7;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
  }
}

void but_G_callback(void) {
  if (!pio_get(BUT_G_PIO, PIO_INPUT, BUT_G_IDX_MASK) == 0) {
    char botao = 0;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_clear(LED1_PIO, LED1_IDX_MASK);
  }
  else {
    char botao = 8;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_set(LED1_PIO, LED1_IDX_MASK);
  }
}

void but_GS_callback(void) {
  if (!pio_get(BUT_GS_PIO, PIO_INPUT, BUT_GS_IDX_MASK) == 0) {
    char botao = 0;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_clear(LED1_PIO, LED1_IDX_MASK);
  }
  else {
    char botao = 9;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_set(LED1_PIO, LED1_IDX_MASK);
  }
}

void but_A_callback(void) {
  if (!pio_get(BUT_A_PIO, PIO_INPUT, BUT_A_IDX_MASK) == 0) {
    char botao = 0;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_clear(LED1_PIO, LED1_IDX_MASK);
  }
  else {
    char botao = 10;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_set(LED1_PIO, LED1_IDX_MASK);
  }
}

void but_AS_callback(void) {
  if (!pio_get(BUT_AS_PIO, PIO_INPUT, BUT_AS_IDX_MASK) == 0) {
    char botao = 0;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_clear(LED1_PIO, LED1_IDX_MASK);
  }
  else {
    char botao = 11;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_set(LED1_PIO, LED1_IDX_MASK);
  }
}

void but_B_callback(void) {
  if (!pio_get(BUT_B_PIO, PIO_INPUT, BUT_B_IDX_MASK) == 0) {
    char botao = 0;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_clear(LED1_PIO, LED1_IDX_MASK);
  }
  else {
    char botao = 12;
    xQueueSendFromISR(xQueueBut, (void *)&botao, 0);
    pio_set(LED1_PIO, LED1_IDX_MASK);
  }
}

static void AFEC_pot_callback(void) {
	adcData adc;
	adc.value = afec_channel_get_value(AFEC_POT, AFEC_POT_CHANNEL);
	BaseType_t xHigherPriorityTaskWoken = pdTRUE;
	xQueueSendFromISR(xQueueADC, &adc, &xHigherPriorityTaskWoken);
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

void vTimerCallback(TimerHandle_t xTimer) {
  /* Selecina canal e inicializa conversão */
  afec_channel_enable(AFEC_POT, AFEC_POT_CHANNEL);
  afec_start_software_conversion(AFEC_POT);
}

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

  pmc_enable_periph_clk(ID_PIOA);
  pmc_enable_periph_clk(ID_PIOB);
  pmc_enable_periph_clk(ID_PIOC);
  pmc_enable_periph_clk(ID_PIOD);

  // Configura Pinos
  pio_configure(LED1_PIO, PIO_OUTPUT_0, LED1_IDX_MASK, PIO_DEFAULT | PIO_DEBOUNCE);
  pio_configure(LED2_PIO, PIO_OUTPUT_0, LED2_IDX_MASK, PIO_DEFAULT | PIO_DEBOUNCE);

  pio_configure(BUT_C_PIO, PIO_INPUT, BUT_C_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_C_PIO,
                  BUT_C_PIO_ID,
                  BUT_C_IDX_MASK,
                  PIO_IT_EDGE,
                  but_C_callback);
  pio_enable_interrupt(BUT_C_PIO, BUT_C_IDX_MASK);
  pio_get_interrupt_status(BUT_C_PIO);
  NVIC_EnableIRQ(BUT_C_PIO_ID);
  NVIC_SetPriority(BUT_C_PIO_ID, 4);

  pio_configure(BUT_CS_PIO, PIO_INPUT, BUT_CS_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_CS_PIO,
                  BUT_CS_PIO_ID,
                  BUT_CS_IDX_MASK,
                  PIO_IT_EDGE,
                  but_CS_callback);
  pio_enable_interrupt(BUT_CS_PIO, BUT_CS_IDX_MASK);
  pio_get_interrupt_status(BUT_CS_PIO);
  NVIC_EnableIRQ(BUT_CS_PIO_ID);
  NVIC_SetPriority(BUT_CS_PIO_ID, 4);

  pio_configure(BUT_D_PIO, PIO_INPUT, BUT_D_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_D_PIO,
                  BUT_D_PIO_ID,
                  BUT_D_IDX_MASK,
                  PIO_IT_EDGE,
                  but_D_callback);
  pio_enable_interrupt(BUT_D_PIO, BUT_D_IDX_MASK);
  pio_get_interrupt_status(BUT_D_PIO);
  NVIC_EnableIRQ(BUT_D_PIO_ID);
  NVIC_SetPriority(BUT_D_PIO_ID, 4);

  pio_configure(BUT_DS_PIO, PIO_INPUT, BUT_DS_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_DS_PIO,
                  BUT_DS_PIO_ID,
                  BUT_DS_IDX_MASK,
                  PIO_IT_EDGE,
                  but_DS_callback);
  pio_enable_interrupt(BUT_DS_PIO, BUT_DS_IDX_MASK);
  pio_get_interrupt_status(BUT_DS_PIO);
  NVIC_EnableIRQ(BUT_DS_PIO_ID);
  NVIC_SetPriority(BUT_DS_PIO_ID, 4);

  pio_configure(BUT_E_PIO, PIO_INPUT, BUT_E_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_E_PIO,
                  BUT_E_PIO_ID,
                  BUT_E_IDX_MASK,
                  PIO_IT_EDGE,
                  but_E_callback);
  pio_enable_interrupt(BUT_E_PIO, BUT_E_IDX_MASK);
  pio_get_interrupt_status(BUT_E_PIO);
  NVIC_EnableIRQ(BUT_E_PIO_ID);
  NVIC_SetPriority(BUT_E_PIO_ID, 4);

  pio_configure(BUT_F_PIO, PIO_INPUT, BUT_F_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_F_PIO,
                  BUT_F_PIO_ID,
                  BUT_F_IDX_MASK,
                  PIO_IT_EDGE,
                  but_F_callback);
  pio_enable_interrupt(BUT_F_PIO, BUT_F_IDX_MASK);
  pio_get_interrupt_status(BUT_F_PIO);
  NVIC_EnableIRQ(BUT_F_PIO_ID);
  NVIC_SetPriority(BUT_F_PIO_ID, 4);

  pio_configure(BUT_FS_PIO, PIO_INPUT, BUT_FS_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_FS_PIO,
                  BUT_FS_PIO_ID,
                  BUT_FS_IDX_MASK,
                  PIO_IT_EDGE,
                  but_FS_callback);
  pio_enable_interrupt(BUT_FS_PIO, BUT_FS_IDX_MASK);
  pio_get_interrupt_status(BUT_FS_PIO);
  NVIC_EnableIRQ(BUT_FS_PIO_ID);
  NVIC_SetPriority(BUT_FS_PIO_ID, 4);

  pio_configure(BUT_G_PIO, PIO_INPUT, BUT_G_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_G_PIO,
                  BUT_G_PIO_ID,
                  BUT_G_IDX_MASK,
                  PIO_IT_EDGE,
                  but_G_callback);
  pio_enable_interrupt(BUT_G_PIO, BUT_G_IDX_MASK);
  pio_get_interrupt_status(BUT_G_PIO);
  NVIC_EnableIRQ(BUT_G_PIO_ID);
  NVIC_SetPriority(BUT_G_PIO_ID, 4);

  pio_configure(BUT_GS_PIO, PIO_INPUT, BUT_GS_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_GS_PIO,
                  BUT_GS_PIO_ID,
                  BUT_GS_IDX_MASK,
                  PIO_IT_EDGE,
                  but_GS_callback);
  pio_enable_interrupt(BUT_GS_PIO, BUT_GS_IDX_MASK);
  pio_get_interrupt_status(BUT_GS_PIO);
  NVIC_EnableIRQ(BUT_GS_PIO_ID);
  NVIC_SetPriority(BUT_GS_PIO_ID, 4);

  pio_configure(BUT_A_PIO, PIO_INPUT, BUT_A_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_A_PIO,
                  BUT_A_PIO_ID,
                  BUT_A_IDX_MASK,
                  PIO_IT_EDGE,
                  but_A_callback);
  pio_enable_interrupt(BUT_A_PIO, BUT_A_IDX_MASK);
  pio_get_interrupt_status(BUT_A_PIO);
  NVIC_EnableIRQ(BUT_A_PIO_ID);
  NVIC_SetPriority(BUT_A_PIO_ID, 4);

  pio_configure(BUT_AS_PIO, PIO_INPUT, BUT_AS_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_AS_PIO,
                  BUT_AS_PIO_ID,
                  BUT_AS_IDX_MASK,
                  PIO_IT_EDGE,
                  but_AS_callback);
  pio_enable_interrupt(BUT_AS_PIO, BUT_AS_IDX_MASK);
  pio_get_interrupt_status(BUT_AS_PIO);
  NVIC_EnableIRQ(BUT_AS_PIO_ID);
  NVIC_SetPriority(BUT_AS_PIO_ID, 4);

  pio_configure(BUT_B_PIO, PIO_INPUT, BUT_B_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_handler_set(BUT_B_PIO,
                  BUT_B_PIO_ID,
                  BUT_B_IDX_MASK,
                  PIO_IT_EDGE,
                  but_B_callback);
  pio_enable_interrupt(BUT_B_PIO, BUT_B_IDX_MASK);
  pio_get_interrupt_status(BUT_B_PIO);
  NVIC_EnableIRQ(BUT_B_PIO_ID);
  NVIC_SetPriority(BUT_B_PIO_ID, 4);
}

static void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel,
                            afec_callback_t callback) {
  /*************************************
   * Ativa e configura AFEC
   *************************************/
  /* Ativa AFEC - 0 */
  afec_enable(afec);

  /* struct de configuracao do AFEC */
  struct afec_config afec_cfg;

  /* Carrega parametros padrao */
  afec_get_config_defaults(&afec_cfg);

  /* Configura AFEC */
  afec_init(afec, &afec_cfg);

  /* Configura trigger por software */
  afec_set_trigger(afec, AFEC_TRIG_SW);

  /*** Configuracao específica do canal AFEC ***/
  struct afec_ch_config afec_ch_cfg;
  afec_ch_get_config_defaults(&afec_ch_cfg);
  afec_ch_cfg.gain = AFEC_GAINVALUE_0;
  afec_ch_set_config(afec, afec_channel, &afec_ch_cfg);

  /*
  * Calibracao:
  * Because the internal ADC offset is 0x200, it should cancel it and shift
  down to 0.
  */
  afec_channel_set_analog_offset(afec, afec_channel, 0x200);

  /***  Configura sensor de temperatura ***/
  struct afec_temp_sensor_config afec_temp_sensor_cfg;

  afec_temp_sensor_get_config_defaults(&afec_temp_sensor_cfg);
  afec_temp_sensor_set_config(afec, &afec_temp_sensor_cfg);

  /* configura IRQ */
  afec_set_callback(afec, afec_channel, callback, 1);
  NVIC_SetPriority(afec_id, 4);
  NVIC_EnableIRQ(afec_id);
}

uint32_t usart_puts(uint8_t *pstring) {
  uint32_t i = 0;

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

static void task_adc(void *pvParameters) {

  // configura ADC e TC para controlar a leitura
  config_AFEC_pot(AFEC_POT, AFEC_POT_ID, AFEC_POT_CHANNEL, AFEC_pot_callback);

  xTimer = xTimerCreate(/* Just a text name, not used by the RTOS
                        kernel. */
                        "Timer",
                        /* The timer period in ticks, must be
                        greater than 0. */
                        100,
                        /* The timers will auto-reload themselves
                        when they expire. */
                        pdTRUE,
                        /* The ID is used to store a count of the
                        number of times the timer has expired, which
                        is initialised to 0. */
                        (void *)0,
                        /* Timer callback */
                        vTimerCallback);
  xTimerStart(xTimer, 0);

  // variável para recever dados da fila
  adcData adc;

  while (1) {
    if (xQueueReceive(xQueuePROC, &adc, portMAX_DELAY) == pdTRUE) {
	    printf("ADC: %d\n", adc.value);
		xQueueSend(xQueueVolume, &adc.value, 0);
    } 
	else {
      printf("Nao chegou um novo dado em 1 segundo");
    }
  }
}

static void task_proc(void *pvParameters) {
	adcData adc;
	int lista_adc[10] = {0};
	int soma = 0;
	int media_movel = 0;
	int media_anterior = 0;
	int i = 0;

	for (;;) {
		if (xQueueReceive(xQueueADC, &adc, portMAX_DELAY) == pdTRUE) {
			soma -= lista_adc[i];
			lista_adc[i] = adc.value;
			soma += lista_adc[i];
			
			media_movel = soma / 10;
			
			if (abs(media_movel - media_anterior) > 50){
				
				printf("media: %d\n", media_movel);
				xQueueSend(xQueueVolume, &media_movel, 0);
				media_anterior = media_movel;
			}
			
			// i volta pra 0 quando for 10
			i = (i + 1) % 10;
		}
	}
}

void task_bluetooth(void) {
  printf("Task Bluetooth started \n");
  
  printf("Inicializando HC05 \n");

  vTaskSuspend(xTaskHandshakeHandle);

  uint32_t adc = 0;
  uint32_t botao = 0;
  char eof = 'X';

  // Task não deve retornar.
  while(1) {
    // atualiza valor do botão
    if ((xQueueReceive(xQueueBut, &botao, (TickType_t) 0)) || (xQueueReceive(xQueueVolume, &adc, (TickType_t) 0))) {
      char botao_char = botao + '0';
      char adc_char = adc + '0';
    
		    while(!usart_is_tx_ready(USART_COM)) {
			    vTaskDelay(10 / portTICK_PERIOD_MS);
		    }
		    usart_write(USART_COM, botao_char);

			while(!usart_is_tx_ready(USART_COM)) {
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			usart_write(USART_COM, adc_char);
		    
		    // envia fim de pacote
		    while(!usart_is_tx_ready(USART_COM)) {
			    vTaskDelay(10 / portTICK_PERIOD_MS);
		    }
		    usart_write(USART_COM, eof);

		    // dorme por 500 ms
		    vTaskDelay(50 / portTICK_PERIOD_MS);
	    }
    }
}

void task_handshake(void) {
  #ifndef DEBUG_SERIAL
  config_usart0();
  #endif
  
  hc05_init();
 
  io_init();

  char handshake;
  while (1) {
    if (usart_read(USART_COM, &handshake) == 0) {
      if (handshake == 'H') {
        pio_set(LED2_PIO, LED2_IDX_MASK);
        xTaskCreate(task_bluetooth, "BT", TASK_BLUETOOTH_STACK_SIZE, NULL, TASK_BLUETOOTH_STACK_PRIORITY, NULL);
      }
    }
  }

}


/************************************************************************/
/* main                                                                 */
/************************************************************************/


int main(void) {
  pio_clear(LED1_PIO, LED1_IDX_MASK);
  pio_clear(LED2_PIO, LED2_IDX_MASK);

  xQueueBut = xQueueCreate(32, sizeof(uint32_t));
  if (xQueueBut == NULL)
  printf("falha em criar a queue \n");
  
  xQueueADC = xQueueCreate(32, sizeof(uint32_t));
  if (xQueueADC == NULL)
    printf("falha em criar a queue \n");
	
  if (xTaskCreate(task_adc, "ADC", TASK_ADC_STACK_SIZE, NULL,
  TASK_ADC_STACK_PRIORITY, NULL) != pdPASS) {
	  printf("Failed to create test ADC task\r\n");
  }
	
  xQueuePROC = xQueueCreate(100, sizeof(adcData));
  if (xQueuePROC == NULL)
  printf("falha em criar a queue xQueuePROC \n");

  xQueueVolume = xQueueCreate(100, sizeof(adcData));
  if (xQueueVolume == NULL)
  printf("falha em criar a queue xQueueVolume \n");
	
  if (xTaskCreate(task_proc, "PROC", TASK_ADC_STACK_SIZE, NULL,
  TASK_ADC_STACK_PRIORITY, NULL) != pdPASS) {
	  printf("Failed to create test PROC task\r\n");
  }
	/* Initialize the SAM system */
	sysclk_init();
	board_init();
	
	/* Initialize the console uart */
	configure_console();

	/* Create task to make led blink */
  xTaskCreate(task_handshake, "Handshake", TASK_HANDSHAKE_STACK_SIZE, NULL,	TASK_HANDSHAKE_STACK_PRIORITY, &xTaskHandshakeHandle);

	/* Start the scheduler. */
	vTaskStartScheduler();

  /* RTOS não deve chegar aqui !! */
	while(1){	}

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
