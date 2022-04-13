#include "audio.h"
#include "audioMixer_template.h"
#include <stdio.h>
#include <time.h>

#define NANO_SEC_CONVERSION 1000000000

//Tempo range for audio [40, 300]
#define MAX_TEMPO 300
#define MIN_TEMPO 40

//Volumn range for audio [0, 100]
#define MAX_VOLUMN 100
#define MIN_VOLUMN 0

//Audio file locations
#define GUI_DRUM_DB_HARD "beatbox-wav-files/100051__menegass__gui-drum-bd-hard.wav"
#define GUI_DRUM_CYN_HARD "beatbox-wav-files/100056__menegass__gui-drum-cyn-hard.wav"
#define GUI_DRUM_SNARE_HARD "beatbox-wav-files/100058__menegass__gui-drum-snare-hard.wav"
#define GUI_DRUM_TOM_HI_HARD "beatbox-wav-files/100062__menegass__gui-drum-tom-hi-hard.wav"
#define GUI_DRUM_SNARE_SOFT "beatbox-wav-files/100059__menegass__gui-drum-snare-soft.wav"
#define GUI_DRUM_TOM_HI_SOFT "beatbox-wav-files/100063__menegass__gui-drum-tom-hi-soft.wav"

//Static global for wavedata_t
static wavedata_t drum_db_hard;
static wavedata_t drum_cyn_hard;
static wavedata_t drum_snare_hard;
static wavedata_t drum_tom_hi_hard;
static wavedata_t drum_snare_soft;
static wavedata_t drum_tom_hi_soft;

//Current tempo status
static int tempo = 120;

/*
Initialize audio play using audioMixer_template
input: void
output: void
*/
void start_audio()
{
	AudioMixer_init();
}


/*
End audio play using audioMixer_template
input: void
output: void
*/
void quit_audio() 
{
	AudioMixer_cleanup();
}


/*
Return current status of tempo
input: void
output: int
*/
int getTempo()
{
	return tempo;
}


/*
Copy audio files to memory
input: void
output: void
*/
void initialize_audio_files() 
{
	AudioMixer_readWaveFileIntoMemory(GUI_DRUM_DB_HARD, &drum_db_hard);
	AudioMixer_readWaveFileIntoMemory(GUI_DRUM_CYN_HARD, &drum_cyn_hard);
	AudioMixer_readWaveFileIntoMemory(GUI_DRUM_SNARE_HARD, &drum_snare_hard);
	AudioMixer_readWaveFileIntoMemory(GUI_DRUM_TOM_HI_HARD, &drum_tom_hi_hard);
	AudioMixer_readWaveFileIntoMemory(GUI_DRUM_SNARE_SOFT, &drum_snare_soft);
	AudioMixer_readWaveFileIntoMemory(GUI_DRUM_TOM_HI_SOFT, &drum_tom_hi_soft);
}



/*
Remove audio files from memory
input: void
output: void
*/
void remove_audio_files() 
{
	AudioMixer_freeWaveFileData(&drum_db_hard);
	AudioMixer_freeWaveFileData(&drum_cyn_hard);
	AudioMixer_freeWaveFileData(&drum_snare_hard);
	AudioMixer_freeWaveFileData(&drum_tom_hi_hard);
	AudioMixer_freeWaveFileData(&drum_snare_soft);
	AudioMixer_freeWaveFileData(&drum_tom_hi_soft);
}


/*
Wait function to make audio play in tempo
quarter note 4, 8th note 8, 16th note 16, 32nd note 32
input: int, int
output: void
*/
static void waitTempo(int tempo, int note) 
{
	float waitNanoSecond = (240 / (float)(tempo * note)) * NANO_SEC_CONVERSION;
	struct timespec reqDelay = {0, (long) waitNanoSecond};
	nanosleep(&reqDelay, (struct timespec *) NULL);
}


/*
Adds wavedata to buffer to play rock beat 1
input: void
output: void
*/
void standardRockBeat() 
{
	for(int i = 0; i < 4; i++) {
		AudioMixer_queueSound(&drum_cyn_hard);
		if(i == 0) {
			AudioMixer_queueSound(&drum_db_hard);
		}

		if(i == 2) {
			AudioMixer_queueSound(&drum_snare_hard);
		}
		waitTempo(tempo, 8);
    }
}


/*
Adds wavedata to buffer to play rock beat 2
input: void
output: void
*/
void customDrumBeat() 
{
	for(int i = 0; i < 8; i++) {
		if(i == 0 || i == 2 || i == 3 || i == 5 || i == 7) {
			AudioMixer_queueSound(&drum_db_hard);
		}

		if(i == 0 || i == 4) {
			AudioMixer_queueSound(&drum_snare_soft);
		}

		AudioMixer_queueSound(&drum_tom_hi_soft);
		waitTempo(tempo, 8);
	}
}



/*
Play individual instrument
input: enum
output: void
*/
void queue_sound(Instrument_Sound sound) 
{
	switch(sound) {
		case DRUM_DB_HARD:
			AudioMixer_queueSound(&drum_db_hard);
			break;
		case DRUM_CYN_HARD:
			AudioMixer_queueSound(&drum_cyn_hard);
			break;
		case DRUM_SNARE_HARD:
			AudioMixer_queueSound(&drum_snare_hard);
			break;
		case DRUM_TOM_HI_HARD:
			AudioMixer_queueSound(&drum_tom_hi_hard);
			break;
		case DRUM_SNARE_SOFT:
			AudioMixer_queueSound(&drum_snare_hard);
			break;
		case DRUM_TOM_HI_SOFT:
			AudioMixer_queueSound(&drum_tom_hi_soft);
			break;
		default:
			break;
	}
}



/*
Increase tempo by 5 is input is true else descrease tempo by 5
if tempo + 5 is > MAX_TEMPO then set tempo to MAX_TEMPO
if tempo - 5 is < MIN_TEMPO then set tempo to MIN_TEMPO
input: bool
output: void
*/
void changeTempo(bool increase) {
	if(increase) {
		tempo = tempo + 5 >= MAX_TEMPO ? MAX_TEMPO : tempo + 5;
	} else {
		tempo = tempo - 5 <= MIN_TEMPO ? MIN_TEMPO : tempo - 5;
	}
}



/*
Increase volumn by 5 is input is true else descrease volumn by 5
if volumn + 5 is > MAX_VOLUMN then set volumn to MAX_VOLUMN
if volumn - 5 is < MIN_VOLUMN then set volumn to MIN_VOLUMN
input: bool
output: void
*/
void changeVolumn(bool increase) {
	int newVolumn = AudioMixer_getVolume();
	if(increase) {
		newVolumn = newVolumn + 5 >= MAX_VOLUMN ? MAX_VOLUMN : newVolumn + 5;
	} else {
		newVolumn = newVolumn - 5 <= MIN_VOLUMN ? MIN_VOLUMN : newVolumn - 5;
	}
	AudioMixer_setVolume(newVolumn);
}