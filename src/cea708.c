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
#include "cea708.h"

#include <float.h>
#include <memory.h>
#include <stdio.h>

#include "eia608.h"
#include "dtvcc.h"

int cea708_cc_count(user_data_t* data)
{
    return data->cc_count;
}

uint16_t cea708_cc_data(user_data_t* data, int index, int* valid, cea708_cc_type_t* type)
{
    (*valid) = data->cc_data[index].cc_valid;
    (*type) = data->cc_data[index].cc_type;
    return data->cc_data[index].cc_data;
}

int cea708_init(cea708_t* cea708, double timestamp)
{
    memset(cea708, 0, sizeof(cea708_t));
    cea708->country = country_united_states;
    cea708->provider = t35_provider_atsc;
    cea708->user_identifier = GA94;
    cea708->user_data_type_code = 3;
    cea708->directv_user_data_length = 0;
    cea708->user_data.process_em_data_flag = 0;
    cea708->user_data.process_cc_data_flag = 1;
    cea708->user_data.additional_data_flag = 0;
    cea708->user_data.em_data = 0xFF;
    cea708->user_data.cc_count = 0;
    cea708->timestamp = timestamp;
    return 1;
}

void cea708_parse_user_data_type_strcture(const uint8_t* data, size_t size, user_data_t* user_data)
{
    memset(user_data, 0, sizeof(user_data_t));
    user_data->process_em_data_flag = !!(data[0] & 0x80);
    user_data->process_cc_data_flag = !!(data[0] & 0x40);
    user_data->additional_data_flag = !!(data[0] & 0x20);
    user_data->cc_count = (data[0] & 0x1F);
    user_data->em_data = data[1];
    data += 2, size -= 2;

    for (int i = 0; 3 < size && i < (int)user_data->cc_count; ++i, data += 3, size -= 3) {
        user_data->cc_data[i].marker_bits = data[0] >> 3;
        user_data->cc_data[i].cc_valid = data[0] >> 2;
        user_data->cc_data[i].cc_type = data[0] >> 0;
        user_data->cc_data[i].cc_data = data[1] << 8 | data[2];
    }
}

// 00 00 00  06 C1  FF FC 34 B9 FF : onCaptionInfo.
libcaption_stauts_t cea708_parse_h264(const uint8_t* data, size_t size, cea708_t* cea708)
{
    if (3 > size) {
        goto error;
    }

    // I think the first few bytes need to be handled in mpeg
    cea708->country = (itu_t_t35_country_code_t)(data[0]);
    cea708->provider = (itu_t_t35_provider_code_t)((data[1] << 8) | data[2]);
    cea708->user_identifier = 0;
    cea708->user_data_type_code = 0;
    data += 3, size -= 3;

    if (t35_provider_atsc == cea708->provider) {
        if (4 > size) {
            goto error;
        }

        cea708->user_identifier = ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
        data += 4, size -= 4;
    }

    // where country and provider are zero
    // Im not sure what this extra byte is. It sonly seesm to come up in onCaptionInfo
    // h264 spec seems to describe this
    if (0 == cea708->provider && 0 == cea708->country) {
        if (1 > size) {
            goto error;
        }

        data += 1, size -= 1;
    } else if (t35_provider_atsc == cea708->provider || t35_provider_direct_tv == cea708->provider) {
        if (1 > size) {
            goto error;
        }

        cea708->user_data_type_code = data[0];
        data += 1, size -= 1;
    }

    if (t35_provider_direct_tv == cea708->provider) {
        if (1 > size) {
            goto error;
        }

        cea708->directv_user_data_length = data[0];
        data += 1, size -= 1;
    }

    if (3 == cea708->user_data_type_code && 2 <= size) {
        cea708_parse_user_data_type_strcture(data, size, &cea708->user_data);
    } else if (4 == cea708->user_data_type_code) {
        // additional_CEA_608_data
    } else if (5 == cea708->user_data_type_code) {
        // luma_PAM_data
    } else {
        // ATSC_reserved_user_data
    }

    return LIBCAPTION_OK;
error:
    return LIBCAPTION_ERROR;
}

libcaption_stauts_t cea708_parse_h262(const uint8_t* data, size_t size, cea708_t* cea708)
{
    if (!data || 7 > size) {
        return LIBCAPTION_ERROR;
    }

    cea708->user_identifier = ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
    cea708->user_data_type_code = data[4];
    if (3 == cea708->user_data_type_code) {
        cea708_parse_user_data_type_strcture(data + 5, size - 5, &cea708->user_data);
    }

    return LIBCAPTION_OK;
}

int cea708_add_cc_data(cea708_t* cea708, int valid, cea708_cc_type_t type, uint16_t cc_data)
{
    if (30 <= cea708->user_data.cc_count) {
        return 0;
    }

    cea708->user_data.cc_data[cea708->user_data.cc_count].marker_bits = 0x1F;
    cea708->user_data.cc_data[cea708->user_data.cc_count].cc_valid = valid;
    cea708->user_data.cc_data[cea708->user_data.cc_count].cc_type = type;
    cea708->user_data.cc_data[cea708->user_data.cc_count].cc_data = cc_data;
    ++cea708->user_data.cc_count;
    return 1;
}

const cc_data_t filler_data = { 0x1F, 0, 0, 0 };

int cea708_render(cea708_t* cea708, uint8_t* data, size_t size)
{
    uint8_t i;
    size_t total = 0;
    data[0] = cea708->country;
    data[1] = cea708->provider >> 8;
    data[2] = cea708->provider >> 0;
    total += 3;
    data += 3;
    size -= 3;

    if (t35_provider_atsc == cea708->provider) {

        data[0] = cea708->user_identifier >> 24;
        data[1] = cea708->user_identifier >> 16;
        data[2] = cea708->user_identifier >> 8;
        data[3] = cea708->user_identifier >> 0;
        total += 4;
        data += 4;
        size -= 4;
    }

    if (t35_provider_atsc == cea708->provider || t35_provider_direct_tv == cea708->provider) {
        data[0] = cea708->user_data_type_code;
        total += 1;
        data += 1;
        size -= 1;
    }

    if (t35_provider_direct_tv == cea708->provider) {
        data[0] = cea708->directv_user_data_length;
        total += 1;
        data += 1;
        size -= 1;
    }

    // Make sure we have at least 5 blocks, and a multiple of
    // 5 blocks.
    uint8_t fake_count = (
        cea708->user_data.cc_count == 0 ? 5 :
        cea708->user_data.cc_count % 5 == 0 ? 0 :
        5 - (cea708->user_data.cc_count % 5)
    );

    data[1] = cea708->user_data.em_data;
    data[0] = (cea708->user_data.process_em_data_flag ? 0x80 : 0x00)
        | (cea708->user_data.process_cc_data_flag ? 0x40 : 0x00)
        | (cea708->user_data.additional_data_flag ? 0x20 : 0x00)
        | ((cea708->user_data.cc_count + fake_count) & 0x1F);

    total += 2;
    data += 2;
    size -= 2;

    for (i = 0; i < cea708->user_data.cc_count; ++i) {
        data[0] = (cea708->user_data.cc_data[i].marker_bits << 3) | (cea708->user_data.cc_data[i].cc_valid << 2) | cea708->user_data.cc_data[i].cc_type;
        data[1] = cea708->user_data.cc_data[i].cc_data >> 8;
        data[2] = cea708->user_data.cc_data[i].cc_data >> 0;
        total += 3;
        data += 3;
        size -= 3;
    }

    // Insert filler
    for (i = 0; i < fake_count; i++) {        
        data[0] = (filler_data.marker_bits << 3) | (filler_data.cc_valid << 2) | filler_data.cc_type;
        data[1] = filler_data.cc_data >> 8;
        data[2] = filler_data.cc_data >> 0;
        total += 3;
        data += 3;
        size -= 3;
    }

    data[0] = 0xFF; //marker bits
    return (int)(total + 1);
}

cc_data_t cea708_encode_cc_data(int cc_valid, cea708_cc_type_t type, uint16_t cc_data)
{
    cc_data_t data = { 0x1F, cc_valid, type, cc_data };
    return data;
}

void cea708_dump(cea708_t* cea708)
{
    int i;

    fprintf(stderr, "itu_t_t35_country_code_t %d\n", cea708->country);
    fprintf(stderr, "itu_t_t35_provider_code_t %d\n", cea708->provider);
    fprintf(stderr, "user_identifier %c%c%c%c\n",
        (cea708->user_identifier >> 24) & 0xFF, (cea708->user_identifier >> 16) & 0xFF,
        (cea708->user_identifier >> 8) & 0xFF, cea708->user_identifier & 0xFF);
    fprintf(stderr, "user_data_type_code %d\n", cea708->user_data_type_code);
    fprintf(stderr, "directv_user_data_length %d\n", cea708->directv_user_data_length);
    fprintf(stderr, "user_data.process_em_data_flag %d\n", cea708->user_data.process_em_data_flag);
    fprintf(stderr, "user_data.process_cc_data_flag %d\n", cea708->user_data.process_cc_data_flag);
    fprintf(stderr, "user_data.additional_data_flag %d\n", cea708->user_data.additional_data_flag);
    fprintf(stderr, "user_data.cc_count %d\n", cea708->user_data.cc_count);
    fprintf(stderr, "user_data.em_data %d\n", cea708->user_data.em_data);

    for (i = 0; i < (int)cea708->user_data.cc_count; ++i) {
        int valid;
        cea708_cc_type_t type;
        uint16_t cc_data = cea708_cc_data(&cea708->user_data, i, &valid, &type);

        if (valid && cc_type_ntsc_cc_field_1 == type) {
            eia608_dump(cc_data);
        } else {
            fprintf(stderr, "user_data.cc_data[%d] cc_valid: %s, cc_type: %d, cc_data: %04x\n", i, cea708->user_data.cc_data[i].cc_valid ? "true" : "false", cea708->user_data.cc_data[i].cc_type, cea708->user_data.cc_data[i].cc_data);
        }
    }
}

libcaption_stauts_t cea708_to_caption_frame(caption_frame_t* frame, cea708_t* cea708, dtvcc_packet_t* dtvcc, uint8_t* dtvcc_pos)
{
    int i, count = cea708_cc_count(&cea708->user_data);
    libcaption_stauts_t status = LIBCAPTION_OK;

    if (GA94 == cea708->user_identifier) {
        for (i = 0; i < count; ++i) {
            int valid;
            cea708_cc_type_t type;
            uint16_t cc_data = cea708_cc_data(&cea708->user_data, i, &valid, &type);

            if (!valid) {
                continue;
            }

            switch (type) {
                case cc_type_ntsc_cc_field_1:
                    status = libcaption_status_update(status, caption_frame_decode(frame, cc_data, cea708->timestamp));
                    break;

                case cc_type_dtvcc_packet_start:
                    if (*dtvcc_pos == 0) {
                        status = libcaption_status_update(
                            status, dtvcc_packet_start(dtvcc, cc_data >> 8, cc_data & 0xff));
                        *dtvcc_pos = 1;
                    } else {
                        fprintf(stderr, "unexpected packet start!\n");
                        exit(1);
                    }
                    break;

                case cc_type_dtvcc_packet_data:
                    if (*dtvcc_pos > 0) {
                        status = libcaption_status_update(
                            status, dtvcc_packet_data(dtvcc, cc_data >> 8, cc_data & 0xff, dtvcc_pos));
                    } else {
                        fprintf(stderr, "unexpected data!\n");
                        exit(1);
                    }
                    break;

                default:
                    // fprintf(stderr, "unhandled type (%u): data = %04x\n", type, cc_data);
                    break;
            }
        }
    }

    if (*dtvcc_pos > 0 && dtvcc->packet_data_size == *dtvcc_pos && status != LIBCAPTION_ERROR) {
        // End of DTVCC packet, time to parse it.
        uint8_t pos = 0;
        dtvcc_service_block_t service;
        fprintf(stderr, "dtvcc seq %d: %d bytes\n", dtvcc->sequence_number, dtvcc->packet_data_size);
        while (pos < dtvcc->packet_data_size) { 
            // Read each service.
            status = libcaption_status_update(status, dtvcc_read_service_block(dtvcc, &service, &pos));

            if (status == LIBCAPTION_ERROR) {
                break;
            }

            fprintf(stderr, "service %d: %d bytes...\n", service.service_number, service.block_size);
        }

        fprintf(stderr, "End of DTVCC packet!\n");
        *dtvcc_pos = 0;
        memset(dtvcc, 0, sizeof(dtvcc_packet_t));
    }

    // if (*dtvcc_pos > 0) {
    //     fprintf(stderr, "Got DTVCC packet: #%u, size = %u\n", dtvcc.sequence_number, dtvcc.packet_data_size);
    // }

    return status;
}
