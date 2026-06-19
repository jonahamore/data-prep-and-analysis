#include "KeyProcessor.hpp"
#include <algorithm>

KeyProcessor::KeyProcessor()
    : mode_(Mode::Normal), offsetX_(0), offsetY_(0), rotationAngle_(0.0), zoom_(1.0) {}

bool KeyProcessor::processKey(int key) {
    if (key < 0) return true;

    int ascii = key & 0xFF;

    switch (ascii) {
        case 27:
        case 'q':
        case 'Q':
            return false;
        case '0': mode_ = Mode::Normal; break;
        case '1': mode_ = Mode::Invert; break;
        case '2': mode_ = Mode::Blur; break;
        case '3': mode_ = Mode::Canny; break;
        case '4': mode_ = Mode::Sobel; break;
        case '5': mode_ = Mode::Binary; break;
        case '6': mode_ = Mode::Quantize; break;
        case '7': mode_ = Mode::Glitch; break;
        case '8': mode_ = Mode::Overlay; break;
        case '9': mode_ = Mode::Draw; break;
        case 'r':
        case 'R':
            resetTransform();
            break;
        case '+':
        case '=':
            increaseZoom();
            break;
        case '-':
        case '_':
            decreaseZoom();
            break;
        case 'a':
        case 'A':
            rotationAngle_ -= 5.0;
            break;
        case 'd':
        case 'D':
            rotationAngle_ += 5.0;
            break;
        case 'i':
        case 'I':
            offsetY_ -= 10;
            break;
        case 'k':
        case 'K':
            offsetY_ += 10;
            break;
        case 'j':
        case 'J':
            offsetX_ -= 10;
            break;
        case 'l':
        case 'L':
            offsetX_ += 10;
            break;
        default:
            break;
    }

    return true;
}

KeyProcessor::Mode KeyProcessor::getMode() const { return mode_; }

std::string KeyProcessor::modeName() const {
    switch (mode_) {
        case Mode::Normal: return "Normal";
        case Mode::Invert: return "Invert";
        case Mode::Blur: return "Gaussian blur";
        case Mode::Canny: return "Canny";
        case Mode::Sobel: return "Sobel";
        case Mode::Binary: return "Binary";
        case Mode::Quantize: return "Quantize";
        case Mode::Glitch: return "RGB glitch";
        case Mode::Overlay: return "Picture in picture";
        case Mode::Draw: return "Draw rectangles";
        default: return "Unknown";
    }
}

int KeyProcessor::getOffsetX() const { return offsetX_; }
int KeyProcessor::getOffsetY() const { return offsetY_; }
double KeyProcessor::getRotationAngle() const { return rotationAngle_; }
double KeyProcessor::getZoom() const { return zoom_; }

void KeyProcessor::increaseZoom(double step) {
    zoom_ = std::min(3.0, zoom_ + step);
}

void KeyProcessor::decreaseZoom(double step) {
    zoom_ = std::max(0.3, zoom_ - step);
}

void KeyProcessor::resetTransform() {
    offsetX_ = 0;
    offsetY_ = 0;
    rotationAngle_ = 0.0;
    zoom_ = 1.0;
}
