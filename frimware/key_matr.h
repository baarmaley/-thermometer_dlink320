/* MATR Keyboard 2 х 3 */
#define PORT_MATR PORTB
#define DDR_MATR DDRB
#define PIN_MATR PINB

/* COLUMN */
#define MATR_COLUMN 2 // Столбцов
#define MC_1 1
#define MC_2 2

/* ROW */
#define MATR_ROW 3 // Строк
#define MR_1 3
#define MR_2 4
#define MR_3 5

/* MATR MACRO */
#define SET_DDR (0<<MC_1) | (0<<MC_2) | (0<<MR_1) | (0<<MR_2) | (0<<MR_3)

/* MATR STRUCT */
struct Matrix{
	uint8_t Column[MATR_COLUMN];
	uint8_t Row[MATR_ROW];
	};

struct Matrix Matr = { {MC_1, MC_2}, {MR_1, MR_2, MR_3} };

/* MATR TIMER */
#define START_TIMER  (1<<CS02|0<<CS01|1<<CS00)
#define STOP_TIMER (0<<CS02|0<<CS01|0<<CS00)
/* MATR MASK*/

uint8_t Matr_mask[3][3] = {
	{ 0xE0, 0xB0, 0x00 }, // кнопки: 1-3-5
	{ 0xD0, 0x70, 0xF0 }, // кнопки: 2-4-6
	{ 0x00, 0x00, 0 }
};
