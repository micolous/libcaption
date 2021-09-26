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
#ifndef LIBCAPTION_CEA708_H
#define LIBCAPTION_CEA708_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "caption.h"
#include "caption_frame.h"
#include "cea708_types.h"

#define CEA608_MAX_SIZE (255)

////////////////////////////////////////////////////////////////////////////////

/*! \brief
    \param
*/
cc_data_t cea708_encode_cc_data(int cc_valid, cea708_cc_type_t type, uint16_t cc_data);
/*! \brief
    \param
*/
int cea708_cc_count(user_data_t* data);
/*! \brief
    \param
*/
uint16_t cea708_cc_data(user_data_t* data, int index, int* valid, cea708_cc_type_t* type);
////////////////////////////////////////////////////////////////////////////////

const static uint32_t GA94 = (('G' << 24) | ('A' << 16) | ('9' << 8) | '4');
const static uint32_t DTG1 = (('D' << 24) | ('T' << 16) | ('G' << 8) | '1');

/*! \brief
    \param
*/
int cea708_init(cea708_t* cea708, double timestamp); // will confgure using HLS compatiable defaults
/*! \brief
    \param
*/
libcaption_stauts_t cea708_parse_h264(const uint8_t* data, size_t size, cea708_t* cea708);
/*! \brief
    \param
*/
libcaption_stauts_t cea708_parse_h262(const uint8_t* data, size_t size, cea708_t* cea708);
/*! \brief
    \param
*/
libcaption_stauts_t cea708_to_caption_frame(caption_frame_t* frame, cea708_t* cea708);
/*! \brief
    \param
*/
int cea708_add_cc_data(cea708_t* cea708, int valid, cea708_cc_type_t type, uint16_t cc_data);
/*! \brief
    \param
*/
int cea708_render(cea708_t* cea708, uint8_t* data, size_t size);
/*! \brief
    \param
*/
void cea708_dump(cea708_t* cea708);


#ifdef __cplusplus
}
#endif
#endif
