#include <util/delay.h>
#include <avr/io.h>

#ifndef __one_wire_h_included__
#define __one_wire_h_included__

#define ONEWIRE_PORT PORTC
#define ONEWIRE_DDR DDRC
#define ONEWIRE_PIN PINC
#define ONEWIRE_PIN_NUM PC2


// Устанавливает низкий уровень на шине 1-wire
inline void onewire_low(void) 
{
  ONEWIRE_DDR |= _BV(ONEWIRE_PIN_NUM);
}

// "Отпускает" шину 1-wire
inline void onewire_high(void) 
{
  ONEWIRE_DDR &= ~_BV(ONEWIRE_PIN_NUM);
}

// Читает значение уровня на шине 1-wire
inline uint8_t onewire_level(void) 
{
  return ONEWIRE_PIN & _BV(ONEWIRE_PIN_NUM);
}




// Переменные для хранения промежуточного результата поиска
uint8_t onewire_enum[8]; // найденный восьмибайтовый адрес 
uint8_t onewire_enum_fork_bit; // последний нулевой бит, где была неоднозначность (нумеруя с единицы) 

uint8_t onewire_reset(void);
void onewire_send_bit(uint8_t bit);
void onewire_send(uint8_t b);
uint8_t onewire_read_bit(void);
uint8_t onewire_read(void);
uint8_t onewire_crc_update(uint8_t crc, uint8_t b);
uint8_t onewire_skip(void);
uint8_t onewire_read_rom(uint8_t * buf);
uint8_t onewire_match(uint8_t * data);
void onewire_enum_init(void);
uint8_t * onewire_enum_next(void);
uint8_t onewire_match_last(void);

static inline void  OneWireInit(void)
{
	onewire_enum_init();
	ONEWIRE_PORT &= ~_BV(ONEWIRE_PIN_NUM);
	onewire_high();
}



#endif /* __one_wire_h_included__ */