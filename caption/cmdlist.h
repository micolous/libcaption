#ifndef LIBCAPTION_CMDLIST_H
#define LIBCAPTION_CMDLIST_H
#ifdef __cplusplus
extern "C" {
#endif

#include "caption.h"
#include "cea708.h"

#include <stddef.h>

// TODO: determine if this is reasonable
#define MAX_CC_CMDLIST_SIZE SCREEN_ROWS * SCREEN_COLS * 2 + 5

typedef struct {
    uint16_t length;

    cc_data_t commands[MAX_CC_CMDLIST_SIZE];
} cc_data_cmdlist_t;

/**
 * Pushes a single cc_data block into a cmdlist.
 * @param cmdlist Command list to use.
 * @param int Whether the cc_data block is valid, per CEA-708.
 * @param cc_data 2-byte cc_data_1, cc_data_2.
 */
libcaption_stauts_t cmdlist_push(cc_data_cmdlist_t* cmdlist, int valid, cea708_cc_type_t type, uint16_t cc_data);

/*!
 * Adds a single command from a command list to a cea708 block.
 * @param cea708 Destination CEA-708 block
 * @param cmdlist Source command list
 * @param pos Index of the command within cmdlist to push. This is incremented on success. Must not be null.
 */
libcaption_stauts_t cea708_add_from_cmdlist(cea708_t* cea708, cc_data_cmdlist_t* cmdlist, uint16_t* pos);

/**
 * Adds as many commands as possible from a cmdlist to a cea708 block.
 * @param cea708 Destination CEA-708 block
 * @param cmdlist Source command list
 * @param pos Index of the command within cmdlist to push. This is incremented on success.
 *            If null, starts from position 0.
 */
libcaption_stauts_t cea708_add_all_from_cmdlist(cea708_t* cea708, cc_data_cmdlist_t* cmdlist, uint16_t* pos);

#ifdef __cplusplus
}
#endif
#endif // LIBCAPTION_CMDLIST_H
