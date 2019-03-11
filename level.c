/*

   Example 4 - Simple sound recording

   This example reads from the default PCM device
   and writes to standard output for 5 seconds of data.

*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include "dotstar.h"
#include <math.h>

int main() {
	long loops;
	int rc;
	int size;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	unsigned int val;
	int dir;
	snd_pcm_uframes_t frames;
	char *buffer;



	/* Open PCM device for recording (capture). */
	rc = snd_pcm_open(&handle, "default",
			SND_PCM_STREAM_CAPTURE, 0);
	if (rc < 0) {
		fprintf(stderr, 
				"unable to open pcm device: %s\n",
				snd_strerror(rc));
		exit(1);
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(&params);

	/* Fill it in with default values. */
	snd_pcm_hw_params_any(handle, params);

	/* Set the desired hardware parameters. */

	/* Interleaved mode */
	snd_pcm_hw_params_set_access(handle, params,
			SND_PCM_ACCESS_RW_INTERLEAVED);

	/* Signed 16-bit little-endian format */
	snd_pcm_hw_params_set_format(handle, params,
			SND_PCM_FORMAT_S16_LE);

	/* Two channels (stereo) */
	snd_pcm_hw_params_set_channels(handle, params, 2);

	/* 44100 bits/second sampling rate (CD quality) */
	val = 44100;
	snd_pcm_hw_params_set_rate_near(handle, params, 
			&val, &dir);

	/* Set period size to 32 frames. */
	frames = 32;
	snd_pcm_hw_params_set_period_size_near(handle, 
			params, &frames, &dir);

	/* Write the parameters to the driver */
	rc = snd_pcm_hw_params(handle, params);
	if (rc < 0) {
		fprintf(stderr,
				"unable to set hw parameters: %s\n",
				snd_strerror(rc));
		exit(1);
	}

	/* Use a buffer large enough to hold one period */
	snd_pcm_hw_params_get_period_size(params,
			&frames, &dir);
	size = frames * 4; /* 2 bytes/sample, 2 channels */
	buffer = (char *) malloc(size);

	/* We want to loop for 5 seconds */
	snd_pcm_hw_params_get_period_time(params,
			&val, &dir);
	loops = 50e6 / val; //run for 50 seconds

	printf("period time: %d us\n", val); //length of a period in microseconds
	printf("period frames: %d\n", frames); //frames per period
	printf("loops: %d\n", loops); //number of periods before exiting

	dotstar_t* led_array = dotstar_init(144);
	if (led_array == 0)
		return 1;

	uint16_t avg_len = 1e6/val; //moving average length(1 second)
	uint32_t* avg_array = malloc(avg_len*sizeof(uint32_t));	
	if (avg_array == 0)
		return 1;
	memset(avg_array, 0,avg_len*sizeof(uint32_t)); 
	uint16_t avg_index = 0;
	uint32_t mov_sum = 0; //moving sum
	while (loops > 0) {
		loops--;
		rc = snd_pcm_readi(handle, buffer, frames);
		if (rc == -EPIPE) {
			/* EPIPE means overrun */
			fprintf(stderr, "overrun occurred\n");
			snd_pcm_prepare(handle);
		} else if (rc < 0) {
			fprintf(stderr,
					"error from read: %s\n", 
					snd_strerror(rc));
		} else if (rc != (int)frames) {
			fprintf(stderr, "short read, read %d frames\n", rc);
		}

		int64_t sum = 0;
		int32_t count = 0;
		int32_t i = 0;
		//Calculate the average for this period
		for ( i = 0; i < size; i+=4)
		{
			int16_t ch1 = (buffer[i] | buffer[i+1]<<8);
			sum += (ch1 < 0)? -1*ch1:ch1; //ignore the sign
			count++;
		}
		float avg = (float)sum/(float)(count*10.0);
		int iavg = (int)avg;

		mov_sum -= avg_array[avg_index];
		mov_sum += iavg;

		avg_array[avg_index] = iavg; //insert current avg to the array
		avg_index ++;
		if (avg_index == avg_len) avg_index = 0;

		
//		printf("%f\r", 10*log((double)mov_sum));
//
		double u_thres = 12e4;
		double l_thres = 8e4;
		uint8_t red, green;
		if (mov_sum > l_thres && mov_sum < u_thres) 
		{
			red = (mov_sum - l_thres)/(u_thres-l_thres)*255;
			green = (u_thres-mov_sum)/(u_thres-l_thres)*255;
		}
		else if (mov_sum < l_thres)
		{
			green = 255;
			red = 0;
		}
		else
		{
			green = 0;
			red = 255;
		}
		printf("%d\t%d\t%d\n", mov_sum, red, green);

			
			for ( i = 0; i < 10; i++)

			{
				if (mov_sum > 1e5)
					dotstar_set_led(led_array, i, 0,green,red,2);
				else

					dotstar_set_led(led_array, i, 0,green,red, 2);
			}
			dotstar_refresh(led_array, 2);
		



	}
	uint16_t i;
	for ( i = 0; i < 144; i++)
	{
		dotstar_set_led(led_array, i, 0, 0, 0, 0);
	}
	dotstar_refresh(led_array, 144);
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	free(buffer);

	return 0;
}
