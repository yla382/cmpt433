#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

typedef enum {
	DRUM_DB_HARD,
    DRUM_CYN_HARD,
    DRUM_SNARE_HARD,
    DRUM_TOM_HI_HARD,
    DRUM_SNARE_SOFT,
    DRUM_TOM_HI_SOFT,
} Instrument_Sound;


/*
Initialize audio play using audioMixer_template
input: void
output: void
*/
void start_audio();


/*
End audio play using audioMixer_template
input: void
output: void
*/
void quit_audio();


/*
Return current status of tempo
input: void
output: int
*/
int getTempo();


/*
Copy audio files to memory
input: void
output: void
*/
void initialize_audio_files();


/*
Remove audio files from memory
input: void
output: void
*/
void remove_audio_files();


/*
Adds wavedata to buffer to play rock beat 1
input: void
output: void
*/
void standardRockBeat();


/*
Adds wavedata to buffer to play rock beat 2
input: void
output: void
*/
void customDrumBeat();


/*
Play individual instrument
input: enum
output: void
*/
void queue_sound(Instrument_Sound sound);


/*
Increase tempo by 5 is input is true else descrease tempo by 5
if tempo + 5 is > MAX_TEMPO then set tempo to MAX_TEMPO
if tempo - 5 is < MIN_TEMPO then set tempo to MIN_TEMPO
input: bool
output: void
*/
void changeTempo(bool increase);


/*
Increase volumn by 5 is input is true else descrease volumn by 5
if volumn + 5 is > MAX_VOLUMN then set volumn to MAX_VOLUMN
if volumn - 5 is < MIN_VOLUMN then set volumn to MIN_VOLUMN
input: bool
output: void
*/
void changeVolumn(bool increase);

#endif