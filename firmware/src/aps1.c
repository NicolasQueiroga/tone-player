#include <string.h>
#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"
#include "aps1.h"

volatile _Bool but1_flag = 0;
volatile _Bool but2_flag = 0;
volatile _Bool but3_flag = 0;

void but1_callback(void)
{
	but1_flag = 1;
}

void but2_callback(void)
{
	but2_flag = 1;
}

void but3_callback(void)
{
	but3_flag = 1;
}

_Bool get_but1_flag()
{
	return but1_flag;
}

_Bool get_but2_flag()
{
	return but2_flag;
}

_Bool get_but3_flag()
{
	return but3_flag;
}

void set_buzzer()
{
	pio_set(BUZ_PIO, BUZ_PIO_IDX_MASK);
}

void clear_buzzer()
{
	pio_clear(BUZ_PIO, BUZ_PIO_IDX_MASK);
	pio_clear(LED_PIO, LED_PIO_IDX_MASK);
}

void go_forward(Disc *disc)
{
	int curr_i = disc->song_counter;
	if (curr_i != 0)
		(disc->song_counter)--;
	but3_flag = 0;
}

void go_backward(Disc *disc)
{
	int curr_i = disc->song_counter;
	if (curr_i != 2)
		(disc->song_counter)++;
	but1_flag = 0;
}

// void init_song(Song *s, char *title, int tempo, int *melody)
// {
// 	strcpy(s->title, title);
// 	s->tempo = tempo;
// 	s->size = sizeof(melody);
// 	s->melody = melody;
// }

void tone(int freq, int time, Disc *disc)
{
	int delay = 1e6 / freq;
	for (int i = 0; i < 1000 * time / delay; i++)
	{
		set_buzzer();
		delay_us(delay / 2);
		clear_buzzer();
		delay_us(delay / 2);

		// backward
		if (but1_flag)
		{
			pio_clear(LED1_PIO, LED1_PIO_IDX_MASK);
			go_backward(disc);
			but1_flag = 1;
			return;
		}

		// forward
		else if (but3_flag)
		{
			pio_clear(LED3_PIO, LED3_PIO_IDX_MASK);
			go_forward(disc);
			but3_flag = 1;
			return;
		}

		// pause
		else if (but2_flag)
		{
			but2_flag = 0;
			pio_set(LED_PIO, LED_PIO_IDX_MASK);
			pio_clear(LED2_PIO, LED2_PIO_IDX_MASK);

			pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);

			if (but1_flag)
			{
				go_backward(disc);
				but1_flag = 1;
				return;
			}
			if (but3_flag)
			{
				go_forward(disc);
				but3_flag = 1;
				return;
			}
			but2_flag = 0;
			pio_set(LED2_PIO, LED2_PIO_IDX_MASK);
		}
	}
}

void play(Disc *disc)
{
	but2_flag = 0;

	Song s = disc->songs[disc->song_counter];
	draw_name(s.title);
	int melody_size = s.size;
	int melody[melody_size];
	memcpy(melody, s.melody, melody_size);
	int tempo = s.tempo;
	int notes = sizeof(melody) / sizeof(melody[0]) / 4;
	int wholenote = (60000 * 4) / tempo;
	int divider = 0, noteDuration = 0;

	int x = 0;
	double x_double = 0;

	clear_screen();
	gfx_mono_generic_draw_filled_rect(X_MIN + 1, Y_MIN + 1, X_MAX + 1, Y_MAX + 1, GFX_PIXEL_CLR);
	gfx_mono_generic_draw_rect(X_MIN, Y_MIN, X_MAX, Y_MAX, GFX_PIXEL_SET);

	for (int thisNote = 0; thisNote < notes; thisNote += 2)
	{
		x_double += 127 / ((double)notes) * 2;
		x = x_double;
		gfx_mono_generic_draw_filled_rect(X_MIN, Y_MIN, x, Y_MAX, GFX_PIXEL_SET);
		divider = melody[thisNote + 1];

		if (divider > 0)
			noteDuration = wholenote / divider;
		else if (divider < 0)
			noteDuration = (wholenote) / abs(divider) * 1.5;

		tone(melody[thisNote], noteDuration * 0.9, disc);

		if (but1_flag || but3_flag)
		{
			delay_ms(noteDuration * 0.1);
			pio_set(LED1_PIO, LED1_PIO_IDX_MASK);
			pio_set(LED3_PIO, LED3_PIO_IDX_MASK);
			but1_flag = 0;
			but3_flag = 0;
			return;
		}

		pio_set(LED_PIO, LED_PIO_IDX_MASK);
		delay_ms(noteDuration * 0.1);
	}
}

void clear_screen()
{
	gfx_mono_draw_string("             ", 0, 16, &sysfont);
}

void draw_name(char *name)
{
	clear_screen();
	gfx_mono_draw_string(name, 0, 16, &sysfont);
}

void init(void)
{
	// setup
	board_init();
	sysclk_init();
	delay_init();
	gfx_mono_ssd1306_init();
	WDT->WDT_MR = WDT_MR_WDDIS;

	// enable clocks
	pmc_enable_periph_clk(ID_PIOA);
	pmc_enable_periph_clk(ID_PIOB);
	pmc_enable_periph_clk(ID_PIOC);
	pmc_enable_periph_clk(ID_PIOD);

	// btn1
	pio_configure(BUT_BACKWARD_PIO, PIO_INPUT, BUT_BACKWARD_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_BACKWARD_PIO, BUT_BACKWARD_PIO_IDX_MASK, 60);
	pio_handler_set(BUT_BACKWARD_PIO,
					BUT_BACKWARD_PIO_ID,
					BUT_BACKWARD_PIO_IDX_MASK,
					PIO_IT_FALL_EDGE,
					but1_callback);
	pio_enable_interrupt(BUT_BACKWARD_PIO, BUT_BACKWARD_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT_BACKWARD_PIO);
	NVIC_EnableIRQ(BUT_BACKWARD_PIO_ID);
	NVIC_SetPriority(BUT_BACKWARD_PIO_ID, 4);

	// btn2
	pio_configure(BUT_START_STOP_PIO, PIO_INPUT, BUT_START_STOP_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_START_STOP_PIO, BUT_START_STOP_PIO_IDX_MASK, 60);
	pio_handler_set(BUT_START_STOP_PIO,
					BUT_START_STOP_PIO_ID,
					BUT_START_STOP_PIO_IDX_MASK,
					PIO_IT_FALL_EDGE,
					but2_callback);
	pio_enable_interrupt(BUT_START_STOP_PIO, BUT_START_STOP_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT_START_STOP_PIO);
	NVIC_EnableIRQ(BUT_START_STOP_PIO_ID);
	NVIC_SetPriority(BUT_START_STOP_PIO_ID, 4);

	// btn3
	pio_configure(BUT_FORWARD_PIO, PIO_INPUT, BUT_FORWARD_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_FORWARD_PIO, BUT_FORWARD_PIO_IDX_MASK, 60);
	pio_handler_set(BUT_FORWARD_PIO,
					BUT_FORWARD_PIO_ID,
					BUT_FORWARD_PIO_IDX_MASK,
					PIO_IT_FALL_EDGE,
					but3_callback);
	pio_enable_interrupt(BUT_FORWARD_PIO, BUT_FORWARD_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT_FORWARD_PIO);
	NVIC_EnableIRQ(BUT_FORWARD_PIO_ID);
	NVIC_SetPriority(BUT_FORWARD_PIO_ID, 4);

	pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 0, 0, 0);
	pio_set_output(LED1_PIO, LED1_PIO_IDX_MASK, 0, 0, 0);
	pio_set_output(LED2_PIO, LED2_PIO_IDX_MASK, 0, 0, 0);
	pio_set_output(LED3_PIO, LED3_PIO_IDX_MASK, 0, 0, 0);
	pio_set_output(BUZ_PIO, BUZ_PIO_IDX_MASK, 0, 0, 0);

	pio_set(LED_PIO, LED_PIO_IDX_MASK);
	pio_set(LED1_PIO, LED1_PIO_IDX_MASK);
	pio_set(LED2_PIO, LED2_PIO_IDX_MASK);
	pio_set(LED3_PIO, LED3_PIO_IDX_MASK);
}
