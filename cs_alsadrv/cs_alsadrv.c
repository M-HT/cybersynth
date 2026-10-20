/**
 *
 *  Copyright (C) 2022-2026 Roman Pauer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#define _FILE_OFFSET_BITS 64
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <pwd.h>
#include <alsa/asoundlib.h>
#include "memory32.h"
#include "ptr32.h"
#include "synth.h"


static const char midi_name[] = "CyberSynth";
static const char port_name[] = "CyberSynth port";

static snd_seq_t *midi_seq;
static int midi_port_id;
static pthread_t midi_thread;
static snd_pcm_t *midi_pcm;
static volatile int midi_init_state;
static volatile int midi_event_written;

#ifdef __cplusplus
extern "C" {
#endif
const char *cs_bank_directory_path = NULL;
unsigned char cs_effects_enabled = 1;
unsigned char cs_chorus_enabled = 1;
unsigned char cs_reverb_enabled = 1;
unsigned char cs_midi_mode = 1; // 0 = CyberSound Mode, 1 = GM Mode, 2 = GS Mode
unsigned int cs_number_of_voices = 32; // 0-128
unsigned int cs_sound_cache_size = 8192; // 2048-16384 kilobytes
unsigned char cs_simd_enabled = 1;
int32_t cs_volume_multiplier = 1 << 11; // linear volume multiplier in fixed-point format s.4.11
#ifdef __cplusplus
}
#endif

static int daemonize = 0;

const static unsigned int frequency = 22050;
const static unsigned int num_channels = 2;

static unsigned int num_subbuffers, subbuf_counter;

static struct timespec base_time;

#if defined(CLOCK_MONOTONIC_RAW)
static clockid_t monotonic_clock_id;
#define MONOTONIC_CLOCK_TYPE monotonic_clock_id
#else
#define MONOTONIC_CLOCK_TYPE CLOCK_MONOTONIC
#endif

static uint8_t midi_buffer[65536];
static uint8_t *event_buffer;

static volatile int event_read_index;
static volatile int event_write_index;


#ifdef __cplusplus
extern "C"
#endif
uint32_t CCALL CS_GetTime(void)
{
    struct timespec current_time;

    clock_gettime(MONOTONIC_CLOCK_TYPE, &current_time);

    return ((current_time.tv_sec - base_time.tv_sec) * (uint64_t)1000000000 + current_time.tv_nsec - base_time.tv_nsec) / 1000000ul;
}


static void set_thread_scheduler(void) __attribute__((noinline));
static void set_thread_scheduler(void)
{
    struct sched_param param;

    memset(&param, 0, sizeof(struct sched_param));
    param.sched_priority = sched_get_priority_min(SCHED_FIFO);
    if (param.sched_priority > 0)
    {
        sched_setscheduler(0, SCHED_FIFO, &param);
    }
}

static void wait_for_midi_initialization(void) __attribute__((noinline));
static void wait_for_midi_initialization(void)
{
    while (midi_init_state == 0)
    {
        struct timespec req;

        req.tv_sec = 0;
        req.tv_nsec = 10000000;
        nanosleep(&req, NULL);
    };
}

static void subscription_event(snd_seq_event_t *event) __attribute__((noinline));
static void subscription_event(snd_seq_event_t *event)
{
    snd_seq_client_info_t *cinfo;
    int err;

    snd_seq_client_info_alloca(&cinfo);
    err = snd_seq_get_any_client_info(midi_seq, event->data.connect.sender.client, cinfo);
    if (err >= 0)
    {
        if (event->type == SND_SEQ_EVENT_PORT_SUBSCRIBED)
        {
            printf("Client subscribed: %s\n", snd_seq_client_info_get_name(cinfo));
        }
        else
        {
            printf("Client unsubscribed: %s\n", snd_seq_client_info_get_name(cinfo));
        }
    }
    else
    {
        printf("Client unsubscribed\n");
    }
}

static void write_event(const uint8_t *event, unsigned int length)
{
    int read_index, write_index;
    unsigned int free_space;

    // read global volatile variables to local variables
    read_index = event_read_index;
    write_index = event_write_index;

    if (write_index >= read_index)
    {
        free_space = 65535 - (write_index - read_index);
    }
    else
    {
        free_space = (read_index - write_index) - 1;
    }

    if (length > free_space)
    {
        fprintf(stderr, "Event buffer overflow\n");
        return;
    }

    // write event to event buffer
    for (; length != 0; length--,event++)
    {
        event_buffer[write_index] = *event;
        write_index = (write_index + 1) & 0xffff;
    }

    // update global volatile variable
    event_write_index = write_index;

    midi_event_written = 1;
}

static void process_event(snd_seq_event_t *event, uint8_t *running_status)
{
    uint8_t data[12];
    unsigned int length;

    switch (event->type)
    {
        case SND_SEQ_EVENT_NOTEON:
            data[0] = 0x90 | event->data.note.channel;
            data[1] = event->data.note.note;
            data[2] = event->data.note.velocity;
            length = 3;

            if (data[0] != *running_status)
            {
                *running_status = data[0];
                write_event(data, length);
            }
            else
            {
                write_event(data + 1, length - 1);
            }

#ifdef PRINT_EVENTS
            printf("Note ON, channel:%d note:%d velocity:%d\n", event->data.note.channel, event->data.note.note, event->data.note.velocity);
#endif

            break;

        case SND_SEQ_EVENT_NOTEOFF:
            // send note off event as note on with zero velocity to increase the chance of using running status
            data[0] = 0x90 | event->data.note.channel;
            data[1] = event->data.note.note;
            data[2] = 0;
            length = 3;

            if (data[0] != *running_status)
            {
                *running_status = data[0];
                write_event(data, length);
            }
            else
            {
                write_event(data + 1, length - 1);
            }

#ifdef PRINT_EVENTS
            printf("Note OFF, channel:%d note:%d velocity:%d\n", event->data.note.channel, event->data.note.note, event->data.note.velocity);
#endif

            break;

        case SND_SEQ_EVENT_KEYPRESS:
            // Not used by CyberSynth
#if 0
            data[0] = 0xA0 | event->data.note.channel;
            data[1] = event->data.note.note;
            data[2] = event->data.note.velocity;
            length = 3;

            if (data[0] != *running_status)
            {
                *running_status = data[0];
                write_event(data, length);
            }
            else
            {
                write_event(data + 1, length - 1);
            }
#endif

#ifdef PRINT_EVENTS
            printf("Keypress, channel:%d note:%d velocity:%d\n", event->data.note.channel, event->data.note.note, event->data.note.velocity);
#endif

            break;

        case SND_SEQ_EVENT_CONTROLLER:
            data[0] = 0xB0 | event->data.control.channel;
            data[1] = event->data.control.param;
            data[2] = event->data.control.value;
            length = 3;

            if (data[0] != *running_status)
            {
                *running_status = data[0];
                write_event(data, length);
            }
            else
            {
                write_event(data + 1, length - 1);
            }

#ifdef PRINT_EVENTS
            printf("Controller, channel:%d param:%d value:%d\n", event->data.control.channel, event->data.control.param, event->data.control.value);
#endif

            break;

        case SND_SEQ_EVENT_PGMCHANGE:
            data[0] = 0xC0 | event->data.control.channel;
            data[1] = event->data.control.value;
            length = 2;

            if (data[0] != *running_status)
            {
                *running_status = data[0];
                write_event(data, length);
            }
            else
            {
                write_event(data + 1, length - 1);
            }

#ifdef PRINT_EVENTS
            printf("Program change, channel:%d value:%d\n", event->data.control.channel, event->data.control.value);
#endif

            break;

        case SND_SEQ_EVENT_CHANPRESS:
            data[0] = 0xD0 | event->data.control.channel;
            data[1] = event->data.control.value;
            length = 2;

            if (data[0] != *running_status)
            {
                *running_status = data[0];
                write_event(data, length);
            }
            else
            {
                write_event(data + 1, length - 1);
            }

#ifdef PRINT_EVENTS
            printf("Channel pressure, channel:%d value:%d\n", event->data.control.channel, event->data.control.value);
#endif

            break;

        case SND_SEQ_EVENT_PITCHBEND:
            data[0] = 0xE0 | event->data.control.channel;
            data[1] = (event->data.control.value + 0x2000) & 0x7f;
            data[2] = ((event->data.control.value + 0x2000) >> 7) & 0x7f;
            length = 3;

            if (data[0] != *running_status)
            {
                *running_status = data[0];
                write_event(data, length);
            }
            else
            {
                write_event(data + 1, length - 1);
            }

#ifdef PRINT_EVENTS
            printf("Pitch bend, channel:%d value:%d\n", event->data.control.channel, event->data.control.value);
#endif

            break;

        case SND_SEQ_EVENT_CONTROL14:
            if (event->data.control.param >= 0 && event->data.control.param < 32)
            {
                data[0] = 0xB0 | event->data.control.channel;
                data[1] = event->data.control.param;
                data[2] = (event->data.control.value >> 7) & 0x7f;
                data[3] = event->data.control.param + 32;
                data[4] = event->data.control.value & 0x7f;
                length = 5;

                if (data[0] != *running_status)
                {
                    *running_status = data[0];
                    write_event(data, length);
                }
                else
                {
                    write_event(data + 1, length - 1);
                }

#ifdef PRINT_EVENTS
                printf("Controller 14-bit, channel:%d param:%d value:%d\n", event->data.control.channel, event->data.control.param, event->data.control.value);
#endif
            }
            else
            {
#ifdef PRINT_EVENTS
                printf("Unknown controller, channel:%d param:%d value:%d\n", event->data.control.channel, event->data.control.param, event->data.control.value);
#endif
            }


            break;

        case SND_SEQ_EVENT_NONREGPARAM:
            // Not used by CyberSynth
#if 0
            data[0] = 0xB0 | event->data.control.channel;
            data[1] = 0x63; // NRPN MSB
            data[2] = (event->data.control.param >> 7) & 0x7f;
            data[3] = 0x62; // NRPN LSB
            data[4] = event->data.control.param & 0x7f;
            data[5] = 0x06; // data entry MSB
            data[6] = (event->data.control.value >> 7) & 0x7f;
            data[7] = 0x26; // data entry LSB
            data[8] = event->data.control.value & 0x7f;
            length = 9;

            if (data[0] != *running_status)
            {
                *running_status = data[0];
                write_event(data, length);
            }
            else
            {
                write_event(data + 1, length - 1);
            }
#endif

#ifdef PRINT_EVENTS
            printf("NRPN, channel:%d param:%d value:%d\n", event->data.control.channel, event->data.control.param, event->data.control.value);
#endif

            break;

        case SND_SEQ_EVENT_REGPARAM:
            data[0] = 0xB0 | event->data.control.channel;
            data[1] = 0x65; // RPN MSB
            data[2] = (event->data.control.param >> 7) & 0x7f;
            data[3] = 0x64; // RPN LSB
            data[4] = event->data.control.param & 0x7f;
            data[5] = 0x06; // data entry MSB
            data[6] = (event->data.control.value >> 7) & 0x7f;
            data[7] = 0x26; // data entry LSB
            data[8] = event->data.control.value & 0x7f;
            length = 9;

            if (data[0] != *running_status)
            {
                *running_status = data[0];
                write_event(data, length);
            }
            else
            {
                write_event(data + 1, length - 1);
            }

#ifdef PRINT_EVENTS
            printf("RPN, channel:%d param:%d value:%d\n", event->data.control.channel, event->data.control.param, event->data.control.value);
#endif

            break;

        case SND_SEQ_EVENT_SYSEX:
            *running_status = 0;
            write_event((uint8_t *)event->data.ext.ptr, event->data.ext.len);

#ifdef PRINT_EVENTS
            printf("SysEx (fragment) of size %d\n", event->data.ext.len);
#endif

            break;

        case SND_SEQ_EVENT_QFRAME:
            // Not used by CyberSynth
#if 0
            data[0] = 0xF1;
            data[1] = ev->data.control.value;
            length = 2;

            *running_status = 0;
            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
            printf("MTC Quarter Frame, value:%d\n", event->data.control.value);
#endif

            break;

        case SND_SEQ_EVENT_SONGPOS:
            // Not used by CyberSynth
#if 0
            data[0] = 0xF2;
            data[1] = (event->data.control.value + 0x2000) & 0x7f;
            data[2] = ((event->data.control.value + 0x2000) >> 7) & 0x7f;
            length = 3;

            *running_status = 0;
            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
            printf("Song Position, value:%d\n", event->data.control.value);
#endif

            break;

        case SND_SEQ_EVENT_SONGSEL:
            // Not used by CyberSynth
#if 0
            data[0] = 0xF3;
            data[1] = ev->data.control.value;
            length = 2;

            *running_status = 0;
            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
            printf("Song Select, value:%d\n", event->data.control.value);
#endif

            break;

        case SND_SEQ_EVENT_TUNE_REQUEST:
            // Not used by CyberSynth
#if 0
            data[0] = 0xF6;
            length = 1;

            *running_status = 0;
            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
            printf("Tune Request\n");
#endif

            break;

        case SND_SEQ_EVENT_CLOCK:
            // Not used by CyberSynth
#if 0
            data[0] = 0xF8;
            length = 1;

            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
            printf("Clock\n");
#endif

            break;

        case SND_SEQ_EVENT_TICK:
            // Not used by CyberSynth
#if 0
            data[0] = 0xF9;
            length = 1;

            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
            printf("Tick\n");
#endif

            break;

        case SND_SEQ_EVENT_START:
            // Not used by CyberSynth
#if 0
            data[0] = 0xFA;
            length = 1;

            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
            printf("Start\n");
#endif

            break;

        case SND_SEQ_EVENT_CONTINUE:
            // Not used by CyberSynth
#if 0
            data[0] = 0xFB;
            length = 1;

            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
            printf("Continue\n");
#endif

            break;

        case SND_SEQ_EVENT_STOP:
            // Not used by CyberSynth
#if 0
            data[0] = 0xFC;
            length = 1;

            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
            printf("Stop\n");
#endif

            break;

        case SND_SEQ_EVENT_SENSING:
            // Not used by CyberSynth
#if 0
            data[0] = 0xFE;
            length = 1;

            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
            printf("Active Sense\n");
#endif

            break;

        case SND_SEQ_EVENT_RESET:
            // Not used by CyberSynth
#if 0
            data[0] = 0xFF;
            length = 1;

            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
            printf("Reset\n");
#endif

            break;

        case SND_SEQ_EVENT_PORT_SUBSCRIBED:
            subscription_event(event);
            break;

        case SND_SEQ_EVENT_PORT_UNSUBSCRIBED:
            subscription_event(event);
            break;

        default:
            fprintf(stderr, "Unhandled event type: %i\n", event->type);
            break;
    }
}

static void *midi_thread_proc(void *arg)
{
    snd_seq_event_t *event;
    uint8_t running_status;

    // try setting thread scheduler (only root)
    set_thread_scheduler();

    // set thread as initialized
    *(int *)arg = 1;

    wait_for_midi_initialization();

    running_status = 0;

    while (midi_init_state > 0)
    {
        if (snd_seq_event_input(midi_seq, &event) < 0)
        {
            continue;
        }

        process_event(event, &running_status);
    }

    return NULL;
}

static void usage(const char *progname)
{
    static const char basename[] = "cs_alsadrv";

    if (progname == NULL)
    {
        progname = basename;
    }
    else
    {
        const char *slash;

        slash = strrchr(progname, '/');
        if (slash != NULL)
        {
            progname = slash + 1;
        }
    }

    printf(
        "%s - CyberSynth\n"
        "Usage: %s [OPTIONS]...\n"
        "  -w PATH  Datafiles path (path to directory with *.iss files)\n"
        "  -p NUM   Polyphony (0-128 [default=32])\n"
        "  -e NUM   Effects (0=off, 1=on [default])\n"
        "  -c NUM   Chorus effect (0=off, 1=on [default])\n"
        "  -r NUM   Reverb effect (0=off, 1=on [default])\n"
        "  -g FLT   Volume gain in decibels (0.0-24.0 [default=0.0])\n"
        "  -m NUM   MIDI mode (0=CyberSound, 1=GM [default], 2=GS)\n"
        "  -k NUM   Sound cache size in kilobytes (2048-16384 [default=8192])\n"
        "  -x NUM   SIMD instructions (0=off, 1=on [default])\n"
        "  -d       Daemonize\n"
        "  -h       Help\n",
        basename,
        progname
    );
    exit(1);
}

static void read_arguments(int argc, char *argv[]) __attribute__((noinline));
static void read_arguments(int argc, char *argv[])
{
    int i, j;
    double k;

    if (argc <= 1)
    {
        return;
    }

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] != 0 && argv[i][2] == 0)
        {
            switch (argv[i][1])
            {
                case 'w': // data files path
                    if ((i + 1) < argc)
                    {
                        i++;
                        cs_bank_directory_path = argv[i];
                    }
                    break;
                case 'p': // polyphony
                    if ((i + 1) < argc)
                    {
                        i++;
                        j = atoi(argv[i]);
                        if (j >= 0 && j <= 128)
                        {
                            cs_number_of_voices = j;
                        }
                    }
                    break;
                case 'e': // effects
                    if ((i + 1) < argc)
                    {
                        i++;
                        j = atoi(argv[i]);
                        if (j >= 0 && j <= 1)
                        {
                            cs_effects_enabled = j;
                        }
                    }
                    break;
                case 'c': // chorus effect
                    if ((i + 1) < argc)
                    {
                        i++;
                        j = atoi(argv[i]);
                        if (j >= 0 && j <= 1)
                        {
                            cs_chorus_enabled = j;
                        }
                    }
                    break;
                case 'r': // reverb effect
                    if ((i + 1) < argc)
                    {
                        i++;
                        j = atoi(argv[i]);
                        if (j >= 0 && j <= 1)
                        {
                            cs_reverb_enabled = j;
                        }
                    }
                    break;
                case 'g': // volume gain in decibels
                    if ((i + 1) < argc)
                    {
                        i++;
                        k = atof(argv[i]);
                        if (k > 0.0 && k <= 24.0)
                        {
                            cs_volume_multiplier = (int32_t)(pow(10.0, k / 20.0) * 2048.0);
                        }
                    }
                    break;
                case 'm': // MIDI mode
                    if ((i + 1) < argc)
                    {
                        i++;
                        j = atoi(argv[i]);
                        if (j >= 0 && j <= 2)
                        {
                            cs_midi_mode = j;
                        }
                    }
                    break;
                case 'k': // sound cache size in kilobytes
                    if ((i + 1) < argc)
                    {
                        i++;
                        j = atoi(argv[i]);
                        if (j >= 2048 && j <= 16384)
                        {
                            cs_sound_cache_size = j;
                        }
                    }
                    break;
                case 'x': // SIMD instructions
                    if ((i + 1) < argc)
                    {
                        i++;
                        j = atoi(argv[i]);
                        if (j >= 0 && j <= 1)
                        {
                            cs_simd_enabled = j;
                        }
                    }
                    break;
                case 'd': // daemonize
                    daemonize = 1;
                    break;
                case 'h': // help
                    usage(argv[0]);
                default:
                    break;
            }
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            usage(argv[0]);
        }
    }
}


static void stop_synth(void)
{
    CS_SynthesizerDeactivate();
    x86_free(event_buffer);
#if !defined(x86_malloc)
    x86_deinit_malloc();
#endif
}

static int start_synth(void) __attribute__((noinline));
static int start_synth(void)
{
#ifdef PTROFS_64BIT
    if (0 != initialize_pointer_offset())
    {
        fprintf(stderr, "Error initializing pointer offset\n");
        return -1;
    }
#endif

#if !defined(x86_malloc)
    if (0 != x86_init_malloc())
    {
        fprintf(stderr, "Error initializing memory allocator\n");
        return -2;
    }
#endif

    event_buffer = (uint8_t *)x86_malloc(65536);
    if (event_buffer == NULL)
    {
        stop_synth();
        fprintf(stderr, "Error allocating memory buffers\n");
        return -3;
    }

#if defined(CLOCK_MONOTONIC_RAW)
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &base_time))
    {
        monotonic_clock_id = CLOCK_MONOTONIC;
        clock_gettime(CLOCK_MONOTONIC, &base_time);
    }
    else
    {
        monotonic_clock_id = CLOCK_MONOTONIC_RAW;
    }
#else
    clock_gettime(MONOTONIC_CLOCK_TYPE, &base_time);
#endif

    // initialize D77
    if (!CS_SynthesizerActivate(NULL, CS_OUTPUT_GenerateAudio, 0, 0, NULL, NULL))
    {
        stop_synth();
        fprintf(stderr, "Error initializing synth\n");
        return -4;
    }

    if (samples_per_call == 0 || bytes_per_call == 0)
    {
        stop_synth();
        fprintf(stderr, "Synth not initialized correctly (datafile error?)\n");
        return 5;
    }


    // prepare output buffer
    num_subbuffers = (4096 * (int64_t)frequency) / (11025 * (int64_t)samples_per_call);
    if (num_subbuffers > 65536 / bytes_per_call)
    {
        num_subbuffers = 65536 / bytes_per_call;
    }
    if (num_subbuffers < 4)
    {
        stop_synth();
        fprintf(stderr, "Unsupported parameters: %i, %i, %i\n", num_channels, frequency, samples_per_call);
        return -6;
    }


    // prepare variables
    event_read_index = 0;
    event_write_index = 0;
    subbuf_counter = 0;
    memset(midi_buffer, 0, 65536);


    return 0;
}

static int run_as_daemon(void) __attribute__((noinline));
static int run_as_daemon(void)
{
    int err;

    printf("Running as daemon...\n");

    err = daemon(0, 0);
    if (err < 0)
    {
        fprintf(stderr, "Error running as daemon: %i\n", err);
        return -1;
    }

    return 0;
}

static int open_midi_port(void) __attribute__((noinline));
static int open_midi_port(void)
{
    int err;
    unsigned int caps, type;

    err = snd_seq_open(&midi_seq, "default", SND_SEQ_OPEN_DUPLEX, 0);
    if (err < 0)
    {
        fprintf(stderr, "Error opening ALSA sequencer: %i\n%s\n", err, snd_strerror(err));
        return -1;
    }

    err = snd_seq_set_client_name(midi_seq, midi_name);
    if (err < 0)
    {
        snd_seq_close(midi_seq);
        fprintf(stderr, "Error setting sequencer client name: %i\n%s\n", err, snd_strerror(err));
        return -2;
    }

    caps = SND_SEQ_PORT_CAP_SUBS_WRITE | SND_SEQ_PORT_CAP_WRITE;
    type = SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_MIDI_GM | SND_SEQ_PORT_TYPE_SYNTHESIZER;
    err = snd_seq_create_simple_port(midi_seq, port_name, caps, type);
    if (err < 0)
    {
        snd_seq_close(midi_seq);
        fprintf(stderr, "Error creating sequencer port: %i\n%s\n", err, snd_strerror(err));
        return -3;
    }
    midi_port_id = err;

    printf("%s ALSA address is %i:0\n", midi_name, snd_seq_client_id(midi_seq));

    return 0;
}

static void close_midi_port(void)
{
    snd_seq_delete_port(midi_seq, midi_port_id);
    snd_seq_close(midi_seq);
}


static int set_hw_params(void)
{
    int err, dir;
    unsigned int rate;
    snd_pcm_uframes_t buffer_size, period_size;
    snd_pcm_hw_params_t *pcm_hwparams;

    snd_pcm_hw_params_alloca(&pcm_hwparams);

    err = snd_pcm_hw_params_any(midi_pcm, pcm_hwparams);
    if (err < 0)
    {
        fprintf(stderr, "Error getting hwparams: %i\n%s\n", err, snd_strerror(err));
        return -1;
    }

    err = snd_pcm_hw_params_set_access(midi_pcm, pcm_hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0)
    {
        fprintf(stderr, "Error setting access: %i\n%s\n", err, snd_strerror(err));
        return -2;
    }

    err = snd_pcm_hw_params_set_format(midi_pcm, pcm_hwparams, SND_PCM_FORMAT_S16);
    if (err < 0)
    {
        fprintf(stderr, "Error setting format: %i\n%s\n", err, snd_strerror(err));
        return -3;
    }

    err = snd_pcm_hw_params_set_channels(midi_pcm, pcm_hwparams, num_channels);
    if (err < 0)
    {
        fprintf(stderr, "Error setting channels: %i\n%s\n", err, snd_strerror(err));
        return -4;
    }

    rate = frequency;
    dir = 0;
    err = snd_pcm_hw_params_set_rate_near(midi_pcm, pcm_hwparams, &rate, &dir);
    if (err < 0)
    {
        fprintf(stderr, "Error setting rate: %i\n%s\n", err, snd_strerror(err));
        return -5;
    }

    buffer_size = samples_per_call * num_subbuffers;
    err = snd_pcm_hw_params_set_buffer_size_near(midi_pcm, pcm_hwparams, &buffer_size);
    if (err < 0)
    {
        fprintf(stderr, "Error setting buffer size: %i\n%s\n", err, snd_strerror(err));
        return -6;
    }

    period_size = samples_per_call;
    dir = 0;
    err = snd_pcm_hw_params_set_period_size_near(midi_pcm, pcm_hwparams, &period_size, &dir);
    if (err < 0)
    {
        fprintf(stderr, "Error setting period size: %i\n%s\n", err, snd_strerror(err));
        return -7;
    }

    err = snd_pcm_hw_params(midi_pcm, pcm_hwparams);
    if (err < 0)
    {
        fprintf(stderr, "Error setting hwparams: %i\n%s\n", err, snd_strerror(err));
        return -8;
    }

    return 0;
}

static int set_sw_params(void)
{
    snd_pcm_sw_params_t *swparams;
    int err;

    snd_pcm_sw_params_alloca(&swparams);

    err = snd_pcm_sw_params_current(midi_pcm, swparams);
    if (err < 0)
    {
        fprintf(stderr, "Error getting swparams: %i\n%s\n", err, snd_strerror(err));
        return -1;
    }

    err = snd_pcm_sw_params_set_avail_min(midi_pcm, swparams, samples_per_call);
    if (err < 0)
    {
        fprintf(stderr, "Error setting avail min: %i\n%s\n", err, snd_strerror(err));
        return -2;
    }

    err = snd_pcm_sw_params(midi_pcm, swparams);
    if (err < 0)
    {
        fprintf(stderr, "Error setting sw params: %i\n%s\n", err, snd_strerror(err));
        return -3;
    }

    return 0;
}

static int open_pcm_output(void) __attribute__((noinline));
static int open_pcm_output(void)
{
    int err;

    err = snd_pcm_open(&midi_pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0)
    {
        fprintf(stderr, "Error opening PCM device: %i\n%s\n", err, snd_strerror(err));
        return -1;
    }

    if (set_hw_params() < 0)
    {
        return -2;
    }

    if (set_sw_params() < 0)
    {
        return -3;
    }

    // set nonblock mode
    snd_pcm_nonblock(midi_pcm, 1);

    snd_pcm_prepare(midi_pcm);

    return 0;
}

static void close_pcm_output(void)
{
    snd_pcm_close(midi_pcm);
}


static int drop_privileges(void)
{
    uid_t uid;
    gid_t gid;
    const char *sudo_id;
    long long int llid;
    const char *xdg_dir;
    char buf[32];
    struct stat statbuf;
    struct passwd *passwdbuf;

    if (getuid() != 0)
    {
        return 0;
    }

    sudo_id = secure_getenv("SUDO_UID");
    if (sudo_id == NULL)
    {
        sudo_id = secure_getenv("PKEXEC_UID");
        if (sudo_id == NULL)
        {
            return -1;
        }
    }

    errno = 0;
    llid = strtoll(sudo_id, NULL, 10);
    uid = (uid_t) llid;
    if (errno != 0 || uid == 0 || llid != (long long int)uid)
    {
        return -2;
    }

    gid = getgid();
    if (gid == 0)
    {
        sudo_id = secure_getenv("SUDO_GID");
        if (sudo_id == NULL)
        {
            passwdbuf = getpwuid(uid);
            if (passwdbuf != NULL)
            {
                gid = passwdbuf->pw_gid;
            }

            if (gid == 0)
            {
                return -3;
            }
        }
        else
        {
            errno = 0;
            llid = strtoll(sudo_id, NULL, 10);
            gid = (gid_t) llid;
            if (errno != 0 || gid == 0 || llid != (long long int)gid)
            {
                return -4;
            }
        }
    }

    if (setgid(gid) != 0)
    {
        return -5;
    }
    if (setuid(uid) != 0)
    {
        return -6;
    }

    printf("Dropped root privileges\n");

    if (0 != chdir("/")) {}

    // define some environment variables

    xdg_dir = getenv("XDG_RUNTIME_DIR");
    if ((xdg_dir == NULL) || (*xdg_dir == 0))
    {
        snprintf(buf, 32, "/run/user/%lli", (long long int)uid);

        if ((stat(buf, &statbuf) == 0) && ((statbuf.st_mode & S_IFMT) == S_IFDIR) && (statbuf.st_uid == uid))
        {
            // if XDG_RUNTIME_DIR is not defined and directory /run/user/$USER exists then use it for XDG_RUNTIME_DIR
            setenv("XDG_RUNTIME_DIR", buf, 1);

            xdg_dir = getenv("XDG_CONFIG_HOME");
            if ((xdg_dir == NULL) || (*xdg_dir == 0))
            {
                passwdbuf = getpwuid(uid);
                if (passwdbuf != NULL)
                {
                    // also if XDG_CONFIG_HOME is not defined then define it as user's home directory
                    setenv("XDG_CONFIG_HOME", passwdbuf->pw_dir, 1);
                }
            }
        }
    }

    return 0;
}

static int start_thread(void) __attribute__((noinline));
static int start_thread(void)
{
    pthread_attr_t attr;
    int err;
    volatile int initialized;

    // try to increase priority (only root)
    if (-1 == nice(-20)) {}

    err = pthread_attr_init(&attr);
    if (err != 0)
    {
        fprintf(stderr, "Error creating thread attribute: %i\n", err);
        return -1;
    }

    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    midi_init_state = 0;
    initialized = 0;
    err = pthread_create(&midi_thread, &attr, &midi_thread_proc, (void *)&initialized);
    pthread_attr_destroy(&attr);

    if (err != 0)
    {
        fprintf(stderr, "Error creating thread: %i\n", err);
        return -2;
    }

    // wait for thread initialization
    while (initialized == 0)
    {
        struct timespec req;

        req.tv_sec = 0;
        req.tv_nsec = 10000000;
        nanosleep(&req, NULL);
    };

    if (drop_privileges() < 0)
    {
        fprintf(stderr, "Error dropping root privileges\n");
    }

    return 0;
}


static void render_audiodata(void)
{
    int read_index, write_index;

    // read global volatile variables to local variables
    read_index = event_read_index;
    write_index = event_write_index;

    if (read_index != write_index)
    {
        // read events from buffer
        if (read_index < write_index)
        {
            CS_SynthesizerMidi(&(event_buffer[read_index]), write_index - read_index);
        }
        else
        {
            CS_SynthesizerMidi(&(event_buffer[read_index]), 65536 - read_index);
            CS_SynthesizerMidi(&(event_buffer[0]), write_index);
        }

        // update global volatile variable
        event_read_index = write_index;
    }

    // render audio data
    CS_ProcessEventsAndGenerateAudio();
}

static int output_subbuffer(int num)
{
    snd_pcm_uframes_t remaining;
    snd_pcm_sframes_t written;
    uint8_t *buf_ptr;

    buf_ptr = &(midi_buffer[num * bytes_per_call]);
    memcpy(buf_ptr, audio_data, bytes_per_call);

    remaining = samples_per_call;
    while (remaining)
    {
        written = snd_pcm_writei(midi_pcm, buf_ptr, remaining);
        if (written < 0)
        {
            return -1;
        }

        remaining -= written;
        buf_ptr += written << 2;
    };

    return 0;
}

static void main_loop(void) __attribute__((noinline));
static void main_loop(void)
{
    int is_paused;
    struct timespec last_written_time, current_time;

    for (unsigned int i = 2; i < num_subbuffers; i++)
    {
        output_subbuffer(i);
    }

    is_paused = 0;
    // pause pcm playback at the beginning
    if (0 == snd_pcm_pause(midi_pcm, 1))
    {
        is_paused = 1;
        printf("PCM playback paused\n");
    }
    else
    {
        // if pausing doesn't work then set time of last written event as current time, so the next attempt to pause will be in 60 seconds
        clock_gettime(MONOTONIC_CLOCK_TYPE, &last_written_time);
    }

    midi_event_written = 0;
    midi_init_state = 1;

    while (1)
    {
        struct timespec req;
        snd_pcm_state_t pcmstate;
        snd_pcm_sframes_t available_frames;

        req.tv_sec = 0;
        req.tv_nsec = 10000000;
        nanosleep(&req, NULL);

        if (midi_event_written)
        {
            midi_event_written = 0;

            // remember time of last written event
            clock_gettime(MONOTONIC_CLOCK_TYPE, &last_written_time);

            if (is_paused)
            {
                is_paused = 0;
                snd_pcm_pause(midi_pcm, 0);
                printf("PCM playback unpaused\n");
            }
        }
        else
        {
            if (is_paused)
            {
                continue;
            }

            clock_gettime(MONOTONIC_CLOCK_TYPE, &current_time);
            // if more than 60 seconds elapsed from last written event, then pause pcm playback
            if (current_time.tv_sec - last_written_time.tv_sec > 60)
            {
                if (0 == snd_pcm_pause(midi_pcm, 1))
                {
                    is_paused = 1;
                    printf("PCM playback paused\n");
                    continue;
                }
                else
                {
                    // if pausing doesn't work then set time of last written event as current time, so the next attempt to pause will be in 60 seconds
                    last_written_time = current_time;
                }
            }
        }

        pcmstate = snd_pcm_state(midi_pcm);
        if (pcmstate == SND_PCM_STATE_XRUN)
        {
            fprintf(stderr, "Buffer underrun\n");
            snd_pcm_prepare(midi_pcm);
        }

        available_frames = snd_pcm_avail_update(midi_pcm);
        while (available_frames >= (3 * samples_per_call))
        {
            render_audiodata();

            if (output_subbuffer(subbuf_counter) < 0)
            {
                fprintf(stderr, "Error writing audio data\n");
                available_frames = 0;
                break;
            }
            else
            {
                available_frames -= samples_per_call;
            }

            subbuf_counter++;
            if (subbuf_counter == num_subbuffers)
            {
                subbuf_counter = 0;
            }
        };
    };
}

int main(int argc, char *argv[])
{
    if (sizeof(PTR32(void)) != 4)
    {
        fprintf(stderr, "Error: The program wasn't compiled correctly for %i-bits\n", (int) (8 * sizeof(void*)));
        return 0;
    }

    read_arguments(argc, argv);

    if (start_synth() < 0)
    {
        return 2;
    }

    if (daemonize)
    {
        if (run_as_daemon() < 0)
        {
            stop_synth();
            return 3;
        }
    }

    if (start_thread() < 0)
    {
        stop_synth();
        return 4;
    }

    if (open_pcm_output() < 0)
    {
        midi_init_state = -1;
        stop_synth();
        return 5;
    }

    if (open_midi_port() < 0)
    {
        midi_init_state = -1;
        close_pcm_output();
        stop_synth();
        return 6;
    }

    main_loop();

    midi_init_state = -1;
    close_midi_port();
    close_pcm_output();
    stop_synth();
    return 0;
}

