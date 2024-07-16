#include <stdint.h>
#include <stdio.h>
#define REG8(addr) (*(volatile uint8_t *)(addr))
#define REG32(addr) (*(volatile uint32_t *)(addr))
#define CONFIG_SERIAL_MMIO 0x10000000
int _write (int fd __attribute__((__unused__)), char *ptr, int len)
{
  for (int i = 0; i < len; i++)
	REG8(CONFIG_SERIAL_MMIO) = ptr[i];
  return len;
}
int main()
{
	printf("Hello, World??\n");
	return 0;
}