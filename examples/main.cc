#include "vad/vad.hpp"
#include <iostream>

using namespace std;
int main() {
    string file_path = "wave_data/wave_1.wav";
    FILE* f;
    char buf[320];
    size_t nread;

    VadInst* vad = WebRtcVad_Create();
    if (vad == nullptr) {
        printf("create vad failed");
        return EXIT_FAILURE;
    }

    if (WebRtcVad_Init(vad) == -1) {
        printf("vad init failed");
        return EXIT_FAILURE;
    }

    WebRtcVad_set_mode(vad, 2);

    f = fopen(file_path.c_str(), "rb");
    fseek(f, 44, SEEK_SET);

    while (!feof(f)) {
        nread = fread(buf, 1, sizeof(buf), f);
        if (nread != 320) {
            printf("0\n");
            break;
        }
        int ret = WebRtcVad_Process(vad, 16000, reinterpret_cast<int16_t*>(buf), 160, 1);
        if (ret == -1) {
            printf("vad process failed");
            break;
        }
        printf("%d", ret);
    }
    WebRtcVad_Free(vad);
    return EXIT_SUCCESS;
}