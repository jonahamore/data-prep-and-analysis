#include "FaceDetector.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>

FaceDetector::FaceDetector(const std::string& prototxtPath,
                           const std::string& modelPath,
                           float confidenceThreshold)
    : confidenceThreshold_(confidenceThreshold),
      running_(true),
      loaded_(false),
      hasNewFrame_(false) {
    try {
        net_ = cv::dnn::readNetFromCaffe(prototxtPath, modelPath);
        loaded_ = !net_.empty();
    } catch (const cv::Exception& exception) {
        std::cerr << "Face detector was not loaded: " << exception.what() << '\n';
        loaded_ = false;
    }

    if (loaded_) {
        worker_ = std::thread(&FaceDetector::workerLoop, this);
    }
}

FaceDetector::~FaceDetector() {
    running_ = false;

    if (worker_.joinable()) {
        worker_.join();
    }
}

bool FaceDetector::isLoaded() const {
    return loaded_;
}

void FaceDetector::submitFrame(const cv::Mat& frame) {
    if (!loaded_ || frame.empty()) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingFrame_ = frame.clone();
        hasNewFrame_ = true;
    }
}

std::vector<cv::Rect> FaceDetector::getFaces() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return faces_;
}

void FaceDetector::workerLoop() {
    while (running_) {
        cv::Mat frameForDetection;
        bool gotFrame = false;

        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (hasNewFrame_) {
                frameForDetection = pendingFrame_.clone();
                hasNewFrame_ = false;
                gotFrame = true;
            }
        }

        if (gotFrame) {
            // Для демонстрації різниці з однопотоковим режимом можна
            // штучно сповільнити детектор, розкоментувавши рядок нижче.
            // Відео при цьому залишається плавним, бо детекція у фоні.
            // std::this_thread::sleep_for(std::chrono::milliseconds(500));

            auto detectedFaces = detectFaces(frameForDetection);

            std::lock_guard<std::mutex> lock(mutex_);
            faces_ = std::move(detectedFaces);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

std::vector<cv::Rect> FaceDetector::detectFaces(const cv::Mat& frame) {
    std::vector<cv::Rect> result;

    if (frame.empty()) {
        return result;
    }

    cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300, 300),
                                          cv::Scalar(104.0, 177.0, 123.0));
    net_.setInput(blob);

    cv::Mat detections = net_.forward();
    cv::Mat detectionMatrix(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());

    for (int i = 0; i < detectionMatrix.rows; ++i) {
        float confidence = detectionMatrix.at<float>(i, 2);

        if (confidence < confidenceThreshold_) {
            continue;
        }

        int x1 = static_cast<int>(detectionMatrix.at<float>(i, 3) * frame.cols);
        int y1 = static_cast<int>(detectionMatrix.at<float>(i, 4) * frame.rows);
        int x2 = static_cast<int>(detectionMatrix.at<float>(i, 5) * frame.cols);
        int y2 = static_cast<int>(detectionMatrix.at<float>(i, 6) * frame.rows);

        x1 = std::clamp(x1, 0, frame.cols - 1);
        y1 = std::clamp(y1, 0, frame.rows - 1);
        x2 = std::clamp(x2, 0, frame.cols - 1);
        y2 = std::clamp(y2, 0, frame.rows - 1);

        if (x2 > x1 && y2 > y1) {
            result.emplace_back(cv::Point(x1, y1), cv::Point(x2, y2));
        }
    }

    return result;
}
