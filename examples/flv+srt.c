/**********************************************************************************************/
/* The MIT License                                                                            */
/*                                                                                            */
/* Copyright 2016-2017 Twitch Interactive, Inc. or its affiliates. All Rights Reserved.       */
/*                                                                                            */
/* Permission is hereby granted, free of charge, to any person obtaining a copy               */
/* of this software and associated documentation files (the "Software"), to deal              */
/* in the Software without restriction, including without limitation the rights               */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell                  */
/* copies of the Software, and to permit persons to whom the Software is                      */
/* furnished to do so, subject to the following conditions:                                   */
/*                                                                                            */
/* The above copyright notice and this permission notice shall be included in                 */
/* all copies or substantial portions of the Software.                                        */
/*                                                                                            */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR                 */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,                   */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE                */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER                     */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,              */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN                  */
/* THE SOFTWARE.                                                                              */
/**********************************************************************************************/
#include "flv.h"
#include "mpeg.h"
#include "srt.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#include <WinSock2.h>
#else
#include <sys/select.h>
#include <unistd.h>
#endif

#define MAX_SRT_SIZE (1 * 1024 * 1024)

// retunes number of bytes read
// negative number on error
// retursn 0 on 'not ready' and 'eof'
// eof set to 1 on end, otherwise zero
size_t fd_read(int fd, uint8_t* data, size_t size, int* eof)
{
    fd_set rfds;
    struct timeval tv;
    size_t retval;

    (*eof) = 0;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 1;
    retval = select(fd + 1, &rfds, NULL, NULL, &tv);

    if (0 > retval) {
        return retval;
    }

    // not ready
    if (!(retval && FD_ISSET(fd, &rfds))) {
        return 0;
    }

#ifdef _WIN32
    retval = _read(fd, data, (unsigned int)(size));
#else
    retval = read(fd, data, size);
#endif

    if (0 == retval) {
        (*eof) = 1;
    }

    return retval;
}

size_t g_srt_size = 0;
utf8_char_t g_srt_data[MAX_SRT_SIZE];
srt_t* srt_from_fd(int fd)
{
    int eof;
    uint8_t c;

    for (;;) {
        size_t ret = fd_read(fd, &c, 1, &eof);

        if (eof || (1 == ret && 0 == c)) {
            srt_t* srt = srt_parse(&g_srt_data[0], g_srt_size);
            g_srt_size = 0;
            return srt;
        }

        if (1 == ret) {
            if (g_srt_size >= MAX_SRT_SIZE - 1) {
                fprintf(stderr, "Warning MAX_SRT_SIZE reached. Clearing buffer\n");
                g_srt_size = 0;
            }

            g_srt_data[g_srt_size] = c;
            g_srt_size += 1;
        } else {
            return 0;
        }
    }
}

int main(int argc, char** argv)
{
    flvtag_t tag;
    srt_t* old_srt = NULL;
    srt_cue_t* next_cue = NULL;
    cc_data_cmdlist_t next_cmdlist, clearlist;
    uint16_t next_cmdlist_pos = 0;
    double timestamp, latest_time = 0, offset = 0, clear_timestamp = 0;
    int has_audio, has_video;
    uint8_t did_something = 0;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input_flv> <input_srt> <output_flv>\n", argv[0]);
        return 1;
    }

    FILE* flv = flv_open_read(argv[1]);
#ifdef _WIN32
    int fd = _open(argv[2], O_RDWR);
#else
    int fd = open(argv[2], O_RDWR);
#endif
    FILE* out = flv_open_write(argv[3]);

    flvtag_init(&tag);
    memset(&next_cmdlist, 0, sizeof(cc_data_cmdlist_t));
    cmdlist_for_text(&clearlist, NULL);

    if (!flv_read_header(flv, &has_audio, &has_video)) {
        fprintf(stderr, "%s is not an flv file\n", argv[1]);
        return EXIT_FAILURE;
    }

    flv_write_header(out, has_audio, has_video);

    fprintf(stderr, "Reading flv from %s\n", argv[1]);
    fprintf(stderr, "Reading captons from %s\n", argv[2]);
    fprintf(stderr, "Writing flv to %s\n", argv[3]);

    while (flv_read_tag(flv, &tag)) {

        srt_t* cur_srt = srt_from_fd(fd);
        timestamp = flvtag_pts_seconds(&tag);

        if (cur_srt) {
            fprintf(stderr, "Loaded new SRT at time %f\n", timestamp);
            if (old_srt != NULL) {
                srt_free(old_srt);
            }
            old_srt = cur_srt;
            offset = timestamp;
            clear_timestamp = timestamp;
            next_cue = cur_srt->cue_head;
            next_cmdlist_pos = 0;
            cmdlist_for_text(&next_cmdlist, srt_cue_data(next_cue));
        }

        if (flvtag_avcpackettype_nalu == flvtag_avcpackettype(&tag) && flvtag_type_video == flvtag_type(&tag)) {
            did_something = 0;

            // We really don't want time going backward on us.
            if (timestamp > latest_time) {
                latest_time = timestamp;

                // fprintf(stderr, "T: %0.02f, frame type = %d\n", timestamp, flvtag_frametype(&tag));
                if (clear_timestamp >= 0 && timestamp >= clear_timestamp) {
                    // TODO: clear this nicer
                    fprintf(stderr, "T: %0.02f: [CAPTIONS CLEARED]\n", timestamp);
                    uint16_t p = 0;
                    sei_for_remaining_commands(&tag, &clearlist, &p);
                    did_something = 1;
                    clear_timestamp = -1;
                } else if (next_cue) {
                    // There is a pending cue, try to push that out.
                    if ((offset + next_cue->timestamp) <= timestamp) {
                        // We're at or past our cue time.
                        // fprintf(stderr, "Buffer state: %" PRIu16 " of %" PRIu16 "\n", next_cmdlist_pos, next_cmdlist.length);
                        fprintf(stderr, "T: %0.02f: Cue %0.02f (%0.02fs):\n%s\n", timestamp, (offset + next_cue->timestamp), next_cue->duration, srt_cue_data(next_cue));
                        clear_timestamp = (offset + next_cue->timestamp) + next_cue->duration;

                        // Push out whatever we have left, it's cue time!
                        sei_for_remaining_commands(&tag, &next_cmdlist, &next_cmdlist_pos);
                        did_something = 1;

                        // Set up for next cue
                        next_cue = next_cue->next;
                        next_cmdlist_pos = 0;
                        if (next_cue) {
                            cmdlist_for_text(&next_cmdlist, srt_cue_data(next_cue));
                        }
                    } else if (next_cmdlist_pos < next_cmdlist.length - 1) {
                        // We're not yet at the next cue time, buffer up the next thing.
                        // fprintf(stderr, "T: %0.02f: Buffering next caption for %0.02f: %" PRIu16 " of %" PRIu16 "\n", timestamp, (offset + next_cue->timestamp), next_cmdlist_pos, next_cmdlist.length);
                        // There is more than 1 command left to insert (last is EOC)
                        sei_for_one_command(&tag, &next_cmdlist, &next_cmdlist_pos);
                        did_something = 1;
                    }
                }
            }

            if (!did_something) {
                // Fill in blanks
                sei_for_remaining_commands(&tag, NULL, NULL);
                did_something = 1;
            }
        }

        flv_write_tag(out, &tag);
    }

    srt_free(old_srt);
    flvtag_free(&tag);
    flv_close(flv);
    flv_close(out);
    return EXIT_SUCCESS;
}
