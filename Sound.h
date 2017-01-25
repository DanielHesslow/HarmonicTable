#pragma once


struct Note
{
	float advance;
	float phase;
	float amplitude;
	double time;
};

Note notes[200];
int num_active_notes;

#include <stdio.h>
#include <math.h>
#include "L:\PortAudio\portaudio.h"

#define NUM_SECONDS   (5)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

typedef struct
{
	float left_phase, right_phase;
	char message[20];
}
paTestData;

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int patestCallback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	paTestData *data = (paTestData*)userData;
	float *out = (float*)outputBuffer;
	unsigned long i;

	(void)timeInfo; /* Prevent unused variable warnings. */
	(void)statusFlags;
	(void)inputBuffer;
	double t = timeInfo->outputBufferDacTime;

	for (i = 0; i<framesPerBuffer; i++)
	{
		float left_ack = 0.0f;
		float right_ack = 0.0f;
		float amplitude_ack = 0.0;

		for (int j = 0; j < num_active_notes; j++)
		{
			float note_time = (t-notes[j].time) + (float)i / SAMPLE_RATE;
			note_time = min(note_time, 1.);
			float time_amp = (1 - pow(note_time * 2 - 1, 6));
			float amplitude = time_amp * notes[j].amplitude;
			left_ack += sin(notes[j].phase) *amplitude;
			amplitude_ack += amplitude;
			notes[j].phase += notes[j].advance;
			if (notes[j].phase > M_PI * 2) notes[j].phase -= M_PI * 2;
		}
		left_ack /= 5.;

		*out++ = left_ack;//left
		*out++ = left_ack;//right (mono for now)
	}

	return paContinue;
}

/*
* This routine is called by portaudio when playback is done.
*/
static void StreamFinished(void* userData)
{
	paTestData *data = (paTestData *)userData;
	printf("Stream Completed: %s\n", data->message);
}

/*******************************************************************/

PaStream *stream=0;

void addNote(float freq)
{
	assert(stream);
	static float q = 0;
	//NOTE Multi-Threaded, things will go wrong here. I don't care atm
	Note note = {};
	note.time = Pa_GetStreamTime(stream);
	float freq_dither = fmod(note.time * 103, 1.0);

	note.advance = freq * 2.0f * M_PI / SAMPLE_RATE;
	note.amplitude = 1.0f;
	if (num_active_notes >= 199) num_active_notes = 0;
	notes[num_active_notes++] = note;
}

void sound_close()
{
	if (stream)
	{
		Pa_StopStream(stream);
		Pa_CloseStream(stream);
	}
	Pa_Terminate();
}

void clear_notes()
{
	num_active_notes = 0;
}

void init_sound()
{
	PaStreamParameters outputParameters;
	PaError err;
	paTestData data;
	int i;

	printf("PortAudio Test: output sine wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);


	data.left_phase = data.right_phase = 0;

	err = Pa_Initialize();
	if (err != paNoError) goto error;

	PaHostApiTypeId type = paDirectSound;
	PaHostApiIndex api_index = Pa_HostApiTypeIdToHostApiIndex(type);

	PaDeviceIndex Pa_HostApiDeviceIndexToDeviceIndex(PaHostApiIndex hostApi,
		int hostApiDeviceIndex);

	outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
	if (outputParameters.device == paNoDevice) {
		fprintf(stderr, "Error: No default output device.\n");
		goto error;
	}
	outputParameters.channelCount = 2;       /* stereo output */
	outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	err = Pa_OpenStream(
		&stream,
		NULL, /* no input */
		&outputParameters,
		SAMPLE_RATE,
		FRAMES_PER_BUFFER,
		paClipOff,      /* we won't output out of range samples so don't bother clipping them */
		patestCallback,
		&data);
	if (err != paNoError) goto error;

	sprintf(data.message, "No Message");
	err = Pa_SetStreamFinishedCallback(stream, &StreamFinished);
	err = Pa_StartStream(stream);
	return;
error:
	sound_close();
}

