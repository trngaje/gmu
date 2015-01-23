/* 
 * Gmu Music Player
 *
 * Copyright (c) 2006-2012 Johannes Heimansberg (wejp.k.vu)
 *
 * File: audio.c  Created: 061110
 *
 * Description: Audio output functions
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 of
 * the License. See the file COPYING in the Gmu's main directory
 * for details.
 */
#include <math.h>
#include "SDL.h"
#include "ringbuffer.h"
#include "audio.h"
#include "fmath.h"
#include "debug.h"
#include "eventqueue.h"
#include "gmuerror.h"
#include "core.h"
#include FILE_HW_H
#define RINGBUFFER_SIZE 131072

static RingBuffer    audio_rb;
static SDL_mutex    *audio_mutex, *spectrum_mutex, *cond_mutex, *audio_mutex2;
static SDL_cond     *cond_data_needed;
static unsigned long buf_read_counter;
static int           have_samplerate, have_channels;
static int           device_open;
static int           paused, done;
static char          buf[65536];
static unsigned int  volume, volume_internal, volume_fade_percent = 100;
static int           spectrum_reg = 0;

int audio_fill_buffer(char *data, int size)
{
	int result = 0;
	if (SDL_mutexP(audio_mutex) == 0) {
		result = ringbuffer_write(&audio_rb, data, size);
		SDL_mutexV(audio_mutex);
	}
	return result;
}

static void calculate_dft(int16_t *input_signal, int input_signal_size, int *rex, int *imx)
{
	int res_size = input_signal_size / 2 + 1;

	if (rex && imx) {
		int i, j, rs = res_size * sizeof(int);
		memset(rex, 0, rs);
		memset(imx, 0, rs);
		for (j = 0; j < res_size; j++) {
			for (i = 0; i < input_signal_size; i++) {
				/*if (rex) rex[j] = rex[j] + input_signal[i] * cos(2*M_PI*j*i/input_signal_size);
				if (imx) imx[j] = imx[j] + input_signal[i] * sin(2*M_PI*j*i/input_signal_size);*/
				if (rex) rex[j] = rex[j] + input_signal[i] * fcos(F_PI2*j*i/input_signal_size);
				if (imx) imx[j] = imx[j] + input_signal[i] * fsin(F_PI2*j*i/input_signal_size);
			}
		}
		for (j = 0; j < res_size; j++) {
			if (rex) rex[j] /= 10000;
			if (imx) imx[j] /= 10000;
		}
	}
}

static int16_t amplitudes[16];

int16_t *audio_spectrum_get_current_amplitudes(void)
{
	return amplitudes;
}

void audio_spectrum_register_for_access(void)
{
	spectrum_reg++;
}

void audio_spectrum_unregister(void)
{
	if (spectrum_reg > 0) spectrum_reg--;
}

int audio_spectrum_read_lock(void)
{
	return !SDL_mutexP(spectrum_mutex);
}

void audio_spectrum_read_unlock(void)
{
	SDL_mutexV(spectrum_mutex);
}

static void fill_audio(void *udata, Uint8 *stream, int len)
{
	SDL_mutexP(audio_mutex);
	if (ringbuffer_get_fill(&audio_rb) < MIN_BUFFER_FILL * 3) {
		SDL_CondSignal(cond_data_needed);
	}
	if (ringbuffer_get_fill(&audio_rb) < MIN_BUFFER_FILL) {
		wdprintf(V_WARNING, "audio", "Buffer %sempty! Buffer fill: %d bytes\n",
		         ringbuffer_get_fill(&audio_rb) > 0 ? "almost " : "",
		         ringbuffer_get_fill(&audio_rb));
		if (ringbuffer_get_fill(&audio_rb) == 0) {
			SDL_PauseAudio(1);
			if (SDL_mutexV(audio_mutex) == 0) {
				int tmp_done = 0;
				if (SDL_mutexP(audio_mutex2) != -1) {
					tmp_done = done;
					SDL_mutexV(audio_mutex2);
				}
				if (tmp_done) {
					audio_set_pause(1);
				} else {
					int loop = 1;
					while (loop) {
						if (SDL_mutexP(audio_mutex2) != -1) {
							if (paused || done) loop = 0;
							SDL_mutexV(audio_mutex2);
						}
						if (SDL_mutexP(audio_mutex) != -1) {
							if (ringbuffer_get_free(&audio_rb) <= 65536) loop = 0;
							SDL_mutexV(audio_mutex);
						}
						wdprintf(V_DEBUG, "audio", "Waiting for buffer to refill...\n");
						SDL_Delay(100);
					}
				}
				SDL_mutexP(audio_mutex);
			}
			if (!paused) SDL_PauseAudio(0);
		}
	}
	if (!ringbuffer_read(&audio_rb, buf, len)) {
		int avail = ringbuffer_get_fill(&audio_rb);
		memset(buf, 0, 65536);
		if (ringbuffer_read(&audio_rb, buf, avail)) {
			if (SDL_mutexP(audio_mutex2) != -1) {
				buf_read_counter += avail;
				SDL_mutexV(audio_mutex2);
			}
		}
	} else if (SDL_mutexP(audio_mutex2) != -1) {
		buf_read_counter += len;
		SDL_mutexV(audio_mutex2);
	}

	/* When requested, run DFT on a few samples of each block of data for visualization purposes */
	if (spectrum_reg > 0) {
		int rex[9], imx[9];
		int i, j; 
		int16_t samples_l[16];

		for (i = 0, j = 0; i < 16 * 2 * have_channels; i += (2 * have_channels), j++) {
			samples_l[j] = (buf[i+1] << 8) + buf[i];
		}
		calculate_dft(samples_l, 16, rex, imx);
		SDL_mutexP(spectrum_mutex);
		for (i = 1; i < 9; i++) amplitudes[i-1] = (imx[i] < 0 ? -imx[i] : imx[i]);
		SDL_mutexV(spectrum_mutex);
	}

	SDL_MixAudio(stream, (unsigned char *)buf, len, volume * volume_fade_percent / 100);
	SDL_mutexV(audio_mutex);
}

int audio_device_open(int samplerate, int channels)
{
	static SDL_AudioSpec wanted, obtained;
	int                  result = -1;

	/* Keep audio device open unless sampling rate or number of channels change */
	if (SDL_mutexP(audio_mutex2) != -1) {
		buf_read_counter = 0;
		wdprintf(V_DEBUG, "audio", "Device already open: %s\n", device_open ? "yes" : "no");
		if (device_open)
			wdprintf(V_DEBUG, "audio", "Samplerate: have=%d want=%d Channels: have=%d want=%d\n",
					 have_samplerate, samplerate, have_channels, channels);
		if (!device_open || samplerate != have_samplerate || channels != have_channels) {
			if (device_open) audio_device_close();
			wdprintf(V_INFO, "audio", "Opening audio device...\n");
			wanted.freq     = samplerate;
			wanted.format   = AUDIO_S16;
			wanted.channels = channels; /* 1 = mono, 2 = stereo */
			wanted.samples  = SAMPLE_BUFFER_SIZE;
			wanted.callback = fill_audio;
			wanted.userdata = NULL;
			SDL_ClearError();
			if (SDL_OpenAudio(&wanted, &obtained) < 0) {
				wdprintf(V_ERROR, "audio", "Could not open audio: %s\n", SDL_GetError());
				event_queue_push_with_parameter(gmu_core_get_event_queue(),
				                                GMU_ERROR,
				                                GMU_ERROR_CANNOT_OPEN_AUDIO_DEVICE);
				result = -3;
			} else {
				result = 0;
				device_open = 1;
				have_samplerate = samplerate;
				have_channels   = channels;
				wdprintf(V_INFO, "audio", "Device opened with %d Hz, %d channels and sample buffer w/ %d samples.\n",
						 obtained.freq, obtained.channels, obtained.samples);
			}
			if (SDL_mutexV(audio_mutex2) != -1) {
				if (SDL_mutexP(audio_mutex) != -1) {
					ringbuffer_clear(&audio_rb);
					SDL_mutexV(audio_mutex);
				}
				SDL_mutexP(audio_mutex2);
			}
		} else {
			wdprintf(V_INFO, "audio", "Using already opened audio device with the same settings...\n");
			result = 0;
		}

		if (result == 0) {
			done = 0;
			SDL_CondSignal(cond_data_needed);
		}
		SDL_mutexV(audio_mutex2);
	}
	return result;
}

int audio_set_pause(int pause_state)
{
	if (device_open) {
		wdprintf(V_DEBUG, "audio", "%s\n", pause_state ? "Pause!" : "Play!");
		if (paused != pause_state) {
			paused = pause_state;
			if (paused) memset(amplitudes, 0, sizeof(int16_t) * 16);
			SDL_PauseAudio(paused);
		}
	} else {
		wdprintf(V_WARNING, "audio", "Device not opened. Cannot set pause state!\n");
	}
	return paused;
}

void audio_set_done(void)
{
	if (SDL_mutexP(audio_mutex2) != -1) {
		done = 1;
		SDL_mutexV(audio_mutex2);
	}
}

int audio_get_pause(void)
{
	return paused;
}

int audio_get_playtime(void)
{
	int res = 0;
	if (SDL_mutexP(audio_mutex2) != -1) {
		res = buf_read_counter / (have_samplerate * 2 * have_channels) * 1000;
		SDL_mutexV(audio_mutex2);
	}
	return res;
}

int audio_buffer_get_fill(void)
{
	int res = 0;
	if (SDL_mutexP(audio_mutex) != -1) {
		res = ringbuffer_get_fill(&audio_rb);
		SDL_mutexV(audio_mutex);
	}
	return res;
}

int audio_buffer_get_size(void)
{
	int res = 0;
	if (SDL_mutexP(audio_mutex) != -1) {
		res = ringbuffer_get_size(&audio_rb);
		SDL_mutexV(audio_mutex);
	}
	return res;
}

void audio_buffer_init(void)
{
	volume = SDL_MIX_MAXVOLUME;
	volume_internal = 15;
	paused = 1;
	done = 0;
	device_open = 0;
	have_samplerate = 1;
	have_channels = 1;
	ringbuffer_init(&audio_rb, RINGBUFFER_SIZE);
	audio_mutex = SDL_CreateMutex();
	spectrum_mutex = SDL_CreateMutex();
	cond_mutex = SDL_CreateMutex();
	cond_data_needed = SDL_CreateCond();
	audio_mutex2 = SDL_CreateMutex();
}

void audio_buffer_clear(void)
{
	audio_set_pause(1);
	if (SDL_mutexP(audio_mutex) != -1) {
		ringbuffer_clear(&audio_rb);
		SDL_mutexV(audio_mutex);
	}
}

void audio_buffer_free(void)
{
	ringbuffer_free(&audio_rb);
	SDL_DestroyMutex(audio_mutex);
	SDL_DestroyMutex(spectrum_mutex);
	SDL_DestroyCond(cond_data_needed);
	SDL_DestroyMutex(cond_mutex);
	if (audio_mutex2) SDL_DestroyMutex(audio_mutex2);
}

void audio_device_close(void)
{
	if (device_open) {
		wdprintf(V_DEBUG, "audio", "Closing device.\n");
		audio_set_pause(1);
		device_open = 0;
		SDL_CloseAudio();
		wdprintf(V_INFO, "audio", "Device closed.\n");
	}
}

static const int volume_array[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 16, 24, 32, 48, 64, 96, 128 };

void audio_set_volume(int vol) /* 0..AUDIO_MAX_SW_VOLUME */
{
	volume_internal = (vol < AUDIO_MAX_SW_VOLUME ? vol : AUDIO_MAX_SW_VOLUME-1);
	volume_internal = (volume_internal > 0 ? volume_internal : 0);
	volume = volume_array[volume_internal]; /*(SDL_MIX_MAXVOLUME / AUDIO_MAX_SW_VOLUME) * volume_internal;*/
	wdprintf(V_DEBUG, "audio", "volume=%d (%d/%d)\n", volume, SDL_MIX_MAXVOLUME, AUDIO_MAX_SW_VOLUME);
}

int audio_get_volume(void)
{
	return volume_internal;
}

long audio_set_sample_counter(long sample)
{
	long res = 0;
	if (SDL_mutexP(audio_mutex2) != -1) {
		res = buf_read_counter = (sample * 2 * have_channels);
		SDL_mutexV(audio_mutex2);
	}
	return res;
}

long audio_increase_sample_counter(long sample_offset)
{
	long res = 0;
	if (SDL_mutexP(audio_mutex2) != -1) {
		buf_read_counter += (sample_offset * 2 * have_channels);
		res = buf_read_counter;
		SDL_mutexV(audio_mutex2);
	}
	return res;
}

long audio_get_sample_count(void)
{
	long res = 0;
	if (SDL_mutexP(audio_mutex2) != -1) {
		res = buf_read_counter / (2 * have_channels);
		SDL_mutexV(audio_mutex2);
	}
	return res;
}

void audio_wait_until_more_data_is_needed(void)
{
	SDL_mutexP(cond_mutex);
	SDL_CondWaitTimeout(cond_data_needed, cond_mutex, 200);
	SDL_mutexV(cond_mutex);
}

void audio_set_fade_volume(int percent)
{
	if (percent >= 0 && percent <= 100)
		volume_fade_percent = percent;
}

/**
 * Fades out one step. To be called multiple times until the volume has
 * reached 0. Returns 1 when the volume is zero, and 0 otherwise.
 */
int audio_fade_out_step(unsigned int step_size)
{
	if (volume_fade_percent > 0 && volume_fade_percent >= step_size)
		volume_fade_percent -= step_size;
	else
		volume_fade_percent = 0;
	wdprintf(V_DEBUG, "audio", "fadeout: %d\n", volume_fade_percent);
	return volume_fade_percent == 0 ? 1 : 0;
}

void audio_reset_fade_volume(void)
{
	volume_fade_percent = 100;
}

int audio_fade_out_in_progress(void)
{
	return (volume_fade_percent < 100 && volume_fade_percent > 0) ? 1 : 0;
}
