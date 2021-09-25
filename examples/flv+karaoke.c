/*
 * flv+karaoke.c
 * Copyright 2021 Michael Farrell <micolous+git@gmail.com>
 * Copyright 2016-2017 Twitch Interactive, Inc. or its affiliates.
 * 
 * SPDX-License-Identifier: MIT
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This takes in a text file with timestamps of single words, and muxes as
 * a CEA-608 caption in a given FLV file.
 * 
 * The text file looks like this:
 * 
 * 0: This
 * 449: PEP
 * 749: describes
 * 1449: changes
 * 
 * Each line starts with a timestamp as an integer number of milliseconds,
 * followed by a colon and space, then the next word.
 * 
 * There is no support for comments, and the file is terminated by a new
 * line.
 */

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

vtt_t* karoke_parse(const utf8_char_t* data, size_t size) {
    vtt_t* vtt = NULL;
    size_t line_length = 0, trimmed_length = 0;

    if (!data || !size || size < 6) {
        return NULL;
    }

    vtt = vtt_new();

    for (;;) {
        line_length = 0;

        do {
            data += line_length;
            size -= line_length;
            line_length = utf8_line_length(data); // Line length
            trimmed_length = utf8_trimmed_length(data, line_length);
            // Skip empty lines
        } while (0 < line_length && 0 == trimmed_length);

        // line length only zero at EOF
        if (0 == line_length) {
            break;
        }

        uint64_t ts = 0;
        char cue_str[1024];

        memset(cue_str, 0, sizeof(cue_str));

        int matches = sscanf(data, "%" PRIu64 ": %1023[^\n\r]", &ts, cue_str);
        if (matches != 2) {
            fprintf(stderr, "error reading line: %s\n", data);
            exit(1);
        }

        data += line_length;
        size -= line_length;

        size_t text_size = strnlen(cue_str, sizeof(cue_str) - 1);
        // Add a space after the cue.
        cue_str[text_size] = ' ';
        text_size++;

        // should we trim here?

        vtt_block_t* block = vtt_block_new(vtt, cue_str, text_size, VTT_CUE);
        block->timestamp = ((double)ts) / 1000;
    }

    return vtt;
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
            srt_t* srt = karoke_parse(&g_srt_data[0], g_srt_size);
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

int should_do_things(timecode_ring_t* ring, double new_ts) {
    double oldest = timecode_ring_oldest(ring);
    double newest = timecode_ring_newest(ring);
    double rate = timecode_ring_rate(ring);

    if (oldest < 0 || newest < 0) {
        // Nothing in the ring, do something!
        goto something;
    }

    if (oldest > new_ts) {
        // Older than the oldest thing we have, do nothing
        goto nothing;
    }

    if (rate < 0) {
        // No rate signal
        if (new_ts > newest) {
            goto something;
        } else {
            goto nothing;
        }
    }

    // There's a rate signal, see if we're < 110% of that.
    if (newest < new_ts) {
        if (newest + (rate * 1.1) > new_ts) {
            goto something;
        } else {
            // we're more than 110% of newest at normal rate.
            // Probably skewing.
            goto nothing;
        }
    }

    // We're somewhere between oldest and newest. Is that window a good size for us?
    if (newest - rate < new_ts) {
        // Lets try waiting for a better window
        goto nothing;
    }

    // Do something I guess?
something:
    timecode_ring_push(ring, new_ts);
    return 1;

nothing:
    timecode_ring_push(ring, new_ts);
    return 0;
}

int main(int argc, char** argv)
{
    flvtag_t tag;
    srt_t* old_srt = NULL;
    srt_cue_t* next_cue = NULL;
    cc_data_cmdlist_t next_cmdlist;
    uint16_t next_cmdlist_pos = 0;
    double timestamp, latest_time = 0, offset = 0;
    int has_audio, has_video;
    uint8_t did_something = 0;
    timecode_ring_t ring;
    uint8_t column = 0;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input_flv> <input_karaoke> <output_flv>\n", argv[0]);
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
    timecode_ring_init(&ring);

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
            next_cue = cur_srt->cue_head;
            next_cmdlist_pos = 0;
            cmdlist_from_streaming_karaoke(&next_cmdlist, srt_cue_data(next_cue), &column);
        }

        if (flvtag_avcpackettype_nalu == flvtag_avcpackettype(&tag) && flvtag_type_video == flvtag_type(&tag)) {
            did_something = 0;

            // We really don't want time going backward on us.
            if (should_do_things(&ring, timestamp) && timestamp > latest_time) {
                latest_time = timestamp;

                // fprintf(stderr, "T: %0.02f, frame type = %d\n", timestamp, flvtag_frametype(&tag));
                /* if (clear_timestamp >= 0 && timestamp >= clear_timestamp) {
                    // TODO: clear this nicer
                    fprintf(stderr, "T: %0.02f: [CAPTIONS CLEARED]\n", timestamp);
                    uint16_t p = 0;
                    sei_for_remaining_commands(&tag, &clearlist, &p);
                    did_something = 1;
                    clear_timestamp = -1;
                } else */
                if (next_cue) {
                    // There is a pending cue, try to push that out.
                    if (timestamp >= (offset + next_cue->timestamp) && next_cmdlist_pos < next_cmdlist.length) {
                        // We should send some of that cue!
                        if (next_cmdlist_pos == 0) {
                            fprintf(stderr, "T: %0.02f: Cue %0.02f (%0.02fs):\n%s\n", timestamp, (offset + next_cue->timestamp), next_cue->duration, srt_cue_data(next_cue));
                        }

                        sei_for_n_commands(&tag, &next_cmdlist, &next_cmdlist_pos, 2);
                        did_something = 1;

                        if (next_cmdlist_pos >= next_cmdlist.length) {
                            // We need to reload!
                            next_cue = next_cue->next;
                            next_cmdlist_pos = 0;
                            if (next_cue) {
                                fprintf(stderr, "LINE!\n");
                                cmdlist_from_streaming_karaoke(&next_cmdlist, srt_cue_data(next_cue), &column);
                            }
                        }
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
