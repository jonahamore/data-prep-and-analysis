#include "CameraProvider.hpp"
#include <stdexcept>

CameraProvider::CameraProvider(int cameraIndex) : capture_(cameraIndex) {
    if (capture_.isOpened()) {
        capture_.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
        capture_.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    }
}

CameraProvider::~CameraProvider() {
    if (capture_.isOpened()) {
        capture_.release();
    }
}

bool CameraProvider::isOpened() const {
    return capture_.isOpened();
}

cv::Mat CameraProvider::getFrame() {
    cv::Mat frame;
    capture_ >> frame;
    return frame;
}
