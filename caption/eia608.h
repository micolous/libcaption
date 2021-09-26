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
#ifndef LIBCAPTION_EIA608_H
#define LIBCAPTION_EIA608_H
#ifdef __cplusplus
extern "C" {
#endif

#include "caption_frame.h"
#include "cea708_types.h"
#include "cmdlist.h"
#include "eia608_charmap.h"
#include "utf8.h"

////////////////////////////////////////////////////////////////////////////////
// Parity
#define EIA608_BX(B, X) (((B) << (X)) & 0x80)
#define EIA608_BP(B) ((B)&0x7F) | (0x80 ^ EIA608_BX((B), 1) ^ EIA608_BX((B), 2) ^ EIA608_BX((B), 3) ^ EIA608_BX((B), 4) ^ EIA608_BX((B), 5) ^ EIA608_BX((B), 6) ^ EIA608_BX((B), 7))
#define EIA608_B2(B) EIA608_BP((B) + 0), EIA608_BP((B) + 1), EIA608_BP((B) + 2), EIA608_BP((B) + 3), EIA608_BP((B) + 4), EIA608_BP((B) + 5), EIA608_BP((B) + 6), EIA608_BP((B) + 7)
#define EIA608_B1(B) EIA608_B2((B) + 0), EIA608_B2((B) + 8), EIA608_B2((B) + 16), EIA608_B2((B) + 24), EIA608_B2((B) + 32), EIA608_B2((B) + 40), EIA608_B2((B) + 48), EIA608_B2((B) + 56)

#define DEFAULT_CHANNEL 0

static const uint8_t eia608_parity_table[] = { EIA608_B1(0), EIA608_B1(64) };
extern const char* eia608_style_map[];

#ifdef _MSC_VER
#ifndef inline
#define inline __inline
#endif
#endif

/*! \brief
    \param
*/
static inline uint8_t eia608_parity_byte(uint8_t cc_data) { return eia608_parity_table[0x7F & cc_data]; }
/*! \brief
    \param
*/
static inline uint16_t eia608_parity_word(uint16_t cc_data) { return (uint16_t)((eia608_parity_byte((uint8_t)(cc_data >> 8)) << 8) | eia608_parity_byte((uint8_t)cc_data)); }
/*! \brief
    \param
*/
static inline uint16_t eia608_parity(uint16_t cc_data) { return eia608_parity_word(cc_data); }
/*! \brief
    \param
*/
static inline int eia608_parity_varify(uint16_t cc_data) { return eia608_parity_word(cc_data) == cc_data ? 1 : 0; }
/*! \brief
    \param
*/
static inline int eia608_parity_strip(uint16_t cc_data) { return cc_data & 0x7F7F; }
/*! \brief
    \param
*/
static inline int eia608_test_second_channel_bit(uint16_t cc_data) { return (cc_data & 0x0800); }

////////////////////////////////////////////////////////////////////////////////
// cc_data types
/*! \brief
    \param
*/
static inline int eia608_is_basicna(uint16_t cc_data) { return 0x0000 != (0x6000 & cc_data); /*&& 0x1F00 < (0x7F00 & cc_data);*/ }
/*! \brief
    \param
*/
static inline int eia608_is_preamble(uint16_t cc_data) { return 0x1040 == (0x7040 & cc_data); }
/*! \brief
    \param
*/
static inline int eia608_is_midrowchange(uint16_t cc_data) { return 0x1120 == (0x7770 & cc_data); }
/*! \brief
    \param
*/
static inline int eia608_is_specialna(uint16_t cc_data) { return 0x1130 == (0x7770 & cc_data); }
/*! \brief
    \param
*/
static inline int eia608_is_xds(uint16_t cc_data) { return 0x0000 == (0x7070 & cc_data) && 0x0000 != (0x0F0F & cc_data); }
/*! \brief
    \param
*/
static inline int eia608_is_westeu(uint16_t cc_data) { return 0x1220 == (0x7660 & cc_data); }
/*! \brief
    \param
*/
static inline int eia608_is_control(uint16_t cc_data) { return 0x1420 == (0x7670 & cc_data) || 0x1720 == (0x7770 & cc_data); }
/*! \brief
    \param
*/
static inline int eia608_is_norpak(uint16_t cc_data) { return 0x1724 == (0x777C & cc_data) || 0x1728 == (0x777C & cc_data); }
/*! \brief
    \param
*/
static inline int eia608_is_padding(uint16_t cc_data) { return 0x8080 == cc_data; }

////////////////////////////////////////////////////////////////////////////////
// preamble

/*! \brief
    \param
*/
int eia608_parse_preamble(uint16_t cc_data, int* row, int* col, eia608_style_t* style, int* chan, int* underline);
/*! \brief
    \param
*/
int eia608_parse_midrowchange(uint16_t cc_data, int* chan, eia608_style_t* style, int* underline);
/*! \brief
    \param
*/
uint16_t eia608_row_column_pramble(int row, int col, int chan, int underline);
/*! \brief
    \param
*/
uint16_t eia608_row_style_pramble(int row, int chan, eia608_style_t style, int underline);
/*! \brief
    \param
*/
uint16_t eia608_midrow_change(int chan, eia608_style_t style, int underline);
////////////////////////////////////////////////////////////////////////////////

/*! \brief
    \param
*/
uint16_t eia608_control_command(eia608_control_t cmd, int cc);
/*! \brief
    \param
*/
static inline uint16_t eia608_tab(int size, int cc) { return eia608_control_command((eia608_control_t)(eia608_tab_offset_0 | (size & 0x0F)), cc); }
/*! \brief
    \param
*/
eia608_control_t eia608_parse_control(uint16_t cc_data, int* cc);
////////////////////////////////////////////////////////////////////////////////
// text
/*! \brief
    \param c
*/
uint16_t eia608_from_utf8_1(const utf8_char_t* c, int chan);
/*! \brief
    \param
*/
uint16_t eia608_from_utf8_2(const utf8_char_t* c1, const utf8_char_t* c2);
/*! \brief
    \param
*/
uint16_t eia608_from_basicna(uint16_t bna1, uint16_t bna2);
/*! \brief
    \param
*/
int eia608_to_utf8(uint16_t c, int* chan, utf8_char_t* char1, utf8_char_t* char2);
////////////////////////////////////////////////////////////////////////////////
/*! \brief
    \param
*/
void eia608_dump(uint16_t cc_data);

// Prepare a command list for later writing
void cmdlist_for_text(cc_data_cmdlist_t* cmdlist, const utf8_char_t* text);

libcaption_stauts_t commands_for_frame(cc_data_cmdlist_t* cmdlist, caption_frame_t* frame);
libcaption_stauts_t cmdlist_from_caption_clear(cc_data_cmdlist_t* cmdlist);
libcaption_stauts_t cmdlist_from_caption_fullreset(cc_data_cmdlist_t* cmdlist);
libcaption_stauts_t cmdlist_from_streaming_text(cc_data_cmdlist_t* cmdlist, const utf8_char_t* data);
libcaption_stauts_t cmdlist_from_streaming_karaoke(cc_data_cmdlist_t* cmdlist, const utf8_char_t* data, uint8_t* column);


////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif
#endif
