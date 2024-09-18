

void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
unsigned char USART_Receive(void);
int uart_putchar(char c, FILE *stream);  //help function for printf_init
int uart_getchar(FILE *stream);			//help function for printf_init
void printf_init()