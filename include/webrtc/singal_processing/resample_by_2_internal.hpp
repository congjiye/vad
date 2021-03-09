#ifndef WEBRTC_SINGAL_PROCESSING_RESAMPLE_BY_2_INTERNAL_HPP
#define WEBRTC_SINGAL_PROCESSING_RESAMPLE_BY_2_INTERNAL_HPP
#include "webrtc/common.hpp"

namespace webrtc {
// allpass filter coefficients.
static const int16_t kResampleAllpass[2][3] = {{821, 6110, 12382}, {3050, 9368, 15063}};

/*******************************************************************
 * resample_by_2_fast.c
 * Functions for internal use in the other resample functions
 ******************************************************************/
void WebRtcSpl_DownBy2IntToShort(int32_t *in, int32_t len, int16_t *out, int32_t *state);

void WebRtcSpl_DownBy2ShortToInt(const int16_t *in, int32_t len, int32_t *out, int32_t *state);

void WebRtcSpl_LPBy2IntToInt(const int32_t *in, int32_t len, int32_t *out, int32_t *state);

void RTC_NO_SANITIZE("signed-integer-overflow")
    WebRtcSpl_DownBy2IntToShort(int32_t *in, int32_t len, int16_t *out, int32_t *state) {
    int32_t tmp0, tmp1, diff;
    int32_t i;

    len >>= 1;

    // lower allpass filter (operates on even input samples)
    for (i = 0; i < len; i++) {
        tmp0 = in[i << 1];
        diff = tmp0 - state[1];
        // UBSan: -1771017321 - 999586185 cannot be represented in type 'int'

        // scale down and round
        diff = (diff + (1 << 13)) >> 14;
        tmp1 = state[0] + diff * kResampleAllpass[1][0];
        state[0] = tmp0;
        diff = tmp1 - state[2];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0) diff += 1;
        tmp0 = state[1] + diff * kResampleAllpass[1][1];
        state[1] = tmp1;
        diff = tmp0 - state[3];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0) diff += 1;
        state[3] = state[2] + diff * kResampleAllpass[1][2];
        state[2] = tmp0;

        // divide by two and store temporarily
        in[i << 1] = (state[3] >> 1);
    }

    in++;

    // upper allpass filter (operates on odd input samples)
    for (i = 0; i < len; i++) {
        tmp0 = in[i << 1];
        diff = tmp0 - state[5];
        // scale down and round
        diff = (diff + (1 << 13)) >> 14;
        tmp1 = state[4] + diff * kResampleAllpass[0][0];
        state[4] = tmp0;
        diff = tmp1 - state[6];
        // scale down and round
        diff = diff >> 14;
        if (diff < 0) diff += 1;
        tmp0 = state[5] + diff * kResampleAllpass[0][1];
        state[5] = tmp1;
        diff = tmp0 - state[7];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0) diff += 1;
        state[7] = state[6] + diff * kResampleAllpass[0][2];
        state[6] = tmp0;

        // divide by two and store temporarily
        in[i << 1] = (state[7] >> 1);
    }

    in--;

    // combine allpass outputs
    for (i = 0; i < len; i += 2) {
        // divide by two, add both allpass outputs and round
        tmp0 = (in[i << 1] + in[(i << 1) + 1]) >> 15;
        tmp1 = (in[(i << 1) + 2] + in[(i << 1) + 3]) >> 15;
        if (tmp0 > (int32_t)0x00007FFF) tmp0 = 0x00007FFF;
        if (tmp0 < (int32_t)0xFFFF8000) tmp0 = 0xFFFF8000;
        out[i] = (int16_t)tmp0;
        if (tmp1 > (int32_t)0x00007FFF) tmp1 = 0x00007FFF;
        if (tmp1 < (int32_t)0xFFFF8000) tmp1 = 0xFFFF8000;
        out[i + 1] = (int16_t)tmp1;
    }
}

void RTC_NO_SANITIZE("signed-integer-overflow")
    WebRtcSpl_DownBy2ShortToInt(const int16_t *in, int32_t len, int32_t *out, int32_t *state) {
    int32_t tmp0, tmp1, diff;
    int32_t i;

    len >>= 1;

    // lower allpass filter (operates on even input samples)
    for (i = 0; i < len; i++) {
        tmp0 = ((int32_t)in[i << 1] << 15) + (1 << 14);
        diff = tmp0 - state[1];
        // scale down and round
        diff = (diff + (1 << 13)) >> 14;
        tmp1 = state[0] + diff * kResampleAllpass[1][0];
        state[0] = tmp0;
        diff = tmp1 - state[2];
        // UBSan: -1379909682 - 834099714 cannot be represented in type 'int'

        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0) diff += 1;
        tmp0 = state[1] + diff * kResampleAllpass[1][1];
        state[1] = tmp1;
        diff = tmp0 - state[3];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0) diff += 1;
        state[3] = state[2] + diff * kResampleAllpass[1][2];
        state[2] = tmp0;

        // divide by two and store temporarily
        out[i] = (state[3] >> 1);
    }

    in++;

    // upper allpass filter (operates on odd input samples)
    for (i = 0; i < len; i++) {
        tmp0 = ((int32_t)in[i << 1] << 15) + (1 << 14);
        diff = tmp0 - state[5];
        // scale down and round
        diff = (diff + (1 << 13)) >> 14;
        tmp1 = state[4] + diff * kResampleAllpass[0][0];
        state[4] = tmp0;
        diff = tmp1 - state[6];
        // scale down and round
        diff = diff >> 14;
        if (diff < 0) diff += 1;
        tmp0 = state[5] + diff * kResampleAllpass[0][1];
        state[5] = tmp1;
        diff = tmp0 - state[7];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0) diff += 1;
        state[7] = state[6] + diff * kResampleAllpass[0][2];
        state[6] = tmp0;

        // divide by two and store temporarily
        out[i] += (state[7] >> 1);
    }

    in--;
}

void RTC_NO_SANITIZE("signed-integer-overflow")
    WebRtcSpl_LPBy2IntToInt(const int32_t *in, int32_t len, int32_t *out, int32_t *state) {
    int32_t tmp0, tmp1, diff;
    int32_t i;

    len >>= 1;

    // lower allpass filter: odd input -> even output samples
    in++;
    // initial state of polyphase delay element
    tmp0 = state[12];
    for (i = 0; i < len; i++) {
        diff = tmp0 - state[1];
        // scale down and round
        diff = (diff + (1 << 13)) >> 14;
        tmp1 = state[0] + diff * kResampleAllpass[1][0];
        state[0] = tmp0;
        diff = tmp1 - state[2];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0) diff += 1;
        tmp0 = state[1] + diff * kResampleAllpass[1][1];
        state[1] = tmp1;
        diff = tmp0 - state[3];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0) diff += 1;
        state[3] = state[2] + diff * kResampleAllpass[1][2];
        state[2] = tmp0;

        // scale down, round and store
        out[i << 1] = state[3] >> 1;
        tmp0 = in[i << 1];
    }
    in--;

    // upper allpass filter: even input -> even output samples
    for (i = 0; i < len; i++) {
        tmp0 = in[i << 1];
        diff = tmp0 - state[5];
        // UBSan: -794814117 - 1566149201 cannot be represented in type 'int'

        // scale down and round
        diff = (diff + (1 << 13)) >> 14;
        tmp1 = state[4] + diff * kResampleAllpass[0][0];
        state[4] = tmp0;
        diff = tmp1 - state[6];
        // scale down and round
        diff = diff >> 14;
        if (diff < 0) diff += 1;
        tmp0 = state[5] + diff * kResampleAllpass[0][1];
        state[5] = tmp1;
        diff = tmp0 - state[7];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0) diff += 1;
        state[7] = state[6] + diff * kResampleAllpass[0][2];
        state[6] = tmp0;

        // average the two allpass outputs, scale down and store
        out[i << 1] = (out[i << 1] + (state[7] >> 1)) >> 15;
    }

    // switch to odd output samples
    out++;

    // lower allpass filter: even input -> odd output samples
    for (i = 0; i < len; i++) {
        tmp0 = in[i << 1];
        diff = tmp0 - state[9];
        // scale down and round
        diff = (diff + (1 << 13)) >> 14;
        tmp1 = state[8] + diff * kResampleAllpass[1][0];
        state[8] = tmp0;
        diff = tmp1 - state[10];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0) diff += 1;
        tmp0 = state[9] + diff * kResampleAllpass[1][1];
        state[9] = tmp1;
        diff = tmp0 - state[11];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0) diff += 1;
        state[11] = state[10] + diff * kResampleAllpass[1][2];
        state[10] = tmp0;

        // scale down, round and store
        out[i << 1] = state[11] >> 1;
    }

    // upper allpass filter: odd input -> odd output samples
    in++;
    for (i = 0; i < len; i++) {
        tmp0 = in[i << 1];
        diff = tmp0 - state[13];
        // scale down and round
        diff = (diff + (1 << 13)) >> 14;
        tmp1 = state[12] + diff * kResampleAllpass[0][0];
        state[12] = tmp0;
        diff = tmp1 - state[14];
        // scale down and round
        diff = diff >> 14;
        if (diff < 0) diff += 1;
        tmp0 = state[13] + diff * kResampleAllpass[0][1];
        state[13] = tmp1;
        diff = tmp0 - state[15];
        // scale down and truncate
        diff = diff >> 14;
        if (diff < 0) diff += 1;
        state[15] = state[14] + diff * kResampleAllpass[0][2];
        state[14] = tmp0;

        // average the two allpass outputs, scale down and store
        out[i << 1] = (out[i << 1] + (state[15] >> 1)) >> 15;
    }
}
}  // namespace webrtc
#endif