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

#include <AvailabilityMacros.h>
#include <AudioToolbox/AudioToolbox.h>
#include <CoreMIDI/CoreMIDI.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/mman.h>
#include <pwd.h>
#include <libproc.h>
#include <signal.h>
#include <spawn.h>
#include <crt_externs.h>
#include "memory32.h"
#include "ptr32.h"
#include "synth.h"

#if defined(MAC_OS_VERSION_11_0) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_VERSION_11_0
#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_VERSION_11_0
#define MIDI_API 0
#else
#define MIDI_API 1
#endif
#else
#define MIDI_API -1
#endif


#define MIDI_NAME "CyberSynth"

static const uint32_t midi_name_crc32 = 0xe45a20aa;

static MIDIClientRef midi_client;
static MIDIEndpointRef midi_endpoint;
static AudioQueueRef midi_pcm_queue;
static volatile int midi_event_written;
static uint8_t midi_running_status;

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

static uint64_t base_time;

static uint8_t *event_buffer;
static struct AudioQueueBuffer **midi_queue_buffer;

static volatile int event_read_index;
static volatile int event_write_index;


#ifdef __cplusplus
extern "C"
#endif
uint32_t CCALL CS_GetTime(void)
{
    uint64_t current_time;

    current_time = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);

    return (uint32_t)((current_time - base_time) / 1000000ul);
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

#if (MIDI_API <= 0)
static void midi_read_proc(const MIDIPacketList *pktlist, void * __nullable readProcRefCon, void * __nullable srcConnRefCon)
{
    unsigned int index1, index2, length;
    uint8_t data[4];
    const MIDIPacket *packet;
    uint8_t *running_status;

    running_status = (uint8_t *)readProcRefCon;
    for (index1 = 0, packet = pktlist->packet; index1 < pktlist->numPackets; index1++, packet = MIDIPacketNext(packet))
    {
        for (index2 = 0; index2 < packet->length; index2++)
        {
            switch ((packet->data[index2] >> 4) & 0x0f) // status
            {
                case 0x08:
                    // send note off event as note on with zero velocity to increase the chance of using running status
                    data[0] = 0x10 | packet->data[index2];
                    data[1] = packet->data[index2 + 1] & 0x7f;
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
                    printf("Note OFF, channel:%d note:%d velocity:%d\n", packet->data[index2] & 0x0f, packet->data[index2 + 1] & 0x7f, packet->data[index2 + 2] & 0x7f);
#endif

                    index2 += 2;
                    break;

                case 0x09:
                    data[0] = packet->data[index2];
                    data[1] = packet->data[index2 + 1] & 0x7f;
                    data[2] = packet->data[index2 + 2] & 0x7f;
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
                    printf("Note ON, channel:%d note:%d velocity:%d\n", packet->data[index2] & 0x0f, packet->data[index2 + 1] & 0x7f, packet->data[index2 + 2] & 0x7f);
#endif

                    index2 += 2;
                    break;

                case 0x0a:
                    // Not used by CyberSynth
#if 0
                    data[0] = packet->data[index2];
                    data[1] = packet->data[index2 + 1] & 0x7f;
                    data[2] = packet->data[index2 + 2] & 0x7f;
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
                    printf("Keypress, channel:%d note:%d velocity:%d\n", packet->data[index2] & 0x0f, packet->data[index2 + 1] & 0x7f, packet->data[index2 + 2] & 0x7f);
#endif

                    index2 += 2;
                    break;

                case 0x0b:
                    data[0] = packet->data[index2];
                    data[1] = packet->data[index2 + 1] & 0x7f;
                    data[2] = packet->data[index2 + 2] & 0x7f;
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
                    printf("Controller, channel:%d param:%d value:%d\n", packet->data[index2] & 0x0f, packet->data[index2 + 1] & 0x7f, packet->data[index2 + 2] & 0x7f);
#endif

                    index2 += 2;
                    break;

                case 0x0c:
                    data[0] = packet->data[index2];
                    data[1] = packet->data[index2 + 1] & 0x7f;
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
                    printf("Program change, channel:%d value:%d\n", packet->data[index2] & 0x0f, packet->data[index2 + 1] & 0x7f);
#endif

                    index2++;
                    break;

                case 0x0d:
                    data[0] = packet->data[index2];
                    data[1] = packet->data[index2 + 1] & 0x7f;
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
                    printf("Channel pressure, channel:%d value:%d\n", packet->data[index2] & 0x0f, packet->data[index2 + 1] & 0x7f);
#endif

                    index2++;
                    break;

                case 0x0e:
                    data[0] = packet->data[index2];
                    data[1] = packet->data[index2 + 1] & 0x7f;
                    data[2] = packet->data[index2 + 2] & 0x7f;
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
                    printf("Pitch bend, channel:%d value:%d\n", packet->data[index2] & 0x0f, ((packet->data[index2 + 1] & 0x7f) | ((packet->data[index2 + 2] & 0x7f) << 7)) - 0x2000);
#endif

                    index2 += 2;
                    break;

                case 0x0f:
                    switch (packet->data[index2])
                    {
                        case 0xf0:
                            *running_status = 0;
                            write_event(packet->data + index2, packet->length - index2);

#ifdef PRINT_EVENTS
                            printf("SysEx (fragment), length:%d\n", packet->length - index2);
#endif

                            index2 = packet->length - 1;
                            break;

                        case 0xf1:
                            // Not used by CyberSynth
#if 0
                            data[0] = packet->data[index2];
                            data[1] = packet->data[index2 + 1] & 0x7f;
                            length = 2;

                            *running_status = 0;
                            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
                            printf("MTC Quarter Frame, value:%d\n", packet->data[index2 + 1] & 0x7f);
#endif

                            index2++;
                            break;

                        case 0xf2:
                            // Not used by CyberSynth
#if 0
                            data[0] = packet->data[index2];
                            data[1] = packet->data[index2 + 1] & 0x7f;
                            data[2] = packet->data[index2 + 2] & 0x7f;
                            length = 3;

                            *running_status = 0;
                            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
                            printf("Song Position, value:%d\n", ((packet->data[index2 + 1] & 0x7f) | ((packet->data[index2 + 2] & 0x7f) << 7)) - 0x2000);
#endif

                            index2 += 2;
                            break;

                        case 0xf3:
                            // Not used by CyberSynth
#if 0
                            data[0] = packet->data[index2];
                            data[1] = packet->data[index2 + 1] & 0x7f;
                            length = 2;

                            *running_status = 0;
                            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
                            printf("Song Select, value:%d\n", packet->data[index2 + 1] & 0x7f);
#endif

                            index2++;
                            break;

                        case 0xf6:
                            // Not used by CyberSynth
#if 0
                            data[0] = packet->data[index2];
                            length = 1;

                            *running_status = 0;
                            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
                            printf("Tune Request\n");
#endif

                            break;

                        case 0xf8:
                            // Not used by CyberSynth
#if 0
                            data[0] = packet->data[index2];
                            length = 1;

                            *running_status = 0;
                            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
                            printf("Clock\n");
#endif

                            break;

                        case 0xfa:
                            // Not used by CyberSynth
#if 0
                            data[0] = packet->data[index2];
                            length = 1;

                            *running_status = 0;
                            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
                            printf("Start\n");
#endif

                            break;

                        case 0xfb:
                            // Not used by CyberSynth
#if 0
                            data[0] = packet->data[index2];
                            length = 1;

                            *running_status = 0;
                            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
                            printf("Continue\n");
#endif

                            break;

                        case 0xfc:
                            // Not used by CyberSynth
#if 0
                            data[0] = packet->data[index2];
                            length = 1;

                            *running_status = 0;
                            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
                            printf("Stop\n");
#endif

                            break;

                        case 0xfe:
                            // Not used by CyberSynth
#if 0
                            data[0] = packet->data[index2];
                            length = 1;

                            *running_status = 0;
                            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
                            printf("Active Sense\n");
#endif

                            break;

                        case 0xff:
                            // Not used by CyberSynth
#if 0
                            data[0] = packet->data[index2];
                            length = 1;

                            *running_status = 0;
                            write_event(data, length);
#endif

#ifdef PRINT_EVENTS
                            printf("Reset\n");
#endif

                            break;

                        default:
                            fprintf(stderr, "Unhandled system message: 0x%x\n", packet->data[index2]);
                            break;
                    }
                    break;
                default:
                    if (index2 == 0)
                    {
                        *running_status = 0;
                        write_event(packet->data, packet->length);

#ifdef PRINT_EVENTS
                        printf("SysEx (fragment) of size %d\n", length);
#endif

                        index2 = packet->length - 1;
                    }
                    else
                    {
                        fprintf(stderr, "Unhandled message: 0x%x\n", packet->data[index2]);
                    }
                    break;
            }
        }
    }
}
#endif

#if (MIDI_API >= 0)
static void midi_receive_proc(const MIDIEventList *evtlist, uint8_t *running_status, void * __nullable srcConnRefCon)
{
    unsigned int index1, index2, length;
    uint8_t data[8];
    const MIDIEventPacket *packet;

    for (index1 = 0, packet = evtlist->packet; index1 < evtlist->numPackets; index1++, packet = MIDIEventPacketNext(packet))
    {
        for (index2 = 0; index2 < packet->wordCount; index2++)
        {
            switch ((packet->words[index2] >> 28) & 0x0f) // message type
            {
                // Traditional MIDI 1.0 Functionality
                case kMIDIMessageTypeSystem:
                    switch ((packet->words[index2] >> 16) & 0xff) // status
                    {
                        case kMIDIStatusMTC:
                            // Not used by CyberSynth
#if 0
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                data[0] = 0xF1;
                                data[1] = (packet->words[index2] >> 8) & 0x7f;
                                length = 2;

                                *running_status = 0;
                                write_event(data, length);
                            }
#endif

#ifdef PRINT_EVENTS
                            printf("MTC Quarter Frame, group:%d value:%d\n", (packet->words[index2] >> 24) & 0x0f, (packet->words[index2] >> 8) & 0x7f);
#endif

                            break;

                        case kMIDIStatusSongPosPointer:
                            // Not used by CyberSynth
#if 0
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                data[0] = 0xF2;
                                data[1] = (packet->words[index2] >> 8) & 0x7f;
                                data[2] = packet->words[index2] & 0x7f;
                                length = 3;

                                *running_status = 0;
                                write_event(data, length);
                            }
#endif

#ifdef PRINT_EVENTS
                            printf("Song Position, group:%d value:%d\n", (packet->words[index2] >> 24) & 0x0f, (((packet->words[index2] >> 8) & 0x7f) | ((packet->words[index2] & 0x7f) << 7)) - 0x2000);
#endif

                            break;

                        case kMIDIStatusSongSelect:
                            // Not used by CyberSynth
#if 0
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                data[0] = 0xF3;
                                data[1] = (packet->words[index2] >> 8) & 0x7f;
                                length = 2;

                                *running_status = 0;
                                write_event(data, length);
                            }
#endif

#ifdef PRINT_EVENTS
                            printf("Song Select, group:%d value:%d\n", (packet->words[index2] >> 24) & 0x0f, (packet->words[index2] >> 8) & 0x7f);
#endif

                            break;

                        case kMIDIStatusTuneRequest:
                            // Not used by CyberSynth
#if 0
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                data[0] = 0xF6;
                                length = 1;

                                *running_status = 0;
                                write_event(data, length);
                            }
#endif

#ifdef PRINT_EVENTS
                            printf("Tune Request, group:%d\n", (packet->words[index2] >> 24) & 0x0f);
#endif

                            break;

                        case kMIDIStatusTimingClock:
                            // Not used by CyberSynth
#if 0
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                data[0] = 0xF8;
                                length = 1;

                                *running_status = 0;
                                write_event(data, length);
                            }
#endif

#ifdef PRINT_EVENTS
                            printf("Clock, group:%d\n", (packet->words[index2] >> 24) & 0x0f);
#endif

                            break;

                        case kMIDIStatusStart:
                            // Not used by CyberSynth
#if 0
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                data[0] = 0xFA;
                                length = 1;

                                *running_status = 0;
                                write_event(data, length);
                            }
#endif

#ifdef PRINT_EVENTS
                            printf("Start, group:%d\n", (packet->words[index2] >> 24) & 0x0f);
#endif

                            break;

                        case kMIDIStatusContinue:
                            // Not used by CyberSynth
#if 0
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                data[0] = 0xFB;
                                length = 1;

                                *running_status = 0;
                                write_event(data, length);
                            }
#endif

#ifdef PRINT_EVENTS
                            printf("Continue, group:%d\n", (packet->words[index2] >> 24) & 0x0f);
#endif

                            break;

                        case kMIDIStatusStop:
                            // Not used by CyberSynth
#if 0
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                data[0] = 0xFC;
                                length = 1;

                                *running_status = 0;
                                write_event(data, length);
                            }
#endif

#ifdef PRINT_EVENTS
                            printf("Stop, group:%d\n", (packet->words[index2] >> 24) & 0x0f);
#endif

                            break;

                        case kMIDIStatusActiveSending:
                            // Not used by CyberSynth
#if 0
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                data[0] = 0xFE;
                                length = 1;

                                *running_status = 0;
                                write_event(data, length);
                            }
#endif

#ifdef PRINT_EVENTS
                            printf("Active Sense, group:%d\n", (packet->words[index2] >> 24) & 0x0f);
#endif

                            break;

                        case kMIDIStatusSystemReset:
                            // Not used by CyberSynth
#if 0
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                data[0] = 0xFF;
                                length = 1;

                                *running_status = 0;
                                write_event(data, length);
                            }
#endif

#ifdef PRINT_EVENTS
                            printf("Reset, group:%d\n", (packet->words[index2] >> 24) & 0x0f);
#endif

                            break;

                        default:
                            fprintf(stderr, "Unhandled system message: 0x%x, group:%d\n", (packet->words[index2] >> 16) & 0xff, (packet->words[index2] >> 24) & 0x0f);
                            break;
                    }
                    break;

                case kMIDIMessageTypeChannelVoice1:
                    switch ((packet->words[index2] >> 20) & 0x0f) // status
                    {
                        case kMIDICVStatusNoteOff:
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                // send note off event as note on with zero velocity to increase the chance of using running status
                                data[0] = 0x10 | (packet->words[index2] >> 16);
                                data[1] = (packet->words[index2] >> 8) & 0x7f;
                                data[2] = packet->words[index2] & 0x7f;
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
                            }

#ifdef PRINT_EVENTS
                            printf("Note OFF, group:%d channel:%d note:%d velocity:%d\n", (packet->words[index2] >> 24) & 0x0f, (packet->words[index2] >> 16) & 0x0f, (packet->words[index2] >> 8) & 0x7f, packet->words[index2] & 0x7f);
#endif

                            break;

                        case kMIDICVStatusNoteOn:
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                data[0] = packet->words[index2] >> 16;
                                data[1] = (packet->words[index2] >> 8) & 0x7f;
                                data[2] = packet->words[index2] & 0x7f;
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
                            }

#ifdef PRINT_EVENTS
                            printf("Note ON, group:%d channel:%d note:%d velocity:%d\n", (packet->words[index2] >> 24) & 0x0f, (packet->words[index2] >> 16) & 0x0f, (packet->words[index2] >> 8) & 0x7f, packet->words[index2] & 0x7f);
#endif

                            break;

                        case kMIDICVStatusPolyPressure:
                            // Not used by CyberSynth
#if 0
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                data[0] = packet->words[index2] >> 16;
                                data[1] = (packet->words[index2] >> 8) & 0x7f;
                                data[2] = packet->words[index2] & 0x7f;
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
                            }
#endif

#ifdef PRINT_EVENTS
                            printf("Keypress, group:%d channel:%d note:%d velocity:%d\n", (packet->words[index2] >> 24) & 0x0f, (packet->words[index2] >> 16) & 0x0f, (packet->words[index2] >> 8) & 0x7f, packet->words[index2] & 0x7f);
#endif

                            break;

                        case kMIDICVStatusControlChange:
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                data[0] = packet->words[index2] >> 16;
                                data[1] = (packet->words[index2] >> 8) & 0x7f;
                                data[2] = packet->words[index2] & 0x7f;
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
                            }

#ifdef PRINT_EVENTS
                            printf("Controller, group:%d channel:%d param:%d value:%d\n", (packet->words[index2] >> 24) & 0x0f, (packet->words[index2] >> 16) & 0x0f, (packet->words[index2] >> 8) & 0x7f, packet->words[index2] & 0x7f);
#endif

                            break;

                        case kMIDICVStatusProgramChange:
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                data[0] = packet->words[index2] >> 16;
                                data[1] = (packet->words[index2] >> 8) & 0x7f;
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
                            }

#ifdef PRINT_EVENTS
                            printf("Program change, group:%d channel:%d value:%d\n", (packet->words[index2] >> 24) & 0x0f, (packet->words[index2] >> 16) & 0x0f, (packet->words[index2] >> 8) & 0x7f);
#endif

                            break;

                        case kMIDICVStatusChannelPressure:
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                data[0] = packet->words[index2] >> 16;
                                data[1] = (packet->words[index2] >> 8) & 0x7f;
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
                            }

#ifdef PRINT_EVENTS
                            printf("Channel pressure, group:%d channel:%d value:%d\n", (packet->words[index2] >> 24) & 0x0f, (packet->words[index2] >> 16) & 0x0f, (packet->words[index2] >> 8) & 0x7f);
#endif

                            break;

                        case kMIDICVStatusPitchBend:
                            if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                            {
                                data[0] = packet->words[index2] >> 16;
                                data[1] = (packet->words[index2] >> 8) & 0x7f;
                                data[2] = packet->words[index2] & 0x7f;
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
                            }

#ifdef PRINT_EVENTS
                            printf("Pitch bend, group:%d channel:%d value:%d\n", (packet->words[index2] >> 24) & 0x0f, (packet->words[index2] >> 16) & 0x0f, (((packet->words[index2] >> 8) & 0x7f) | ((packet->words[index2] & 0x7f) << 7)) - 0x2000);
#endif

                            break;

                        default:
                            fprintf(stderr, "Unhandled channel voice message: 0x%x, group:%d\n", (packet->words[index2] >> 16) & 0xff, (packet->words[index2] >> 24) & 0x0f);
                            break;
                    }
                    break;

                case kMIDIMessageTypeSysEx:
                    if (((packet->words[index2] >> 24) & 0x0f) == 0) // group
                    {
                        if (((packet->words[index2] >> 20) & 0x0f) == kMIDISysExStatusComplete || ((packet->words[index2] >> 20) & 0x0f) == kMIDISysExStatusStart)
                        {
                            data[0] = 0xf0;
                            length = 1;
                        }
                        else length = 0;

                        switch ((packet->words[index2] >> 16) & 0x0f)
                        {
                            case 6:
                                data[length + 5] = packet->words[index2 + 1] & 0x7f;
                                // fallthrough
                            case 5:
                                data[length + 4] = (packet->words[index2 + 1] >> 8) & 0x7f;
                                // fallthrough
                            case 4:
                                data[length + 3] = (packet->words[index2 + 1] >> 16) & 0x7f;
                                // fallthrough
                            case 3:
                                data[length + 2] = (packet->words[index2 + 1] >> 24) & 0x7f;
                                // fallthrough
                            case 2:
                                data[length + 1] = packet->words[index2] & 0x7f;
                                // fallthrough
                            case 1:
                                data[length] = (packet->words[index2] >> 8) & 0x7f;
                                length += (packet->words[index2] >> 16) & 0x0f;
                                // fallthrough
                            case 0:
                            default:
                                break;
                        }

                        if (((packet->words[index2] >> 20) & 0x0f) == kMIDISysExStatusComplete || ((packet->words[index2] >> 20) & 0x0f) == kMIDISysExStatusEnd)
                        {
                            data[length] = 0xf7;
                            length++;
                        }

                        *running_status = 0;
                        write_event(data, length);

                        index2++;

#ifdef PRINT_EVENTS
                        printf("SysEx (fragment), group:%d length:%d\n", 0, length);
#endif
                    }
                    else
                    {
#ifdef PRINT_EVENTS
                        printf("SysEx (fragment), group:%d length:%d\n", (packet->words[index2] >> 24) & 0x0f, (packet->words[index2] >> 16) & 0x0f);
#endif
                        index2++;
                    }

                    break;

                // other types - 1 word
                case kMIDIMessageTypeUtility:
                case 6:
                case 7:
                default:
                    fprintf(stderr, "Unhandled message type: %d\n", (packet->words[index2] >> 28) & 0x0f);
                    break;

                // other types - 2 words
                case kMIDIMessageTypeChannelVoice2:
                case 8:
                case 9:
                case 10:
                    fprintf(stderr, "Unhandled message type: %d\n", (packet->words[index2] >> 28) & 0x0f);
                    index2++;
                    break;

                // other types - 3 words
                case 11:
                case 12:
                    fprintf(stderr, "Unhandled message type: %d\n", (packet->words[index2] >> 28) & 0x0f);
                    index2 += 2;
                    break;

                // other types - 4 words
                case kMIDIMessageTypeData128:
                case 13:
                case 14:
                case 15:
                    fprintf(stderr, "Unhandled message type: %d\n", (packet->words[index2] >> 28) & 0x0f);
                    index2 += 3;
                    break;
            }
        }
    }
}
#endif


static void usage(const char *progname)
{
    static const char basename[] = "cs_coredrv";

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


static int run_as_daemon_start(char *argv[]) __attribute__((noinline));
static int run_as_daemon_start(char *argv[])
{
    pid_t pid;
    int res, err;
    struct sigaction chld_action;
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];

    // get current signal action for SIGCHLD
    sigaction(SIGCHLD, NULL, &chld_action);

    pid = getpid();
    if (chld_action.sa_handler != SIG_IGN || getpgrp() == pid)
    {
        // spawn a new process (current process is process group leader or signal handler for SIGCHLD is not SIG_IGN)

        signal(SIGCHLD, SIG_IGN);

        res = proc_pidpath(pid, pathbuf, sizeof(pathbuf));
        err = posix_spawn(NULL, (res > 0 && res < sizeof(pathbuf)) ? pathbuf : argv[0], NULL, NULL, argv, *_NSGetEnviron());
        if (err != 0)
        {
            fprintf(stderr, "Error spawning process: %i\n", err);
            return -1;
        }

        exit(0);
    }
    else
    {
        // create new session (current process is not process group leader and signal handler for SIGCHLD is SIG_IGN)

        if (setsid() < 0)
        {
            fprintf(stderr, "Error creating session\n");
            return -2;
        }
    }

    printf("Running as daemon...\n");

    return 0;
}

static void run_as_daemon_finish(void) __attribute__((noinline));
static void run_as_daemon_finish(void)
{
    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);
}

static void stop_synth(void) __attribute__((noinline));
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
        return 1;
    }
#endif

#if !defined(x86_malloc)
    if (0 != x86_init_malloc())
    {
        fprintf(stderr, "Error initializing memory allocator\n");
        return 1;
    }
#endif

    event_buffer = (uint8_t *)x86_malloc(65536);
    if (event_buffer == NULL)
    {
        stop_synth();
        fprintf(stderr, "Error allocating memory buffers\n");
        return -3;
    }

    base_time = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);

    // initialize D77
    if (!CS_SynthesizerActivate(NULL, CS_OUTPUT_GenerateAudio, 0, 0, NULL, NULL))
    {
        stop_synth();
        fprintf(stderr, "error initializing synth\n");
        return -4;
    }

    if (samples_per_call == 0 || bytes_per_call == 0)
    {
        stop_synth();
        fprintf(stderr, "synth not initialized correctly (datafile error?)\n");
        return -5;
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


    return 0;
}

static int drop_privileges(void)
{
    uid_t uid;
    gid_t gid;
    const char *sudo_id;
    long long int llid;
    struct passwd *passwdbuf;

    if (getuid() != 0)
    {
        return 0;
    }

    if (issetugid())
    {
        return -1;
    }

    sudo_id = getenv("SUDO_UID");
    if (sudo_id == NULL)
    {
        return -2;
    }

    errno = 0;
    llid = strtoll(sudo_id, NULL, 10);
    uid = (uid_t) llid;
    if (errno != 0 || uid == 0 || llid != (long long int)uid)
    {
        return -3;
    }

    gid = getgid();
    if (gid == 0)
    {
        sudo_id = getenv("SUDO_GID");
        if (sudo_id == NULL)
        {
            passwdbuf = getpwuid(uid);
            if (passwdbuf != NULL)
            {
                gid = passwdbuf->pw_gid;
            }

            if (gid == 0)
            {
                return -4;
            }
        }
        else
        {
            errno = 0;
            llid = strtoll(sudo_id, NULL, 10);
            gid = (gid_t) llid;
            if (errno != 0 || gid == 0 || llid != (long long int)gid)
            {
                return -5;
            }
        }
    }

    if (setgid(gid) != 0)
    {
        return -6;
    }
    if (setuid(uid) != 0)
    {
        return -7;
    }

    printf("Dropped root privileges\n");

    chdir("/");

    return 0;
}

static void handle_privileges(void) __attribute__((noinline));
static void handle_privileges(void)
{
    // try to increase priority (only root)
    setpriority(PRIO_PROCESS, 0, -20);

    if (drop_privileges() < 0)
    {
        fprintf(stderr, "Error dropping root privileges\n");
    }
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

static void audio_callback_proc(void *inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer)
{
    render_audiodata();
    midi_queue_buffer[subbuf_counter]->mAudioDataByteSize = bytes_per_call;
    memcpy(midi_queue_buffer[subbuf_counter]->mAudioData, audio_data, bytes_per_call);
    AudioQueueEnqueueBuffer(midi_pcm_queue, midi_queue_buffer[subbuf_counter], 0, NULL);
    subbuf_counter++;
    if (subbuf_counter == num_subbuffers)
    {
        subbuf_counter = 0;
    }
}

static int open_pcm_output(void) __attribute__((noinline));
static int open_pcm_output(void)
{
    AudioStreamBasicDescription format;
    OSStatus err;

    format.mSampleRate = frequency;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
    format.mBytesPerPacket = 2 * num_channels;
    format.mFramesPerPacket = 1;
    format.mBytesPerFrame = 2 * num_channels;
    format.mChannelsPerFrame = num_channels;
    format.mBitsPerChannel = 16;
    format.mReserved = 0;

    err = AudioQueueNewOutput(&format, audio_callback_proc, NULL, NULL, NULL, 0, &midi_pcm_queue);
    if (err != noErr)
    {
        fprintf(stderr, "Error creating PCM queue: %i\n", err);
        return -1;
    }

    midi_queue_buffer = (struct AudioQueueBuffer **)malloc(num_subbuffers * sizeof(void *));
    if (midi_queue_buffer == NULL)
    {
        fprintf(stderr, "Error allocating queue buffer\n");
        return -2;
    }

    for (int i = 0; i < num_subbuffers; i++)
    {
        err = AudioQueueAllocateBuffer(midi_pcm_queue, bytes_per_call, &midi_queue_buffer[i]);
        if (err != noErr)
        {
            fprintf(stderr, "Error allocating queue buffer: %i\n", err);
            return -2;
        }

        midi_queue_buffer[i]->mUserData = (void *)(uintptr_t)i;
    }

    return 0;
}

static void close_pcm_output(void) __attribute__((noinline));
static void close_pcm_output(void)
{
    AudioQueueDispose(midi_pcm_queue, 1);
}


static int open_midi_endpoint(void) __attribute__((noinline));
static int open_midi_endpoint(void)
{
    OSStatus err;

    err = MIDIClientCreate(CFSTR(MIDI_NAME), NULL, NULL, &midi_client);
    if (err != noErr)
    {
        fprintf(stderr, "Error creating MIDI client: %i\n", err);
        return -1;
    }

    midi_running_status = 0;

#if (MIDI_API >= 0)
#if (MIDI_API == 0)
    if (__builtin_available(macOS 11.0, *))
#endif
    {
        err = MIDIDestinationCreateWithProtocol(midi_client, CFSTR(MIDI_NAME), kMIDIProtocol_1_0, &midi_endpoint, ^(const MIDIEventList *evtlist, void * __nullable srcConnRefCon) { midi_receive_proc(evtlist, &midi_running_status, srcConnRefCon); } );
    }
#endif
#if (MIDI_API <= 0)
#if (MIDI_API == 0)
    else
#endif
    {
        err = MIDIDestinationCreate(midi_client, CFSTR(MIDI_NAME), midi_read_proc, &midi_running_status, &midi_endpoint);
    }
#endif
    if (err != noErr)
    {
        MIDIClientDispose(midi_client);
        fprintf(stderr, "Error creating MIDI destination: %i\n", err);
        return -2;
    }

    printf("MIDI destination is %s\n", MIDI_NAME);

    if (MIDIObjectSetIntegerProperty(midi_endpoint, kMIDIPropertyUniqueID, midi_name_crc32) == noErr)
    {
        printf("Unique ID is %i\n", (int)(int32_t)midi_name_crc32);
    }
    else
    {
        int32_t unique_id;

        for (int i = 0; i < 32; i++)
        {
            unique_id = midi_name_crc32 ^ (1 << i);
            err = MIDIObjectSetIntegerProperty(midi_endpoint, kMIDIPropertyUniqueID, unique_id);
            if (err == noErr)
            {
                printf("Unique ID is %i\n", (int)unique_id);
                break;
            }
        }

        if (err != noErr)
        {
            if (MIDIObjectGetIntegerProperty(midi_endpoint, kMIDIPropertyUniqueID, &unique_id) == noErr)
            {
                printf("Unique ID is %i\n", (int)unique_id);
            }
        }
    }

    return 0;
}

static void close_midi_endpoint(void) __attribute__((noinline));
static void close_midi_endpoint(void)
{
    MIDIEndpointDispose(midi_endpoint);
    MIDIClientDispose(midi_client);
}


static void main_loop(void) __attribute__((noinline));
static void main_loop(void)
{
    int is_paused;
    uint64_t last_written_time, current_time;

    for (int i = 2; i < num_subbuffers; i++)
    {
        midi_queue_buffer[i]->mAudioDataByteSize = bytes_per_call;
        memset(midi_queue_buffer[i]->mAudioData, 0, bytes_per_call);
        AudioQueueEnqueueBuffer(midi_pcm_queue, midi_queue_buffer[i], 0, NULL);
    }

    // pause pcm playback at the beginning
    is_paused = 1;

    midi_event_written = 0;

    for (;;)
    {
        if (is_paused)
        {
            struct timespec req;

            req.tv_sec = 0;
            req.tv_nsec = 10000000;
            nanosleep(&req, NULL);

            if (midi_event_written)
            {
                midi_event_written = 0;

                // remember time of last written event
                last_written_time = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);

                if (AudioQueueStart(midi_pcm_queue, NULL) == noErr)
                {
                    is_paused = 0;
                    printf("PCM playback unpaused\n");
                }
            }
            else
            {
                continue;
            }
        }

        CFRunLoopRunInMode(kCFRunLoopDefaultMode, 1, 0);

        current_time = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);
        if (midi_event_written)
        {
            midi_event_written = 0;

            // remember time of last written event
            last_written_time = current_time;

            continue;
        }

        // if more than 60 seconds elapsed from last written event, then pause pcm playback
        if (current_time - last_written_time > 60000000000ull)
        {
            if (AudioQueuePause(midi_pcm_queue) == noErr)
            {
                is_paused = 1;
                printf("PCM playback paused\n");
            }
            else
            {
                // if pausing doesn't work then set time of last written event as current time, so the next attempt to pause will be in 60 seconds
                last_written_time = current_time;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (sizeof(PTR32(void)) != 4)
    {
        fprintf(stderr, "Error: The program wasn't compiled correctly for %i-bits\n", (int) (8 * sizeof(void*)));
        return 0;
    }

    read_arguments(argc, argv);

    if (daemonize)
    {
        if (run_as_daemon_start(argv) < 0)
        {
            return 1;
        }
    }

    if (start_synth() < 0)
    {
        return 2;
    }

    handle_privileges();

    if (open_pcm_output() < 0)
    {
        stop_synth();
        return 5;
    }

    if (open_midi_endpoint() < 0)
    {
        close_pcm_output();
        stop_synth();
        return 6;
    }

    if (daemonize)
    {
        run_as_daemon_finish();
    }

    main_loop();

    close_midi_endpoint();
    close_pcm_output();
    stop_synth();
    return 0;
}

