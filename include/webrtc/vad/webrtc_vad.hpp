#ifndef WEBRTC_VAD_WEBRTC_VAD_HPP
#define WEBRTC_VAD_WEBRTC_VAD_HPP
#include <stddef.h>
#include <stdint.h>

namespace webrtc {
typedef struct WebRtcVadInst VadInst;

#ifdef __cplusplus
extern "C" {
#endif

// Creates an instance to the VAD structure.
VadInst* WebRtcVad_Create(void);

// Frees the dynamic memory of a specified VAD instance.
//
// - handle [i] : Pointer to VAD instance that should be freed.
void WebRtcVad_Free(VadInst* handle);

// Initializes a VAD instance.
//
// - handle [i/o] : Instance that should be initialized.
//
// returns        : 0 - (OK),
//                 -1 - (null pointer or Default mode could not be set).
int WebRtcVad_Init(VadInst* handle);

// Sets the VAD operating mode. A more aggressive (higher mode) VAD is more
// restrictive in reporting speech. Put in other words the probability of being
// speech when the VAD returns 1 is increased with increasing mode. As a
// consequence also the missed detection rate goes up.
//
// - handle [i/o] : VAD instance.
// - mode   [i]   : Aggressiveness mode (0, 1, 2, or 3).
//
// returns        : 0 - (OK),
//                 -1 - (null pointer, mode could not be set or the VAD instance
//                       has not been initialized).
int WebRtcVad_set_mode(VadInst* handle, int mode);

// Calculates a VAD decision for the |audio_frame|. For valid sampling rates
// frame lengths, see the description of WebRtcVad_ValidRatesAndFrameLengths().
//
// - handle       [i/o] : VAD Instance. Needs to be initialized by
//                        WebRtcVad_Init() before call.
// - fs           [i]   : Sampling frequency (Hz): 8000, 16000, or 32000
// - audio_frame  [i]   : Audio frame buffer.
// - frame_length [i]   : Length of audio frame buffer in number of samples.
//
// returns              : 1 - (Active Voice),
//                        0 - (Non-active Voice),
//                       -1 - (Error)
int WebRtcVad_Process(VadInst* handle, int fs, const int16_t* audio_frame, size_t frame_length);

// Checks for valid combinations of |rate| and |frame_length|. We support 10,
// 20 and 30 ms frames and the rates 8000, 16000 and 32000 Hz.
//
// - rate         [i] : Sampling frequency (Hz).
// - frame_length [i] : Speech frame buffer length in number of samples.
//
// returns            : 0 - (valid combination), -1 - (invalid combination)
int WebRtcVad_ValidRateAndFrameLength(int rate, size_t frame_length);

#ifdef __cplusplus
}
#endif
}  // namespace webrtc

#include "webrtc/vad/vad_core.hpp"
namespace webrtc {
static const int kValidRates[] = {8000, 16000, 32000, 48000};
static const size_t kRatesSize = sizeof(kValidRates) / sizeof(*kValidRates);
static const int kMaxFrameLengthMs = 30;

VadInst* WebRtcVad_Create() {
    VadInstT* self = (VadInstT*)malloc(sizeof(VadInstT));
    self->init_flag = 0;
    return (VadInst*)self;
}

void WebRtcVad_Free(VadInst* handle) { free(handle); }

// TODO(bjornv): Move WebRtcVad_InitCore() code here.
int WebRtcVad_Init(VadInst* handle) {
    // Initialize the core VAD component.
    return WebRtcVad_InitCore((VadInstT*)handle);
}

// TODO(bjornv): Move WebRtcVad_set_mode_core() code here.
int WebRtcVad_set_mode(VadInst* handle, int mode) {
    VadInstT* self = (VadInstT*)handle;

    if (handle == NULL) {
        return -1;
    }
    if (self->init_flag != kInitCheck) {
        return -1;
    }

    return WebRtcVad_set_mode_core(self, mode);
}

int WebRtcVad_Process(VadInst* handle, int fs, const int16_t* audio_frame, size_t frame_length) {
    int vad = -1;
    VadInstT* self = (VadInstT*)handle;

    if (handle == NULL) {
        return -1;
    }

    if (self->init_flag != kInitCheck) {
        return -1;
    }
    if (audio_frame == NULL) {
        return -1;
    }
    if (WebRtcVad_ValidRateAndFrameLength(fs, frame_length) != 0) {
        return -1;
    }

    if (fs == 48000) {
        vad = WebRtcVad_CalcVad48khz(self, audio_frame, frame_length);
    } else if (fs == 32000) {
        vad = WebRtcVad_CalcVad32khz(self, audio_frame, frame_length);
    } else if (fs == 16000) {
        vad = WebRtcVad_CalcVad16khz(self, audio_frame, frame_length);
    } else if (fs == 8000) {
        vad = WebRtcVad_CalcVad8khz(self, audio_frame, frame_length);
    }
    return vad;
}

int WebRtcVad_ValidRateAndFrameLength(int rate, size_t frame_length) {
    int return_value = -1;
    size_t i;
    int valid_length_ms;
    size_t valid_length;

    // We only allow 10, 20 or 30 ms frames. Loop through valid frame rates and
    // see if we have a matching pair.
    for (i = 0; i < kRatesSize; i++) {
        if (kValidRates[i] == rate) {
            for (valid_length_ms = 10; valid_length_ms <= kMaxFrameLengthMs; valid_length_ms += 10) {
                valid_length = (size_t)(kValidRates[i] / 1000 * valid_length_ms);
                if (frame_length == valid_length) {
                    return_value = 0;
                    break;
                }
            }
            break;
        }
    }

    return return_value;
}
}  // namespace webrtc
#endif