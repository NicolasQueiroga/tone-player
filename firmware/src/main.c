#include <string.h>
#include "aps1.h"
#include "tetris.h"
#include "cannonind.h"
#include "starwars.h"

int main(void)
{
	init();
	Song tetris, starwars, cannonind;

	// init_song(&mario, tetris_title, tetris_tempo, tetris_melody);
	// init_song(&starwars, starwars_title, starwars_tempo, starwars_melody);
	// init_song(&cannonind, cannonind_title, cannonind_tempo, &cannonind_melody);

	strcpy(tetris.title, tetris_title);
	tetris.tempo = tetris_tempo;
	tetris.melody = &tetris_melody;
	tetris.size = sizeof(tetris_melody);

	strcpy(starwars.title, starwars_title);
	starwars.tempo = starwars_tempo;
	starwars.melody = &starwars_melody;
	starwars.size = sizeof(starwars_melody);

	strcpy(cannonind.title, cannonind_title);
	cannonind.tempo = cannonind_tempo;
	cannonind.melody = &cannonind_melody;
	cannonind.size = sizeof(cannonind_melody);

	Disc disc = {{tetris, cannonind, starwars}, 1};
	draw_name(disc.songs[disc.song_counter].title);

	while (1)
	{
		if (get_but2_flag())
			play(&disc);
		if (get_but1_flag())
			go_backward(&disc);
		if (get_but3_flag())
			go_forward(&disc);

		draw_name(disc.songs[disc.song_counter].title);

		delay_ms(100);
		pio_set(LED_PIO, LED_PIO_IDX_MASK);
		pio_set(LED1_PIO, LED1_PIO_IDX_MASK);
		pio_set(LED2_PIO, LED2_PIO_IDX_MASK);
		pio_set(LED3_PIO, LED3_PIO_IDX_MASK);
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}

	return 0;
}