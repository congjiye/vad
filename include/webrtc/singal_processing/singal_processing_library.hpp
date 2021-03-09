#ifndef WEBRTC_SINGAL_PROCESSING_SINGAL_PROCESSING_LIBRARY_HPP
#define WEBRTC_SINGAL_PROCESSING_SINGAL_PROCESSING_LIBRARY_HPP
#include "webrtc/common.hpp"

// Macros specific for the fixed point implementation
#define WEBRTC_SPL_WORD16_MAX 32767
#define WEBRTC_SPL_WORD16_MIN -32768
#define WEBRTC_SPL_WORD32_MAX (int32_t)0x7fffffff
#define WEBRTC_SPL_WORD32_MIN (int32_t)0x80000000

#define WEBRTC_SPL_MUL(a, b) ((int32_t)((int32_t)(a) * (int32_t)(b)))

#include "webrtc/singal_processing/spl_inl.hpp"

namespace webrtc {

int16_t WebRtcSpl_GetScalingSquare(int16_t* in_vector, size_t in_vector_length, size_t times);

// Divisions. Implementations collected in division_operations.c and
// descriptions at bottom of this file.
int32_t WebRtcSpl_DivW32W16(int32_t num, int16_t den);
// End: Divisions.

int32_t WebRtcSpl_Energy(int16_t* vector, size_t vector_length, int* scale_factor);


/************************************************************
 *
 * RESAMPLING FUNCTIONS AND THEIR STRUCTS ARE DEFINED BELOW
 *
 ************************************************************/

/*******************************************************************
 * resample_fractional.c
 * Functions for internal use in the other resample functions
 *
 * Includes the following resampling combinations
 * 48 kHz -> 32 kHz
 *
 ******************************************************************/

void WebRtcSpl_Resample48khzTo32khz(const int32_t* In, int32_t* Out, size_t K);

/*******************************************************************
 * resample_48khz.c
 *
 * Includes the following resampling combinations
 * 48 kHz ->  8 kHz
 *
 ******************************************************************/

typedef struct {
    int32_t S_48_24[8];
    int32_t S_24_24[16];
    int32_t S_24_16[8];
    int32_t S_16_8[8];
} WebRtcSpl_State48khzTo8khz;

void WebRtcSpl_Resample48khzTo8khz(const int16_t* in, int16_t* out, WebRtcSpl_State48khzTo8khz* state, int32_t* tmpmem);

void WebRtcSpl_ResetResample48khzTo8khz(WebRtcSpl_State48khzTo8khz* state);

/************************************************************
 * END OF RESAMPLING FUNCTIONS
 ************************************************************/

int32_t WebRtcSpl_DivW32W16(int32_t num, int16_t den) {
    // Guard against division with 0
    if (den != 0) {
        return (int32_t)(num / den);
    } else {
        return (int32_t)0x7FFFFFFF;
    }
}

int32_t WebRtcSpl_Energy(int16_t* vector, size_t vector_length, int* scale_factor) {
    int32_t en = 0;
    size_t i;
    int scaling = WebRtcSpl_GetScalingSquare(vector, vector_length, vector_length);
    size_t looptimes = vector_length;
    int16_t* vectorptr = vector;

    for (i = 0; i < looptimes; i++) {
        en += (*vectorptr * *vectorptr) >> scaling;
        vectorptr++;
    }
    *scale_factor = scaling;

    return en;
}

int16_t WebRtcSpl_GetScalingSquare(int16_t* in_vector, size_t in_vector_length, size_t times) {
    int16_t nbits = WebRtcSpl_GetSizeInBits((uint32_t)times);
    size_t i;
    int16_t smax = -1;
    int16_t sabs;
    int16_t* sptr = in_vector;
    int16_t t;
    size_t looptimes = in_vector_length;

    for (i = looptimes; i > 0; i--) {
        sabs = (*sptr > 0 ? *sptr++ : -*sptr++);
        smax = (sabs > smax ? sabs : smax);
    }
    t = WebRtcSpl_NormW32(WEBRTC_SPL_MUL(smax, smax));

    if (smax == 0) {
        return 0;  // Since norm(0) returns 0
    } else {
        return (t > nbits) ? 0 : nbits - t;
    }
}

// interpolation coefficients
static const int16_t kCoefficients48To32[2][8] = {{778, -2050, 1087, 23285, 12903, -3783, 441, 222},
                                                  {222, 441, -3783, 12903, 23285, 1087, -2050, 778}};

//   Resampling ratio: 2/3
// input:  int32_t (normalized, not saturated) :: size 3 * K
// output: int32_t (shifted 15 positions to the left, + offset 16384) :: size 2 * K
//      K: number of blocks

void WebRtcSpl_Resample48khzTo32khz(const int32_t* In, int32_t* Out, size_t K) {
    /////////////////////////////////////////////////////////////
    // Filter operation:
    //
    // Perform resampling (3 input samples -> 2 output samples);
    // process in sub blocks of size 3 samples.
    int32_t tmp;
    size_t m;

    for (m = 0; m < K; m++) {
        tmp = 1 << 14;
        tmp += kCoefficients48To32[0][0] * In[0];
        tmp += kCoefficients48To32[0][1] * In[1];
        tmp += kCoefficients48To32[0][2] * In[2];
        tmp += kCoefficients48To32[0][3] * In[3];
        tmp += kCoefficients48To32[0][4] * In[4];
        tmp += kCoefficients48To32[0][5] * In[5];
        tmp += kCoefficients48To32[0][6] * In[6];
        tmp += kCoefficients48To32[0][7] * In[7];
        Out[0] = tmp;

        tmp = 1 << 14;
        tmp += kCoefficients48To32[1][0] * In[1];
        tmp += kCoefficients48To32[1][1] * In[2];
        tmp += kCoefficients48To32[1][2] * In[3];
        tmp += kCoefficients48To32[1][3] * In[4];
        tmp += kCoefficients48To32[1][4] * In[5];
        tmp += kCoefficients48To32[1][5] * In[6];
        tmp += kCoefficients48To32[1][6] * In[7];
        tmp += kCoefficients48To32[1][7] * In[8];
        Out[1] = tmp;

        // update pointers
        In += 3;
        Out += 2;
    }
}
}  // namespace webrtc

#include <string.h>

#include "webrtc/singal_processing/resample_by_2_internal.hpp"

////////////////////////////
///// 48 kHz ->  8 kHz /////
////////////////////////////

namespace webrtc {
// 48 -> 8 resampler
void WebRtcSpl_Resample48khzTo8khz(const int16_t* in, int16_t* out, WebRtcSpl_State48khzTo8khz* state,
                                   int32_t* tmpmem) {
    ///// 48 --> 24 /////
    // int16_t  in[480]
    // int32_t out[240]
    /////
    WebRtcSpl_DownBy2ShortToInt(in, 480, tmpmem + 256, state->S_48_24);

    ///// 24 --> 24(LP) /////
    // int32_t  in[240]
    // int32_t out[240]
    /////
    WebRtcSpl_LPBy2IntToInt(tmpmem + 256, 240, tmpmem + 16, state->S_24_24);

    ///// 24 --> 16 /////
    // int32_t  in[240]
    // int32_t out[160]
    /////
    // copy state to and from input array
    memcpy(tmpmem + 8, state->S_24_16, 8 * sizeof(int32_t));
    memcpy(state->S_24_16, tmpmem + 248, 8 * sizeof(int32_t));
    WebRtcSpl_Resample48khzTo32khz(tmpmem + 8, tmpmem, 80);

    ///// 16 --> 8 /////
    // int32_t  in[160]
    // int16_t out[80]
    /////
    WebRtcSpl_DownBy2IntToShort(tmpmem, 160, out, state->S_16_8);
}

// initialize state of 48 -> 8 resampler
void WebRtcSpl_ResetResample48khzTo8khz(WebRtcSpl_State48khzTo8khz* state) {
    memset(state->S_48_24, 0, 8 * sizeof(int32_t));
    memset(state->S_24_24, 0, 16 * sizeof(int32_t));
    memset(state->S_24_16, 0, 8 * sizeof(int32_t));
    memset(state->S_16_8, 0, 8 * sizeof(int32_t));
}
}  // namespace webrtc
#endif