#include <iostream>
#include "webrtc/webrtc.hpp"

using namespace std;
using namespace webrtc;
int main() {
    string file_path = "wave_data/wave_1.wav";
    FILE* f;
    char buf[320];
    size_t nread;

    Vad vad(Vad::kVadAggressive);
    bool ret = vad.Init();
    if (ret == false) {
        return EXIT_FAILURE;
    }

    f = fopen(file_path.c_str(), "rb");
    fseek(f, 44, SEEK_SET);

    while (!feof(f)) {
        nread = fread(buf, 1, sizeof(buf), f);
        if (nread != 320) {
            printf("0\n");
            break;
        }
        int ret = vad.IsSpeech(reinterpret_cast<int16_t*>(buf), 160, 16000);
        if (ret == Vad::kError) {
            printf("vad process failed");
            break;
        }
        printf("%d", ret > 0 ? 1 : 0);
    }
    fclose(f);
    return EXIT_SUCCESS;
}