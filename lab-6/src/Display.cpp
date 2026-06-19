#include "Display.hpp"

Display::Display(const std::string& windowName) : windowName_(windowName) {
    cv::namedWindow(windowName_, cv::WINDOW_AUTOSIZE);
}

Display::~Display() {
    cv::destroyWindow(windowName_);
}

void Display::show(const cv::Mat& frame) const {
    cv::imshow(windowName_, frame);
}

const std::string& Display::windowName() const {
    return windowName_;
}
