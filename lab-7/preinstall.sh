#!/usr/bin/env bash
set -e

# Works on Debian-based distributions, including Kali Linux and Ubuntu.
sudo apt update
sudo apt install -y build-essential cmake gcc g++ make libopencv-dev v4l-utils wget

# Download the ResNet-10 SSD face detector model (architecture + weights).
mkdir -p assets
wget -O assets/deploy.prototxt \
  https://raw.githubusercontent.com/opencv/opencv/master/samples/dnn/face_detector/deploy.prototxt
wget -O assets/res10_300x300_ssd_iter_140000.caffemodel \
  https://raw.githubusercontent.com/opencv/opencv_3rdparty/dnn_samples_face_detector_20170830/res10_300x300_ssd_iter_140000.caffemodel

echo "Dependencies and face detector model installed successfully."
