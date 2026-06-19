#include "FrameProcessor.hpp"
#include <iomanip>
#include <sstream>
#include <vector>

FrameProcessor::FrameProcessor()
    : frameCounter_(0), brightness_(100), fps_(0.0), lastTick_(cv::getTickCount()) {
    overlayImage_ = cv::imread("assets/overlay.png", cv::IMREAD_UNCHANGED);
}

cv::Mat FrameProcessor::process(const cv::Mat& frame,
                                KeyProcessor::Mode mode,
                                const KeyProcessor& keyProcessor,
                                const MouseHandler& mouseHandler) {
    frameCounter_++;
    updateFps();

    cv::Mat result = applyBrightness(frame);

    switch (mode) {
        case KeyProcessor::Mode::Normal:
            break;
        case KeyProcessor::Mode::Invert:
            cv::bitwise_not(result, result);
            break;
        case KeyProcessor::Mode::Blur:
            cv::GaussianBlur(result, result, cv::Size(21, 21), 0);
            break;
        case KeyProcessor::Mode::Canny: {
            cv::Mat gray, edges;
            cv::cvtColor(result, gray, cv::COLOR_BGR2GRAY);
            cv::Canny(gray, edges, 80, 160);
            cv::cvtColor(edges, result, cv::COLOR_GRAY2BGR);
            break;
        }
        case KeyProcessor::Mode::Sobel: {
            cv::Mat gray, gradX, gradY, absX, absY;
            cv::cvtColor(result, gray, cv::COLOR_BGR2GRAY);
            cv::Sobel(gray, gradX, CV_16S, 1, 0, 3);
            cv::Sobel(gray, gradY, CV_16S, 0, 1, 3);
            cv::convertScaleAbs(gradX, absX);
            cv::convertScaleAbs(gradY, absY);
            cv::addWeighted(absX, 0.5, absY, 0.5, 0, result);
            cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
            break;
        }
        case KeyProcessor::Mode::Binary: {
            cv::Mat gray, bin;
            cv::cvtColor(result, gray, cv::COLOR_BGR2GRAY);
            cv::threshold(gray, bin, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
            cv::cvtColor(bin, result, cv::COLOR_GRAY2BGR);
            break;
        }
        case KeyProcessor::Mode::Quantize:
            result = quantize(result);
            break;
        case KeyProcessor::Mode::Glitch:
            result = glitch(result);
            break;
        case KeyProcessor::Mode::Overlay:
            result = addOverlay(result);
            break;
        case KeyProcessor::Mode::Draw:
            mouseHandler.draw(result);
            break;
    }

    result = rotateAndZoom(result, keyProcessor.getRotationAngle(), keyProcessor.getZoom());
    result = translate(result, keyProcessor.getOffsetX(), keyProcessor.getOffsetY());

    drawHud(result, mode);
    return result;
}

void FrameProcessor::onBrightnessTrackbar(int value, void* userdata) {
    auto* processor = static_cast<FrameProcessor*>(userdata);
    if (processor) {
        processor->setBrightness(value);
    }
}

void FrameProcessor::setBrightness(int value) {
    brightness_ = value;
}

int FrameProcessor::getBrightness() const {
    return brightness_;
}

void FrameProcessor::updateFps() {
    int64 now = cv::getTickCount();
    double seconds = static_cast<double>(now - lastTick_) / cv::getTickFrequency();
    if (seconds > 0.0) {
        fps_ = 0.9 * fps_ + 0.1 * (1.0 / seconds);
    }
    lastTick_ = now;
}

cv::Mat FrameProcessor::applyBrightness(const cv::Mat& frame) const {
    cv::Mat result;
    int beta = brightness_ - 100;
    frame.convertTo(result, -1, 1.0, beta);
    return result;
}

cv::Mat FrameProcessor::rotateAndZoom(const cv::Mat& frame, double angle, double zoom) const {
    cv::Point2f center(frame.cols / 2.0F, frame.rows / 2.0F);
    cv::Mat matrix = cv::getRotationMatrix2D(center, angle, zoom);
    cv::Mat result;
    cv::warpAffine(frame, result, matrix, frame.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT);
    return result;
}

cv::Mat FrameProcessor::translate(const cv::Mat& frame, int dx, int dy) const {
    cv::Mat matrix = (cv::Mat_<double>(2, 3) << 1, 0, dx, 0, 1, dy);
    cv::Mat result;
    cv::warpAffine(frame, result, matrix, frame.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT);
    return result;
}

cv::Mat FrameProcessor::quantize(const cv::Mat& frame) const {
    cv::Mat result = frame.clone();
    const int levels = 4;
    const int step = 256 / levels;
    for (int y = 0; y < result.rows; ++y) {
        for (int x = 0; x < result.cols; ++x) {
            cv::Vec3b& pixel = result.at<cv::Vec3b>(y, x);
            for (int c = 0; c < 3; ++c) {
                pixel[c] = static_cast<uchar>((pixel[c] / step) * step + step / 2);
            }
        }
    }
    return result;
}

cv::Mat FrameProcessor::glitch(const cv::Mat& frame) const {
    std::vector<cv::Mat> channels;
    cv::split(frame, channels);

    auto shift = [](const cv::Mat& src, int dx, int dy) {
        cv::Mat matrix = (cv::Mat_<double>(2, 3) << 1, 0, dx, 0, 1, dy);
        cv::Mat dst;
        cv::warpAffine(src, dst, matrix, src.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT);
        return dst;
    };

    channels[0] = shift(channels[0], -10, 0);
    channels[1] = shift(channels[1], 0, 8);
    channels[2] = shift(channels[2], 10, 0);

    cv::Mat result;
    cv::merge(channels, result);
    return result;
}

cv::Mat FrameProcessor::addOverlay(const cv::Mat& frame) const {
    cv::Mat result = frame.clone();

    if (!overlayImage_.empty()) {
        cv::Mat resized;
        int width = result.cols / 4;
        int height = overlayImage_.rows * width / overlayImage_.cols;
        cv::resize(overlayImage_, resized, cv::Size(width, height));

        int x0 = result.cols - resized.cols - 20;
        int y0 = 20;
        cv::Rect roi(x0, y0, resized.cols, resized.rows);

        if (resized.channels() == 4) {
            std::vector<cv::Mat> bgra;
            cv::split(resized, bgra);
            cv::Mat bgr, alpha;
            cv::merge(std::vector<cv::Mat>{bgra[0], bgra[1], bgra[2]}, bgr);
            alpha = bgra[3];
            bgr.copyTo(result(roi), alpha);
        } else {
            resized.copyTo(result(roi));
        }
    } else {
        cv::rectangle(result, cv::Rect(result.cols - 220, 20, 200, 120), cv::Scalar(50, 50, 50), cv::FILLED);
        cv::putText(result, "Picture-in-picture", cv::Point(result.cols - 210, 80), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
    }

    return result;
}

void FrameProcessor::drawHud(cv::Mat& frame, KeyProcessor::Mode mode) const {
    cv::Scalar textColor(255, 255, 255);
    cv::Scalar bgColor(0, 0, 0);
    cv::rectangle(frame, cv::Rect(10, 10, 540, 135), bgColor, cv::FILLED);

    std::ostringstream fpsText;
    fpsText << std::fixed << std::setprecision(1) << fps_;

    cv::Scalar meanColor = cv::mean(frame);
    std::ostringstream meanText;
    meanText << "Mean BGR: " << static_cast<int>(meanColor[0]) << ", "
             << static_cast<int>(meanColor[1]) << ", " << static_cast<int>(meanColor[2]);

    std::string modeText = "Mode: " + std::to_string(static_cast<int>(mode)) + " | 0-9 switch modes";
    cv::putText(frame, modeText, cv::Point(20, 35), cv::FONT_HERSHEY_SIMPLEX, 0.55, textColor, 1);
    cv::putText(frame, "Frames: " + std::to_string(frameCounter_) + " | FPS: " + fpsText.str(), cv::Point(20, 60), cv::FONT_HERSHEY_SIMPLEX, 0.55, textColor, 1);
    cv::putText(frame, meanText.str(), cv::Point(20, 85), cv::FONT_HERSHEY_SIMPLEX, 0.55, textColor, 1);
    cv::putText(frame, "A/D rotate, +/- zoom, I/J/K/L move, R reset, Q/ESC quit", cv::Point(20, 110), cv::FONT_HERSHEY_SIMPLEX, 0.5, textColor, 1);
    cv::putText(frame, "Mouse: drag in mode 9 to draw rectangles, wheel changes zoom", cv::Point(20, 132), cv::FONT_HERSHEY_SIMPLEX, 0.5, textColor, 1);
}
