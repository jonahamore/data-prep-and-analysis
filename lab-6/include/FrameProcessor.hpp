#pragma once

#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"
#include "MouseHandler.hpp"

class FrameProcessor {
public:
    FrameProcessor();

    cv::Mat process(const cv::Mat& frame,
                    KeyProcessor::Mode mode,
                    const KeyProcessor& keyProcessor,
                    const MouseHandler& mouseHandler);

    static void onBrightnessTrackbar(int value, void* userdata);
    void setBrightness(int value);
    int getBrightness() const;

private:
    int frameCounter_;
    int brightness_;
    double fps_;
    int64 lastTick_;

    cv::Mat overlayImage_;

    void updateFps();
    cv::Mat applyBrightness(const cv::Mat& frame) const;
    cv::Mat rotateAndZoom(const cv::Mat& frame, double angle, double zoom) const;
    cv::Mat translate(const cv::Mat& frame, int dx, int dy) const;
    cv::Mat quantize(const cv::Mat& frame) const;
    cv::Mat glitch(const cv::Mat& frame) const;
    cv::Mat addOverlay(const cv::Mat& frame) const;
    void drawHud(cv::Mat& frame, KeyProcessor::Mode mode) const;
};
