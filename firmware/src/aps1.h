#ifndef __APS1_H_
#define __APS1_H_

#include "asf.h"

#define X_MIN 1
#define Y_MIN 16
#define X_MAX 127
#define Y_MAX 10

#define LED_PIO PIOC
#define LED_PIO_ID ID_PIOC
#define LED_PIO_IDX 8
#define LED_PIO_IDX_MASK (1u << LED_PIO_IDX)

#define LED1_PIO PIOA
#define LED1_PIO_ID ID_PIOA
#define LED1_PIO_IDX 0
#define LED1_PIO_IDX_MASK (1u << LED1_PIO_IDX)

#define LED2_PIO PIOC
#define LED2_PIO_ID ID_PIOC
#define LED2_PIO_IDX 30
#define LED2_PIO_IDX_MASK (1u << LED2_PIO_IDX)

#define LED3_PIO PIOB
#define LED3_PIO_ID ID_PIOB
#define LED3_PIO_IDX 2
#define LED3_PIO_IDX_MASK (1u << LED3_PIO_IDX)

#define BUT_BACKWARD_PIO PIOD
#define BUT_BACKWARD_PIO_ID ID_PIOD
#define BUT_BACKWARD_PIO_IDX 28
#define BUT_BACKWARD_PIO_IDX_MASK (1u << BUT_BACKWARD_PIO_IDX)

#define BUT_START_STOP_PIO PIOC
#define BUT_START_STOP_PIO_ID ID_PIOC
#define BUT_START_STOP_PIO_IDX 31
#define BUT_START_STOP_PIO_IDX_MASK (1u << BUT_START_STOP_PIO_IDX)

#define BUT_FORWARD_PIO PIOA
#define BUT_FORWARD_PIO_ID ID_PIOA
#define BUT_FORWARD_PIO_IDX 19
#define BUT_FORWARD_PIO_IDX_MASK (1u << BUT_FORWARD_PIO_IDX)

#define BUZ_PIO PIOD
#define BUZ_PIO_ID ID_PIOD
#define BUZ_PIO_IDX 30
#define BUZ_PIO_IDX_MASK (1u << BUZ_PIO_IDX)

typedef struct
{
    int *melody;
    int tempo;
    char title[30];
    int size;
} Song;

typedef struct
{
    Song songs[3];
    int song_counter;
} Disc;

void but1_callback(void);
void but2_callback(void);
void but3_callback(void);
_Bool get_but1_flag();
_Bool get_but2_flag();
_Bool get_but3_flag();

void set_buzzer();
void clear_buzzer();
void go_forward(Disc *disc);
void go_backward(Disc *disc);

// void init_song(Song *s, char *title, int tempo, int *melody);
void tone(int freq, int time, Disc *disc);
void play(Disc *disc);
void clear_screen();
void draw_name(char *name);
void init(void);

#endif