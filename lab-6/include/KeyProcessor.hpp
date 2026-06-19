#pragma once

#include <string>

class KeyProcessor {
public:
    enum class Mode {
        Normal,
        Invert,
        Blur,
        Canny,
        Sobel,
        Binary,
        Quantize,
        Glitch,
        Overlay,
        Draw
    };

    KeyProcessor();

    bool processKey(int key);

    Mode getMode() const;
    std::string modeName() const;

    int getOffsetX() const;
    int getOffsetY() const;
    double getRotationAngle() const;
    double getZoom() const;

    void increaseZoom(double step = 0.1);
    void decreaseZoom(double step = 0.1);

private:
    Mode mode_;
    int offsetX_;
    int offsetY_;
    double rotationAngle_;
    double zoom_;

    void resetTransform();
};
