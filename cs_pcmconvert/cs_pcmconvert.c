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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "midi_loader.h"

#include "memory32.h"
#include "ptr32.h"
#include "synth.h"

#if (defined(__WIN32__) || defined(__WINDOWS__)) && !defined(_WIN32)
#define _WIN32
#endif

#ifdef _WIN32
    #include <io.h>
    #include <fcntl.h>
#else
    #include <sys/types.h>
    #include <dirent.h>
#endif

#if defined(_MSC_VER)

#undef BIG_ENDIAN_BYTE_ORDER

#elif defined(__BYTE_ORDER__)

#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define BIG_ENDIAN_BYTE_ORDER
#else
#undef BIG_ENDIAN_BYTE_ORDER
#endif

#else

#include <endian.h>
#if (__BYTE_ORDER == __BIG_ENDIAN)
#define BIG_ENDIAN_BYTE_ORDER
#else
#undef BIG_ENDIAN_BYTE_ORDER
#endif

#endif

#if defined(__GNUC__)
#define INLINE __inline__
#elif defined(_MSC_VER)
#define INLINE __inline
#else
#define INLINE inline
#endif


static const char *arg_input = NULL;
static const char *arg_output = NULL;
static int wav_to_file = 1;

static uint32_t current_time;

static unsigned int timediv;
static midi_event_info *midi_events;

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

#if !defined(x86_malloc)
static uint8_t *input_buffer;
#endif

const static unsigned int frequency = 22050;


#ifdef __cplusplus
extern "C"
#endif
uint32_t CCALL CS_GetTime(void)
{
    return current_time;
}


static INLINE void WRITE_LE_UINT16(uint8_t *ptr, uint16_t value)
{
    ptr[0] = value & 0xff;
    ptr[1] = (value >> 8) & 0xff;
}

static INLINE void WRITE_LE_UINT32(uint8_t *ptr, uint32_t value)
{
    ptr[0] = value & 0xff;
    ptr[1] = (value >> 8) & 0xff;
    ptr[2] = (value >> 16) & 0xff;
    ptr[3] = (value >> 24) & 0xff;
}


static void usage(const char *progname)
{
    static const char basename[] = "cs_pcmconvert";

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

#ifdef _WIN32
        slash = strrchr(progname, '\\');
        if (slash != NULL)
        {
            progname = slash + 1;
        }
#endif
    }

    printf(
        "%s - CyberSynth pcm convert\n"
        "Usage: %s [OPTIONS]...\n"
        "  -i PATH  Input path (path to .mid)\n"
        "  -s       Output raw data do stdout\n"
        "  -o PATH  Output path (path to .wav)\n"
        "  -w PATH  Datafiles path (path to directory with *.iss files)\n"
        "  -p NUM   Polyphony (0-128 [default=32])\n"
        "  -e NUM   Effects (0=off, 1=on [default])\n"
        "  -c NUM   Chorus effect (0=off, 1=on [default])\n"
        "  -r NUM   Reverb effect (0=off, 1=on [default])\n"
        "  -g FLT   Volume gain in decibels (0.0-24.0 [default=0.0])\n"
        "  -m NUM   MIDI mode (0=CyberSound, 1=GM [default], 2=GS)\n"
        "  -k NUM   Sound cache size in kilobytes (2048-16384 [default]=8192)\n"
        "  -x NUM   SIMD instructions (0=off, 1=on [default])\n"
        "  -h       Help\n",
        basename,
        progname
    );
    exit(1);
}

int main(int argc, char *argv[])
{
    int return_value;

    if (sizeof(PTR32(void)) != 4)
    {
        fprintf(stderr, "Error: The program wasn't compiled correctly for %i-bits\n", (int) (8 * sizeof(void*)));
        return 0;
    }

    // parse arguments
    if (argc > 1)
    {
        int i, j;
        double k;
        for (i = 1; i < argc; i++)
        {
            if (argv[i][0] == '-' && argv[i][1] != 0 && argv[i][2] == 0)
            {
                switch (argv[i][1])
                {
                    case 'i': // input
                        if ((i + 1) < argc)
                        {
                            i++;
                            arg_input = argv[i];
                        }
                        break;
                    case 'o': // output
                        if ((i + 1) < argc)
                        {
                            i++;
                            arg_output = argv[i];
                        }
                        break;
                    case 'w': // data files path
                        if ((i + 1) < argc)
                        {
                            i++;
                            cs_bank_directory_path = argv[i];
                        }
                        break;
                    case 's': // stdout
                        wav_to_file = 0;
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

    if (arg_input == NULL)
    {
        fprintf(stderr, "no input file\n");
        usage(argv[0]);
    }
    if (wav_to_file && arg_output == NULL)
    {
        fprintf(stderr, "no output file\n");
        usage(argv[0]);
    }

#ifdef PTROFS_64BIT
    if (0 != initialize_pointer_offset())
    {
        fprintf(stderr, "error initializing pointer offset\n");
        return 1;
    }
#endif

#if !defined(x86_malloc)
    if (0 != x86_init_malloc())
    {
        fprintf(stderr, "error initializing memory allocator\n");
        return 1;
    }
#endif

#if !defined(x86_malloc)
    if (sizeof(void *) > 4)
    {
        input_buffer = (uint8_t *)x86_malloc(65536);
        if (input_buffer == NULL)
        {
            fprintf(stderr, "error allocating input buffer\n");
            return 2;
        }
    }
#endif

    // load MIDI file
    if (load_midi_file(arg_input, &timediv, &midi_events))
    {
        fprintf(stderr, "error loading MIDI file\n");
        return 4;
    }


    if (!CS_SynthesizerActivate(NULL, CS_OUTPUT_GenerateAudio, 0, 0, NULL, NULL))
    {
        fprintf(stderr, "error initializing synth\n");
        return 6;
    }

    if (samples_per_call == 0 || bytes_per_call == 0)
    {
        fprintf(stderr, "synth not initialized correctly (datafile error?)\n");
        return 7;
    }

    return_value = 0;
    current_time = 0;

    // play midi
    {
        unsigned int num_calls, remaining_events;
        midi_event_info *cur_event;

        FILE *fout;

        if (wav_to_file)
        {
            uint8_t wav_header[44];
            uint8_t *header_ptr;

#if (defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__) || (defined(__MINGW32__) && defined(_UCRT)) || (defined(__STDC_LIB_EXT1__) && __STDC_WANT_LIB_EXT1__)
            if (fopen_s(&fout, arg_output, "wb"))
#else
            fout = fopen(arg_output, "wb");
            if (fout == NULL)
#endif
            {
                fprintf(stderr, "error opening output file\n");
                return 8;
            }

            // wav header
            header_ptr = wav_header;
            WRITE_LE_UINT32(header_ptr, 0x46464952);        // "RIFF" tag
            WRITE_LE_UINT32(header_ptr + 4, 36);            // RIFF length - filled later
            WRITE_LE_UINT32(header_ptr + 8, 0x45564157);    // "WAVE" tag
            header_ptr += 12;

            // fmt chunk
            WRITE_LE_UINT32(header_ptr, 0x20746D66);    // "fmt " tag
            WRITE_LE_UINT32(header_ptr + 4, 16);        // chunk length
            header_ptr += 8;

            // PCMWAVEFORMAT structure
            WRITE_LE_UINT16(header_ptr, 1);                 // wFormatTag - 1 = PCM
            WRITE_LE_UINT16(header_ptr + 2, 2);             // nChannels - 2 = stereo
            WRITE_LE_UINT32(header_ptr + 4, frequency);     // nSamplesPerSec
            WRITE_LE_UINT32(header_ptr + 8, 4 * frequency); // nAvgBytesPerSec
            WRITE_LE_UINT16(header_ptr + 12, 4);            // nBlockAlign
            WRITE_LE_UINT16(header_ptr + 14, 16);           // wBitsPerSample
            header_ptr += 16;

            // data chunk
            WRITE_LE_UINT32(header_ptr, 0x61746164);    // "data" tag
            WRITE_LE_UINT32(header_ptr + 4, 0);         // chunk length - filled later
            header_ptr += 8;

            if (fwrite(wav_header, 1, 44, fout) != 44)
            {
                fprintf(stderr, "error writing to output file\n");
                return 9;
            }
        }
        else
        {
#ifdef _WIN32
            if (_setmode(_fileno(stdout), _O_BINARY) == -1)
            {
                fprintf(stderr, "error changing output mode to binary\n");
                return 9;
            }
#endif
            fout = stdout;
        }

        num_calls = 0;
        remaining_events = midi_events[0].len;
        cur_event = midi_events + 1;
        while (current_time < midi_events[0].time + 112)
        {
            uint32_t next_time;

            num_calls++;

            next_time = ((num_calls * samples_per_call + (samples_per_call >> 1)) * (uint64_t)1000) / frequency;
            while ((remaining_events > 0) && (cur_event->time <= next_time))
            {
                if (cur_event->len <= 8)
                {
                    if (cur_event->data[0] != 0xff) // skip meta events
                    {
#if !defined(x86_malloc)
                        if (sizeof(void *) > 4)
                        {
                            memcpy(input_buffer, cur_event->data, cur_event->len);
                            CS_SynthesizerMidi(input_buffer, cur_event->len);
                        }
                        else
#endif
                        {
                            CS_SynthesizerMidi(cur_event->data, cur_event->len);
                        }
                    }
                }
                else
                {
                    if (cur_event->sysex[0] != 0xff) // skip meta events
                    {
                        if (cur_event->len <= 18) // max supported length of sysex event
                        {
#if !defined(x86_malloc)
                            if (sizeof(void *) > 4)
                            {
                                if (cur_event->len <= 65536)
                                {
                                    memcpy(input_buffer, cur_event->sysex, cur_event->len);
                                    CS_SynthesizerMidi(input_buffer, cur_event->len);
                                }
                            }
                            else
#endif
                            {
                                CS_SynthesizerMidi(cur_event->sysex, cur_event->len);
                            }
                        }
                    }
                }

                cur_event++;
                remaining_events--;
            }

            current_time = next_time;

            CS_ProcessEventsAndGenerateAudio();

#ifdef BIG_ENDIAN_BYTE_ORDER
            // swap values to little-endian
            {
                int i;
                for (i = 0; i < bytes_per_call; i += 2)
                {
                    uint8_t value;
                    value = audio_data[i];
                    audio_data[i] = audio_data[i + 1];
                    audio_data[i + 1] = value;
                }
            }
#endif

            if (fwrite(audio_data, 1, bytes_per_call, fout) != bytes_per_call)
            {
                fprintf(stderr, "error writing to output file\n");
                return_value = 9;
                break;
            }
        }

        if (wav_to_file)
        {
            uint8_t chunk_length[4];

            // RIFF length
            WRITE_LE_UINT32(chunk_length, 36 + num_calls * bytes_per_call);
            fseek(fout, 4, SEEK_SET);
            if (fwrite(chunk_length, 4, 1, fout) != 1)
            {
                fprintf(stderr, "error writing to output file\n");
                return 9;
            }

            // data chunk length
            WRITE_LE_UINT32(chunk_length, num_calls * bytes_per_call);
            fseek(fout, 40, SEEK_SET);
            if (fwrite(chunk_length, 4, 1, fout) != 1)
            {
                fprintf(stderr, "error writing to output file\n");
                return 9;
            }

            fclose(fout);
        }
        else
        {
            fflush(fout);
            fout = NULL;
        }
    }

    CS_SynthesizerDeactivate();

    // free MIDI file
#if !defined(x86_malloc)
    if (sizeof(void *) > 4)
    {
        x86_free(input_buffer);
    }
#endif
    free_midi_data(midi_events);

#if !defined(x86_malloc)
    x86_deinit_malloc();
#endif

    return return_value;
}

