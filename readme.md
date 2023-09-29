# Matrix-Viewer ![MatrixViewer](icons/matrix-viewer/48x48/matrix-viewer.png)
Frontend to [OpenCV](http://opencv.org/) matrices for visualization and edition as tabular or image data.

[![Build Status](https://travis-ci.org/crep4ever/matrix-viewer.svg?branch=master)](https://travis-ci.org/crep4ever/matrix-viewer)
[![Coverity Scan Build Status](https://img.shields.io/coverity/scan/4241.svg)](https://scan.coverity.com/projects/crep4ever-matrix-viewer)

![MatrixViewer](doc/img/main.png)

# Build and Run

* required packages: cmake; qt5 (core, widgets); libopencv-dev (2.4.11)
* optionnal packages: python-opencv, [Extra CMake Modules package (ECM)](https://api.kde.org/ecm/), [Exiv2](https://exiv2.org/)

## Example to install build dependencies on Ubuntu 22.04

```shell
# Build tools
sudo apt install build-essential cmake git

# Required dependencies
sudo apt install qt6-base-dev libopencv-dev

# Optional dependencies
sudo apt install extra-cmake-modules libexiv2-dev

# Documentation tools
sudo apt install doxygen

```

## Linux

```shell
git clone https://github.com/crep4ever/matrix-viewer.git
cd matrix-viewer
mkdir build && cd build
cmake .. && make
sudo make install
matrix-viewer
```

## Integration with gdb

* copy the script `misc/matrix-viewer-gdb.py` in a local directory
* from a gdb shell, run:
>     source /path/to/matrix-viewer-gdb.py
* the gdb command "mdv m" will launch matrix-viewer to open the cv::Mat 'm'

# Documentation

The developer documentation can be generated with [Doxygen](www.doxygen.org).

* required packages: doxygen

>     cd matrix-viewer
>     doxygen Doxyfile
>     <browser> doc/html/index.html

# Contact
* romain.goffe@gmail.com
