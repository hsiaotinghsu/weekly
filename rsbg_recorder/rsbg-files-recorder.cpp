#include <cstdio>
#include <cstdlib>
#include <exception>
#include <cstring>
#include <memory>
#include <thread>
#include <chrono>

// OpenCV (Webcam)
#include <opencv2/opencv.hpp>

// AR Player
#include <rocksolid/ArPlayerApi.h>

// Data SDK
#include <rocksolid/dsdk.h>
#include <rocksolid/dsdk_data_recorder.h>

// Auto-generated schema header
#include "test2_input_header.hpp"

int main(int argc, const char* argv[])
{
    if (argc < 2) {
        std::fprintf(stderr, "Usage: %s <data.zip>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // 1. 初始化 AR Studio Runtime (ArPlayer)
    rocksolid::ar::player::InitParams params;
    params.argc = argc;
    params.argv = argv;

    std::shared_ptr<rocksolid::ar::player::ArPlayer> player;
    try {
        player = rocksolid::ar::player::makeArPlayer(params);
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Failed to initialize AR Player: %s\n", e.what());
        return EXIT_FAILURE;
    }

    // 2. 取得 Data SDK schemas
    dsdk::schemas_ptr raw_schemas = player->getSchemas();
    if (!raw_schemas) {
        std::fprintf(stderr, "Failed to get Data SDK schemas\n");
        return EXIT_FAILURE;
    }

    // 3. 建立並啟動 rsbg Recorder
    dsdk::data_recorder recorder("output.rsbg");
    if (recorder.start() != dsdk::result::kOk) {
        std::fprintf(stderr, "Failed to start rsbg recorder\n");
        return EXIT_FAILURE;
    }

    // 4. Instantiate generated schemas wrapper
    dsdk::generated::schemas schemas{ raw_schemas };

    // 5. 開啟 Webcam（OpenCV）
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::fprintf(stderr, "Failed to open webcam\n");
        recorder.stop();
        return EXIT_FAILURE;
    }

    const int CAM_WIDTH  = 1920;
    const int CAM_HEIGHT = 1024;
    cap.set(cv::CAP_PROP_FRAME_WIDTH,  CAM_WIDTH);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT);
    std::printf("Webcam opened: %dx%d\n", CAM_WIDTH, CAM_HEIGHT);

    // 6. 主迴圈：Webcam → Schema → rsbg
    const size_t desired_frames = 300; // 約 10 秒 @30 FPS
    size_t frame_count = 0;

    while (frame_count < desired_frames) {
        cv::Mat frame_bgr;
        if (!cap.read(frame_bgr)) break;

        cv::Mat frame_rgb;
        cv::cvtColor(frame_bgr, frame_rgb, cv::COLOR_BGR2RGB);

        if (frame_rgb.cols != CAM_WIDTH || frame_rgb.rows != CAM_HEIGHT)
            cv::resize(frame_rgb, frame_rgb, cv::Size(CAM_WIDTH, CAM_HEIGHT));

        // 寫入 Forward Camera → Camera image
        auto array = schemas.forward_camera.guard_camera_image();
        if (array.data() != nullptr) {
            dsdk::rgb8* dst = reinterpret_cast<dsdk::rgb8*>(array.data());
            for (int y = 0; y < frame_rgb.rows; ++y) {
                const cv::Vec3b* row = frame_rgb.ptr<cv::Vec3b>(y);
                for (int x = 0; x < frame_rgb.cols; ++x) {
                    size_t idx = y * frame_rgb.cols + x;
                    dst[idx].r = row[x][0];
                    dst[idx].g = row[x][1];
                    dst[idx].b = row[x][2];
                }
            }
        }

        // record 一個 data frame
        if (recorder.record(raw_schemas) != dsdk::result::kOk) {
            std::fprintf(stderr, "Failed to record frame\n");
            break;
        }

        frame_count++;
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    // 7. 停止 Recorder
    recorder.stop();
    std::printf("Recording finished, frames: %zu\n", frame_count);

    return EXIT_SUCCESS;
}

