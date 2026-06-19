#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

class MouseHandler {
public:
    MouseHandler();

    static void callback(int event, int x, int y, int flags, void* userdata);

    void handle(int event, int x, int y, int flags);
    void draw(cv::Mat& frame) const;
    void clear();

    void setWheelDelta(int delta);
    int consumeWheelDelta();

private:
    bool drawing_;
    cv::Point startPoint_;
    cv::Point currentPoint_;
    std::vector<cv::Rect> rectangles_;
    int wheelDelta_;
};
