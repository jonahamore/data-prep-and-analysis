#include "MouseHandler.hpp"

MouseHandler::MouseHandler()
    : drawing_(false), startPoint_(0, 0), currentPoint_(0, 0), wheelDelta_(0) {}

void MouseHandler::callback(int event, int x, int y, int flags, void* userdata) {
    auto* handler = static_cast<MouseHandler*>(userdata);
    if (handler) {
        handler->handle(event, x, y, flags);
    }
}

void MouseHandler::handle(int event, int x, int y, int flags) {
    (void)flags;
    if (event == cv::EVENT_LBUTTONDOWN) {
        drawing_ = true;
        startPoint_ = cv::Point(x, y);
        currentPoint_ = startPoint_;
    } else if (event == cv::EVENT_MOUSEMOVE && drawing_) {
        currentPoint_ = cv::Point(x, y);
    } else if (event == cv::EVENT_LBUTTONUP) {
        drawing_ = false;
        currentPoint_ = cv::Point(x, y);
        rectangles_.push_back(cv::Rect(startPoint_, currentPoint_));
    } else if (event == cv::EVENT_MOUSEWHEEL) {
        wheelDelta_ += cv::getMouseWheelDelta(flags);
    }
}

void MouseHandler::draw(cv::Mat& frame) const {
    for (const auto& rect : rectangles_) {
        cv::rectangle(frame, rect, cv::Scalar(0, 255, 0), 2);
    }
    if (drawing_) {
        cv::rectangle(frame, cv::Rect(startPoint_, currentPoint_), cv::Scalar(0, 255, 255), 2);
    }
}

void MouseHandler::clear() {
    rectangles_.clear();
}

void MouseHandler::setWheelDelta(int delta) {
    wheelDelta_ = delta;
}

int MouseHandler::consumeWheelDelta() {
    int delta = wheelDelta_;
    wheelDelta_ = 0;
    return delta;
}
