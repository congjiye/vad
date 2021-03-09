#ifndef WEBRTC_VAD_VAD_HPP
#define WEBRTC_VAD_VAD_HPP
#include <exception>
#include <memory>

#include "webrtc/vad/webrtc_vad.hpp"

namespace webrtc {
class Vad : public std::enable_shared_from_this<Vad> {
public:
    enum Aggressiveness { kVadNormal = 0, kVadLowBitrate = 1, kVadAggressive = 2, kVadVeryAggressive = 3 };

    enum Activity { kPassive = 0, kActive = 1, kError = -1 };
    explicit Vad(Aggressiveness aggressiveness) : handle_(nullptr), aggressiveness_(aggressiveness) {}

    Activity IsSpeech(const int16_t* audio, size_t num_samples, int sample_rate_hz) {
        int ret = WebRtcVad_Process(handle_, sample_rate_hz, audio, num_samples);
        switch (ret) {
            case 0:
                return kPassive;
            case 1:
                return kActive;
            default:
                return kError;
        }
    }

    bool Init() {
        Reset();
        if (handle_ == nullptr) {
            printf("Create vad handle failed.\n");
            return false;
        }
        if (WebRtcVad_Init(handle_) == -1) {
            printf("Init vad handle failed.\n");
            return false;
        }
        if (WebRtcVad_set_mode(handle_, aggressiveness_) == -1) {
            printf("Set vad mode failed.\n");
            return false;
        }
        return true;
    }

    void Reset() {
        if (handle_) {
            WebRtcVad_Free(handle_);
        }
        handle_ = WebRtcVad_Create();
    }

    ~Vad() { WebRtcVad_Free(handle_); }

    void set_aggressiveness(Vad::Aggressiveness aggressiveness) { aggressiveness_ = aggressiveness; }

private:
    VadInst* handle_;
    Aggressiveness aggressiveness_;
};
}  // namespace webrtc
#endif