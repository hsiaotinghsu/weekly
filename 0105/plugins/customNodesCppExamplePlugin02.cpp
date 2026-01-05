#include <iostream>
#include <vector>
#include <tuple>
#include <cstring>
#include <opencv2/opencv.hpp>
#include <rocksolid/dsdk.h>
#include "rocksolid/customNodes/cpp/CustomNodes.h"

namespace rocksolid::ar::customNodes::cpp {
    template<>
    struct TypeTraits<dsdk::rgb8> {
        static constexpr bool value = true;
        static TypeInfo info(TypesRegistry& registry) {
            StructTypeInfo s;
            s.name = "dsdk::rgb8";
            s.fields.push_back(StructField{ "r", registry.add<uint8_t>(), false });
            s.fields.push_back(StructField{ "g", registry.add<uint8_t>(), false });
            s.fields.push_back(StructField{ "b", registry.add<uint8_t>(), false });
            return s;
        }
        static void pushToLua(lua_State* L, const dsdk::rgb8& value) {
            lua_newtable(L);
            lua_pushinteger(L, value.r); lua_setfield(L, -2, "r");
            lua_pushinteger(L, value.g); lua_setfield(L, -2, "g");
            lua_pushinteger(L, value.b); lua_setfield(L, -2, "b");
        }
    };
}

namespace cpp_example {
    namespace cpp = rocksolid::ar::customNodes::cpp;

    static cv::VideoCapture g_cap;
    const int CAM_WIDTH  = 1920;
    const int CAM_HEIGHT = 1024;

    // 優化後的開啟邏輯
    bool openWebcam() {
        if (!g_cap.isOpened()) {
            // 使用 V4L2 驅動明確開啟 video0
            g_cap.open(0, cv::CAP_V4L2);
            if (g_cap.isOpened()) {
                // 強制設定格式為 MJPG 以支援高解析度
                g_cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
                g_cap.set(cv::CAP_PROP_FRAME_WIDTH, CAM_WIDTH);
                g_cap.set(cv::CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT);
                
                // 等待攝影機穩定
                cv::Mat tmp;
                for(int i=0; i<5; i++) g_cap.read(tmp); 
                
                std::cout << "[Plugin] Webcam /dev/video0 opened successfully." << std::endl;
            }
        }
        return g_cap.isOpened();
    }

    void createInstance(cpp::NodesRegistry& registry) {
        openWebcam();

        registry.add("cpp_example", "readWebcamDsdk",
            { "Capture RGB8 frame", "Custom Nodes", "02. Webcam to RGB8" },
            []() {
                int w = 0, h = 0;
                std::vector<dsdk::rgb8> rgb_data;

                // 檢查是否斷線並嘗試重連
                if (!g_cap.isOpened()) openWebcam();

                if (g_cap.isOpened()) {
                    cv::Mat frame_bgr;
                    // 使用 grab() 和 retrieve() 確保拿到最新鮮的一幀，避免緩衝區延遲
                    if (g_cap.grab()) {
                        g_cap.retrieve(frame_bgr);
                        if (!frame_bgr.empty()) {
                            cv::Mat frame_rgb;
                            // 即使攝影機設定了尺寸，仍進行 resize 確保輸出符合 Schema
                            cv::resize(frame_bgr, frame_rgb, cv::Size(CAM_WIDTH, CAM_HEIGHT));
                            cv::cvtColor(frame_rgb, frame_rgb, cv::COLOR_BGR2RGB);

                            w = frame_rgb.cols;
                            h = frame_rgb.rows;
                            rgb_data.resize(w * h);
                            std::memcpy(rgb_data.data(), frame_rgb.data, w * h * 3);
                        }
                    }
                }
                
                // 如果失敗，回傳一幀黑畫面或 0
                return std::make_tuple(w, h, rgb_data);
            },
            {}, 
            { "width", "height", "image_frame" }
        );

        registry.subscribeOnDestroy([]() {
            if (g_cap.isOpened()) g_cap.release();
        });
    }
}

RS_DEFINE_CPP_PLUGIN(CustomNodePlugin, 1, 0, cpp_example::createInstance)
