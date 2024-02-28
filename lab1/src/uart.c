#include "uart.h"

void uart_init()
{
	// Configure GPIO pins
	register unsigned int r = *GPFSEL1;
	r &= ~((7 << 12) | (7 << 15));
	r |= (2 << 12) | (2 << 15);
	*GPFSEL1 = r;

	// Disable GPIO pull up/down
	*GPPUD = 0;
	for (int i = 0; i < 150; i++)
		asm volatile("nop");
	*GPPUDCLK0 = (1 << 14) | (1 << 15);
	for (int i = 0; i < 150; i++)
		asm volatile("nop");
	*GPPUD = 0;
	*GPPUDCLK0 = 0;

	// Initialize mini UART
	*AUX_ENABLE |= 1;   // Enable mini UART
	*AUX_MU_CNTL = 0;   // Disable Tx and Rx during setup
	*AUX_MU_IER = 0;    // Disable interrupt
	*AUX_MU_LCR = 3;    // Set data size to 8 bits
	*AUX_MU_MCR = 0;    // Disable auto flow control
	*AUX_MU_BAUD = 270; // Set baud rate to 115200
	*AUX_MU_IIR = 6;    // No FIFO
	*AUX_MU_CNTL = 3;   // Enable Tx and Rx
}

char uart_getc()
{
	// Check the data ready field on bit 0 of AUX_MU_LSR_REG
	do {
		asm volatile("nop");
	} while (!(*AUX_MU_LSR & 0x01));
	char c = (char)(*AUX_MU_IO); // Read from AUX_MU_IO_REG
	return c == '\r' ? '\n' : c;
}

void uart_putc(char c)
{
	// Check the transmitter empty field on bit 5 of AUX_MU_LSR_REG
	do {
		asm volatile("nop");
	} while (!(*AUX_MU_LSR & 0x20));
	*AUX_MU_IO = c; // Write to AUX_MU_IO_REG
}

void uart_hex(unsigned int h)
{
	unsigned int n;
	int c;
	for (c = 28; c >= 0; c -= 4) {
		n = (h >> c) & 0xf;
		n += n > 9 ? 0x37 : '0';
		uart_putc(n);
	}
}

void uart_puts(const char *s)
{
	while (*s) {
		// Convert '\n' to '\r\n'
		if (*s == '\n')
			uart_putc('\r');
		uart_putc(*s++);
	}
}