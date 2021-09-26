#ifndef LIBCAPTION_DTVCC_H
#define LIBCAPTION_DTVCC_H
#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////////////////////////////////////////////

#include "caption.h"
#include "cea708_types.h"

#include <stdint.h>

#define DTVCC_MAX_PACKET_SIZE 127
#define DTVCC_MAX_SERVICE_BLOCK_SIZE 0x1f

/**
 * Initialises a DTVCC packet with the start bytes.
 *
 * @param dtvcc DTVCC packet to read in to
 * @param cc_data1 First CC data byte
 * @param cc_data2 Second CC data byte
 */
libcaption_stauts_t dtvcc_packet_start(dtvcc_packet_t* dtvcc, uint8_t cc_data1, uint8_t cc_data2);

/**
 * Initialises a DTVCC packet with the start bytes.
 *
 * @param dtvcc DTVCC packet to read in to
 * @param cc_data1 First CC data byte
 * @param cc_data2 Second CC data byte
 * @param pos Current byte offset of the packet, updated on success.
 */
libcaption_stauts_t dtvcc_packet_data(dtvcc_packet_t* dtvcc, uint8_t cc_data1, uint8_t cc_data2, uint8_t* pos);


libcaption_stauts_t dtvcc_read_service_block(const dtvcc_packet_t* dtvcc, dtvcc_service_block_t* service_block, uint8_t* pos);

/**
 * Write a service block to a DTVCC packet.
 */
libcaption_stauts_t dtvcc_write_service_block(dtvcc_packet_t* dtvcc, const dtvcc_service_block_t* service_block);

/**
 * Terminate a DTVCC packet, and update fields.
 */
libcaption_stauts_t dtvcc_finish_service_blocks(dtvcc_packet_t* dtvcc);

// static inline size_t dvtvcc_packet_size_bytes(const struct dtvcc_packet_t *dvtcc) { return dvtcc->packet_size*2-1;}

////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif
#endif // LIBCAPTION_DTVCC_H
