#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>

#include "CameraProvider.hpp"
#include "Display.hpp"
#include "FrameProcessor.hpp"
#include "KeyProcessor.hpp"
#include "MouseHandler.hpp"

int main(int argc, char** argv) {
    const std::string windowName = "Lab 6";

    // Camera index can be passed as the first CLI argument.
    // Useful for virtual cameras such as DroidCam, which are often
    // not /dev/video0. Default is 0. Example: ./run.sh 2
    int cameraIndex = 0;
    if (argc > 1) {
        cameraIndex = std::atoi(argv[1]);
    }

    CameraProvider camera(cameraIndex);
    if (!camera.isOpened()) {
        std::cerr << "Error: camera with index " << cameraIndex
                  << " was not opened.\n"
                  << "Check that a webcam (or DroidCam) is connected and available.\n"
                  << "List available devices with: v4l2-ctl --list-devices\n"
                  << "Then run, for example: ./run.sh 2\n";
        return 1;
    }

    Display display(windowName);
    KeyProcessor keyProcessor;
    FrameProcessor frameProcessor;
    MouseHandler mouseHandler;

    int brightnessValue = frameProcessor.getBrightness();
    cv::createTrackbar("Brightness", windowName, &brightnessValue, 200,
                       FrameProcessor::onBrightnessTrackbar, &frameProcessor);
    cv::setMouseCallback(windowName, MouseHandler::callback, &mouseHandler);

    std::cout << "Controls:\n"
              << "  0 Normal\n"
              << "  1 Invert colors\n"
              << "  2 Gaussian blur\n"
              << "  3 Canny edge detector\n"
              << "  4 Sobel filter\n"
              << "  5 Binary threshold\n"
              << "  6 Quantization\n"
              << "  7 RGB glitch\n"
              << "  8 Picture in picture\n"
              << "  9 Draw rectangles with mouse\n"
              << "  A/D rotate, +/- zoom, mouse wheel zoom, I/J/K/L move frame, R reset\n"
              << "  Q or ESC quit\n";

    while (true) {
        cv::Mat frame = camera.getFrame();
        if (frame.empty()) {
            std::cerr << "Warning: empty frame received.\n";
            break;
        }

        int wheelDelta = mouseHandler.consumeWheelDelta();
        if (wheelDelta > 0) {
            keyProcessor.increaseZoom(0.1);
        } else if (wheelDelta < 0) {
            keyProcessor.decreaseZoom(0.1);
        }

        cv::Mat processed = frameProcessor.process(frame, keyProcessor.getMode(), keyProcessor, mouseHandler);
        display.show(processed);

        int key = cv::waitKey(1);
        if (!keyProcessor.processKey(key)) {
            break;
        }
    }

    cv::destroyAllWindows();
    return 0;
}
