#include "dtvcc.h"

#include "caption.h"
#include "cea708_charmap.h"
#include "cmdlist.h"
#include "utf8.h"

#include <endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

/**
 * Get the expected DTVCC packet size.
 *
 * @param dtvcc DTVCC packet to read from
 * @returns 0 on error, or packet_data length in bytes, up to a maximum of 127
 */
uint8_t dtvcc_packet_data_size(const dtvcc_packet_t* dtvcc) {
    if (!dtvcc) {
        return 0;
    } else if (dtvcc->packet_size_code == 0) {
        // CEA-708-E 5.1
        return DTVCC_MAX_PACKET_SIZE;
    } else {
        return ((dtvcc->packet_size_code * 2) - 1);
    }
}

uint8_t dtvcc_service_block_size(const dtvcc_service_block_t* service) {
    if (!service) {
        return 0;
    } else if (service->service_number == 0 || service->block_size == 0) {
        return 1;
    } else if (service->service_number == 0x1f) {
        return 2 + service->block_size;
    } else {
        return 1 + service->block_size;
    }
}

libcaption_stauts_t dtvcc_packet_start(dtvcc_packet_t* dtvcc, uint8_t cc_data1, uint8_t cc_data2) {
    if (!dtvcc) {
        return LIBCAPTION_ERROR;
    }

    dtvcc->packet_size_code = cc_data1 & 0x3f;
    dtvcc->sequence_number = (cc_data1 >> 6) & 0x03;
    dtvcc->packet_data[0] = cc_data2;

    dtvcc->packet_data_size = dtvcc_packet_data_size(dtvcc);

    return LIBCAPTION_OK;
}

libcaption_stauts_t dtvcc_packet_data(dtvcc_packet_t* dtvcc, uint8_t cc_data1, uint8_t cc_data2, uint8_t* pos) {
    if (!dtvcc || !pos) {
        return LIBCAPTION_ERROR;
    }

    if ((*pos & 0x1) != 0x1) {
        // We expect to always be at an odd offset, because
        // we start at position 1 after the starting header
        return LIBCAPTION_ERROR;
    }

    if (*pos + 1 >= dtvcc->packet_data_size) {
        // Current position is larger than the header indicated
        return LIBCAPTION_ERROR;
    }

    dtvcc->packet_data[*pos] = cc_data1;
    *pos += 1;
    dtvcc->packet_data[*pos] = cc_data2;
    *pos += 1;

    return LIBCAPTION_OK;
}

libcaption_stauts_t dtvcc_read_service_block(const dtvcc_packet_t* dtvcc, dtvcc_service_block_t* service_block, uint8_t* pos) {
    if (!service_block || !dtvcc || !pos) {
        return LIBCAPTION_ERROR;
    }

    uint8_t size = dtvcc_packet_data_size(dtvcc);
    if (*pos >= size) {
        // Current position is larger than the header indicated
        return LIBCAPTION_ERROR;
    }

    // Clear the entire service_block
    memset(service_block, 0, sizeof(dtvcc_service_block_t));

    // Read the first byte
    service_block->block_size = dtvcc->packet_data[*pos] & 0x1f;
    service_block->service_number = dtvcc->packet_data[*pos] >> 5;
    *pos += 1;
    
    if (service_block->service_number == 0 || service_block->block_size == 0) {
        // Null block (6.2.3)
        return LIBCAPTION_OK;
    }

    if (*pos >= size) {
        return LIBCAPTION_ERROR;
    }

    if (service_block->service_number == 0x7) {
        // Extended service number (6.2.2)
        service_block->extended_service_number = dtvcc->packet_data[*pos] & 0x3f;
        *pos += 1;
    }

    if (*pos + service_block->block_size > size) {
        return LIBCAPTION_ERROR;
    }

    // Block data (6.2.4)
    memcpy(service_block->block_data, &dtvcc->packet_data[*pos], service_block->block_size);
    *pos += service_block->block_size;

    return LIBCAPTION_OK;
}

libcaption_stauts_t dtvcc_write_service_block(dtvcc_packet_t* dtvcc, const dtvcc_service_block_t* service_block) {
    if (!service_block || !dtvcc) {
        return LIBCAPTION_ERROR;
    }

    if (service_block->block_size > DTVCC_MAX_SERVICE_BLOCK_SIZE) {
        // Too big!
        return LIBCAPTION_ERROR;
    }

    // Check we have room
    uint8_t service_size = dtvcc_service_block_size(service_block);
    if (service_size + dtvcc->packet_data_size > DTVCC_MAX_PACKET_SIZE) {
        return LIBCAPTION_ERROR;
    }

    dtvcc->packet_data[dtvcc->packet_data_size] = (
        (service_block->block_size & 0x1f) |
        ((service_block->service_number & 0x7) << 5));
    dtvcc->packet_data_size++;

    if (service_block->service_number == 0 || service_block->block_size == 0) {
        // Null block (6.2.3)
        return LIBCAPTION_OK;
    }

    if (service_block->service_number == 0x7) {
        // Extended service number (6.2.2)
        dtvcc->packet_data[dtvcc->packet_data_size] = service_block->extended_service_number & 0x3f;
        dtvcc->packet_data_size++;
    }

    // Block data (6.2.4)
    memcpy(&dtvcc->packet_data[dtvcc->packet_data_size], service_block->block_data, service_block->block_size);
    dtvcc->packet_data_size += service_block->block_size;

    return LIBCAPTION_OK;
}


static const dtvcc_service_block_t null_service_block = {0, 0};

libcaption_stauts_t dtvcc_finish_service_blocks(dtvcc_packet_t* dtvcc, uint8_t sequence_number) {
    libcaption_stauts_t err;
    if (!dtvcc) {
        return LIBCAPTION_ERROR;
    }

    if (dtvcc->packet_data_size > DTVCC_MAX_PACKET_SIZE) {
        return LIBCAPTION_ERROR;
    }
    
    if ((dtvcc->packet_data_size & 0x1) == 0) {
        // Pad with extra byte to make it an odd number of bytes
        err = dtvcc_write_service_block(dtvcc, &null_service_block);
        if (err != LIBCAPTION_OK) {
            return err;
        }
    }

    // Update the size
    if (dtvcc->packet_data_size == DTVCC_MAX_PACKET_SIZE) {
        dtvcc->packet_size_code = 0;
    } else {
        dtvcc->packet_size_code = (dtvcc->packet_data_size + 1) / 2;
    }

    dtvcc->sequence_number = sequence_number % 4;
    return LIBCAPTION_OK;
}

libcaption_stauts_t dtvcc_packet_to_cmdlist(const dtvcc_packet_t *dtvcc, cc_data_cmdlist_t *cmdlist)
{
    if (!dtvcc || !cmdlist || dtvcc->packet_data_size < 1 || (dtvcc->packet_data_size & 0x1) == 0)
    {
        return LIBCAPTION_ERROR;
    }
    libcaption_stauts_t err = LIBCAPTION_OK;

    memset(cmdlist, 0, sizeof(cc_data_cmdlist_t));

    // Start packet
    uint16_t p = ((dtvcc->sequence_number & 0x03) << 14) |
                 ((dtvcc->packet_size_code & 0x3f) << 8) |
                 (dtvcc->packet_data[0] & 0xff);
    err = cmdlist_push(cmdlist, 1, cc_type_dtvcc_packet_start, p);
    if (err != LIBCAPTION_OK) {
        return err;
    }

    // Iterate other bytes
    for (int i = 1; i < dtvcc->packet_data_size; i += 2) {
        p = htons(*(uint16_t*)(dtvcc->packet_data + i));
        err = cmdlist_push(cmdlist, 1, cc_type_dtvcc_packet_data, p);
        if (err != LIBCAPTION_OK) {
            return err;
        }
    }

    return LIBCAPTION_OK;
}

uint8_t dtvcc_push_command_args(dtvcc_service_block_t* service_block, cea708_control_t cmd, const void* args, uint8_t args_len) {
    if (!service_block || (!args && args_len > 0)) {
        fprintf(stderr, "dtvcc_push_command_args: invalid args\n");
        exit(1);
        return 0;
    }

    if (service_block->block_size + args_len + 1 > DTVCC_MAX_SERVICE_BLOCK_SIZE) {
        // service block is full
        fprintf(stderr, "dtvcc_push_command_args: service block full, need (\n");
        exit(1);
        return 0;
    }

    service_block->block_data[service_block->block_size] = (uint8_t)(cmd & 0xff);
    service_block->block_size++;

    if (args && args_len > 0) {
        memcpy(&service_block->block_data[service_block->block_size], args, args_len);
        service_block->block_size += args_len;
    }
    return 1 + args_len;
}

inline uint8_t dtvcc_push_command(dtvcc_service_block_t* service_block, cea708_control_t cmd) {
    return dtvcc_push_command_args(service_block, cmd, NULL, 0);
}

uint8_t dtvcc_define_window(dtvcc_service_block_t* service_block, uint8_t window_id, const cea708_define_window_t* def) {
    if (window_id > 7) {
        return 0;
    }
    // TODO: static assert
    if (sizeof(cea708_define_window_t) != 6) {
        fprintf(stderr, "sizeof(cea708_define_window_t) == %zd, expected 6\n", sizeof(cea708_define_window_t));
        exit(1);
        return 0;
    }

    return dtvcc_push_command_args(service_block, cea708_control_define_window_0 + window_id, def, 6);
}

uint8_t dtvcc_delete_windows(dtvcc_service_block_t* service_block, uint8_t window_mask) {
    return dtvcc_push_command_args(service_block, cea708_control_delete_windows, &window_mask, 1);
}

inline uint8_t dtvcc_delete_all_windows(dtvcc_service_block_t* service_block) {
    return dtvcc_delete_windows(service_block, 0xff);
}

uint8_t dtvcc_set_pen_location(dtvcc_service_block_t* service_block, uint8_t row, uint8_t column) {
    // Clamp values
    cea708_set_pen_location_t spl = {
        .row = row & 0x0f,
        .column = column & 0x3f,
    };

    // TODO: static assert
    if (sizeof(cea708_set_pen_location_t) != 2) {
        fprintf(stderr, "sizeof(cea708_set_pen_location_t) == %zd, expected 2\n", sizeof(cea708_set_pen_location_t));
        exit(1);
        return 0;
    }

    return dtvcc_push_command_args(service_block, cea708_control_set_pen_location, &spl, 2);
}

const cea708_define_window_t karaoke_win = {
    .priority = 0,
    .column_lock = 1,
    .row_lock = 1,
    .visible = 1,
    .anchor_vertical = 99,
    .relative_positioning = 1,
    .anchor_horizontal = 49,
    .row_count = 2,
    .anchor_point = cea708_anchor_point_bottom,
    .column_count = SCREEN_COLS,
    .predefined_pen_style = cea708_predefined_pen_style_proportional_sans,
    .predefined_window_style = cea708_predefined_window_style_608_roll_up,
};

libcaption_stauts_t dtvcc_from_streaming_karaoke(dtvcc_service_block_t* service_block, const utf8_char_t* data, uint8_t* column) {
    if (service_block == NULL || data == NULL || column == NULL) {
        return LIBCAPTION_ERROR;
    }

    ssize_t size = (ssize_t)strlen(data);
    memset(service_block, 0, sizeof(dtvcc_service_block_t));

    // TODO: implement other service numbers
    service_block->service_number = 1;

    if (*column == 0) {
        // 8.11.1 Proper Order of Data, Simple Roll-up Style Captions
        dtvcc_delete_all_windows(service_block);
        dtvcc_define_window(service_block, 0, &karaoke_win);
        dtvcc_set_pen_location(service_block, 0, 0);
        dtvcc_push_command(service_block, cea708_control_carriage_return);
    }

    dtvcc_push_command(service_block, cea708_control_set_current_window_0);

    // fprintf(stderr, "column: %" PRIu8 "\n", *column);
    while ((*data) && size) {
        // skip whitespace at start of line
        while (size && utf8_char_whitespace(data)) {
            size_t s = utf8_char_length(data);
            data += s, size -= s;
        }

        if (!(*data)) {
            // Null terminator
            break;
        }

        // get charcter count for wrap (or orest of line)
        utf8_size_t char_count = utf8_line_length(data);
        // fprintf(stderr, "char_count(1) = %ld\n", char_count);
        if (*column + char_count >= SCREEN_COLS) {
            // need to make a new line
            *column = 0;
            dtvcc_delete_windows(service_block, 0xfe);
            dtvcc_define_window(service_block, 0, &karaoke_win);
            dtvcc_set_pen_location(service_block, 0, 0);
            dtvcc_push_command(service_block, cea708_control_carriage_return);
        }

        char_count = utf8_wrap_length(data, SCREEN_COLS - *column);
        // fprintf(stderr, "char_count(2) = %ld\n", char_count);

        // write to caption frame
        for (size_t c = 0; c < char_count; ++c) {
            size_t char_length = utf8_char_length(data);

            uint16_t cc_data = cea708_from_utf8_1(data);
            if (cc_data > 0 && cc_data <= 0xff) {
                dtvcc_push_command(service_block, (uint8_t)(cc_data & 0xff));
            } else if (cc_data > 0xff) {
                uint8_t high_byte = (cc_data >> 8) & 0xff;

                // Avoids a copy
#if __BYTE_ORDER == __LITTLE_ENDIAN
                const uint8_t* low_byte = (const uint8_t*)&cc_data;
#else
                const uint8_t* low_byte = (const uint8_t*)&cc_data + 1;
#endif
                dtvcc_push_command_args(service_block, high_byte, low_byte, 1);
            }

            data += char_length, size -= char_length;
            *column += 1;
            if (!size) {
                break;
            }
        }
    }

    return LIBCAPTION_OK;
}
