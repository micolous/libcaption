
#include "cmdlist.h"

#include <string.h>

libcaption_stauts_t cmdlist_push(cc_data_cmdlist_t* cmdlist, int valid, cea708_cc_type_t type, uint16_t cc_data) {
    if (cmdlist == NULL || cmdlist->length >= MAX_CC_CMDLIST_SIZE) {
        return LIBCAPTION_ERROR;
    }

    uint16_t i = cmdlist->length;
    cmdlist->length++;

    cmdlist->commands[i].marker_bits = 0x1f;
    cmdlist->commands[i].cc_valid = valid;
    cmdlist->commands[i].cc_type = type;
    cmdlist->commands[i].cc_data = cc_data;

    return LIBCAPTION_OK;
}

libcaption_stauts_t cea708_add_from_cmdlist(cea708_t* cea708, cc_data_cmdlist_t* cmdlist, uint16_t* pos) {
    if (cea708 == NULL || cmdlist == NULL || pos == NULL) {
        return LIBCAPTION_ERROR;
    }

    if (cea708->user_data.cc_count >= 30) {
        // Out of space
        return LIBCAPTION_ERROR;
    }

    if (*pos >= cmdlist->length) {
        // Nothing to do
        return LIBCAPTION_ERROR;
    }

    memcpy(&cea708->user_data.cc_data[cea708->user_data.cc_count],
           &cmdlist->commands[*pos], sizeof(cc_data_t));
    
    cea708->user_data.cc_count++;
    (*pos)++;
    return LIBCAPTION_OK;
}

libcaption_stauts_t cea708_add_all_from_cmdlist(cea708_t* cea708, cc_data_cmdlist_t* cmdlist, uint16_t* pos) {
    libcaption_stauts_t e;
    uint16_t p = 0;

    if (cea708 == NULL || cmdlist == NULL) {
        return LIBCAPTION_ERROR;
    }

    if (pos == NULL) {
        pos = &p;
    }

    while (*pos < cmdlist->length) {
        e = cea708_add_from_cmdlist(cea708, cmdlist, pos);
        if (e != LIBCAPTION_OK) {
            return e;
        }
    }

    return LIBCAPTION_OK;
}
