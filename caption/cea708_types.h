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
#ifndef LIBCAPTION_CEA708_TYPES_H
#define LIBCAPTION_CEA708_TYPES_H
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    cc_type_ntsc_cc_field_1 = 0,
    cc_type_ntsc_cc_field_2 = 1,
    cc_type_dtvcc_packet_data = 2,
    cc_type_dtvcc_packet_start = 3,
} cea708_cc_type_t;

typedef struct {
    unsigned int marker_bits : 5;
    unsigned int cc_valid : 1;
    unsigned int cc_type : 2; // castable to cea708_cc_type_t
    unsigned int cc_data : 16;
} cc_data_t;

typedef struct {
    unsigned int process_em_data_flag : 1;
    unsigned int process_cc_data_flag : 1;
    unsigned int additional_data_flag : 1;
    unsigned int cc_count : 5;
    unsigned int em_data : 8;
    cc_data_t cc_data[32];
} user_data_t;

typedef enum {
    country_united_states = 181,
} itu_t_t35_country_code_t;

typedef enum {
    t35_provider_direct_tv = 47,
    t35_provider_atsc = 49,
} itu_t_t35_provider_code_t;

typedef struct {
    itu_t_t35_country_code_t country;
    itu_t_t35_provider_code_t provider;
    uint32_t user_identifier;
    uint8_t user_data_type_code;
    uint8_t directv_user_data_length;
    user_data_t user_data;
    double timestamp;
} cea708_t;

typedef struct {
    unsigned int sequence_number : 2;
    unsigned int packet_size_code : 6;

    // not part of packet, derrived from packet_size_code
    uint8_t packet_data_size;

    char packet_data[127];
} dtvcc_packet_t;

typedef struct {
    unsigned int service_number : 3;
    unsigned int block_size : 5;

    // Padding is always 0.
    // Only present if block_size != 0 && service_number == 7
    unsigned int _padding : 2;
    // Only present if block_size != 0 && service_number == 7
    unsigned int extended_service_number : 6;

    // Only present if service_number != 0
    // length: block_size bytes
    char block_data[63];
} dtvcc_service_block_t;

typedef enum {
    eia608_style_white = 0,
    eia608_style_green = 1,
    eia608_style_blue = 2,
    eia608_style_cyan = 3,
    eia608_style_red = 4,
    eia608_style_yellow = 5,
    eia608_style_magenta = 6,
    eia608_style_italics = 7,
} eia608_style_t;

// control command
typedef enum {
    eia608_tab_offset_0 = 0x1720,
    eia608_tab_offset_1 = 0x1721,
    eia608_tab_offset_2 = 0x1722,
    eia608_tab_offset_3 = 0x1723,
    eia608_control_resume_caption_loading = 0x1420,
    eia608_control_backspace = 0x1421,
    eia608_control_alarm_off = 0x1422,
    eia608_control_alarm_on = 0x1423,
    eia608_control_delete_to_end_of_row = 0x1424,
    eia608_control_roll_up_2 = 0x1425,
    eia608_control_roll_up_3 = 0x1426,
    eia608_control_roll_up_4 = 0x1427,
    eia608_control_resume_direct_captioning = 0x1429,
    eia608_control_text_restart = 0x142A,
    eia608_control_text_resume_text_display = 0x142B,
    eia608_control_erase_display_memory = 0x142C,
    eia608_control_carriage_return = 0x142D,
    eia608_control_erase_non_displayed_memory = 0x142E,
    eia608_control_end_of_caption = 0x142F,
} eia608_control_t;

typedef enum {
    cea708_control_end_of_text = 0x03,
    cea708_control_set_current_window_0 = 0x80,
    cea708_control_set_current_window_1 = 0x81,
    cea708_control_set_current_window_2 = 0x82,
    cea708_control_set_current_window_3 = 0x83,
    cea708_control_set_current_window_4 = 0x84,
    cea708_control_set_current_window_5 = 0x85,
    cea708_control_set_current_window_6 = 0x86,
    cea708_control_set_current_window_7 = 0x87,
    cea708_control_clear_windows = 0x88,
    cea708_control_display_windows = 0x89,
    cea708_control_hide_windows = 0x8a,
    cea708_control_toggle_windows = 0x8b,
    cea708_control_delete_windows = 0x8c,
    cea708_control_delay = 0x8d,
    cea708_control_delay_cancel = 0x8e,
    cea708_control_reset = 0x8f,
    cea708_control_set_pen_attributes = 0x90,
    cea708_control_set_pen_color = 0x91,
    cea708_control_set_pen_location = 0x92,
    cea708_control_set_window_attributes = 0x97,
    cea708_control_define_window_0 = 0x98,
    cea708_control_define_window_1 = 0x99,
    cea708_control_define_window_2 = 0x9a,
    cea708_control_define_window_3 = 0x9b,
    cea708_control_define_window_4 = 0x9c,
    cea708_control_define_window_5 = 0x9d,
    cea708_control_define_window_6 = 0x9e,
    cea708_control_define_window_7 = 0x9f,
} cea708_control_t;

typedef enum {
    cea708_predefined_window_style_none = 0,
    cea708_predefined_window_style_608_pop_up = 1,
    cea708_predefined_window_style_transparent_pop_up = 2,
    cea708_predefined_window_style_608_pop_up_centered = 3,
    cea708_predefined_window_style_608_roll_up = 4,
    cea708_predefined_window_style_transparent_roll_up = 5,
    cea708_predefined_window_style_608_roll_up_centered = 6,
    cea708_predefined_window_style_ticker_tape = 7,
} cea708_predefined_window_style_t;

typedef enum {
    cea708_predefined_pen_style_none = 0,
    cea708_predefined_pen_style_default = 1,
    cea708_predefined_pen_style_mono_serif = 2,
    cea708_predefined_pen_style_proportional_serif = 3,
    cea708_predefined_pen_style_mono_sans = 4,
    cea708_predefined_pen_style_proportional_sans = 5,
    cea708_predefined_pen_style_mono_sans_transparent = 6,
    cea708_predefined_pen_style_prop_sans_transparent = 7,
} cea708_predefined_pen_style_t;

typedef enum {
    cea708_anchor_point_top_left = 0,
    cea708_anchor_point_top = 1,
    cea708_anchor_point_top_right = 2,
    cea708_anchor_point_left = 3,
    cea708_anchor_point_center = 4,
    cea708_anchor_point_right = 5,
    cea708_anchor_point_bottom_left = 6,
    cea708_anchor_point_bottom = 7,
} cea708_anchor_point_t;

typedef struct {
    unsigned int priority : 3;
    unsigned int column_lock : 1;
    unsigned int row_lock : 1;
    unsigned int visible : 1;
    unsigned int _padding0 : 2;

    unsigned int anchor_vertical : 7;
    unsigned int _padding1 : 1;

    unsigned int anchor_horizontal : 8;

    unsigned int row_count : 4;
    // cea708_anchor_point_t
    unsigned int anchor_point : 4;

    unsigned int column_count : 6;
    unsigned int _padding2 : 2;

    // cea708_predefined_pen_style_t
    unsigned int predefined_pen_style : 3;
    // cea708_predefined_window_style_t
    unsigned int predefined_window_style : 3;
    unsigned int _padding3 : 2;
} cea708_define_window_t;

#ifdef __cplusplus
}
#endif
#endif // LIBCAPTION_CEA708_TYPES_H
