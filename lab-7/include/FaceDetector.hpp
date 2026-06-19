#pragma once

#include <atomic>
#include <mutex>
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include <vector>

class FaceDetector {
public:
    FaceDetector(const std::string& prototxtPath,
                 const std::string& modelPath,
                 float confidenceThreshold = 0.5F);
    ~FaceDetector();

    FaceDetector(const FaceDetector&) = delete;
    FaceDetector& operator=(const FaceDetector&) = delete;

    bool isLoaded() const;
    void submitFrame(const cv::Mat& frame);
    std::vector<cv::Rect> getFaces() const;

private:
    void workerLoop();
    std::vector<cv::Rect> detectFaces(const cv::Mat& frame);

    cv::dnn::Net net_;
    float confidenceThreshold_;
    std::thread worker_;
    mutable std::mutex mutex_;
    std::atomic<bool> running_;
    bool loaded_;
    bool hasNewFrame_;
    cv::Mat pendingFrame_;
    std::vector<cv::Rect> faces_;
};
