#include "dtvcc.h"

#include <string.h>


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

    if (*pos >= dtvcc->packet_data_size) {
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

    if (*pos + service_block->block_size >= size) {
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

libcaption_stauts_t dtvcc_finish_service_blocks(dtvcc_packet_t* dtvcc) {
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

    return LIBCAPTION_OK;
}



/*
void dtvcc_packet_data(struct dtvcc_packet_t* dvtcc, uint8_t cc_data1, uint8_t cc_data2)
{
    if (dvtcc->service_number) {
        if (7 == dvtcc->service_number) {
            dvtcc->service_number
        }
    }
}*/
