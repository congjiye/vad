# VAD

基于 WebRTC 抽取出的 VAD 程序，完整的 WebRTC 程序可以查看官网 [https://chromium.googlesource.com/external/webrtc](https://chromium.googlesource.com/external/webrtc) 查看。

## 安装

这是一个 header-only 的 vad 库，只需将 include 文件中的内容放入到项目中即可。

## 示例

进入到 examples 文件夹下，执行 make 即可测试使用。

### 方式一

使用 c 风格的 vad
```cpp
#include "webrtc/webrtc.hpp"
#include <iostream>

using namespace std;
using namespace webrtc;
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
```

### 方式二

使用 c++ 风格的 vad

```cpp
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
```
