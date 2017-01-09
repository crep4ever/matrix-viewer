# Matrix-Viewer ![MatrixViewer](icons/matrix-viewer/48x48/matrix-viewer.png)
Frontend to [OpenCV](http://opencv.org/) matrices for visualization and edition as tabular or image data.

[![Build Status](https://travis-ci.org/crep4ever/matrix-viewer.svg?branch=master)](https://travis-ci.org/crep4ever/matrix-viewer)
[![Coverity Scan Build Status](https://img.shields.io/coverity/scan/4241.svg)](https://scan.coverity.com/projects/crep4ever-matrix-viewer)

![MatrixViewer](doc/img/main.png)

# Build and Run

* required packages: cmake; qt5 (core, widgets); libopencv-dev (2.4.11)
* optionnal packages: python-opencv

## Linux

>     git clone git://github.com/crep4ever/matrix-viewer.git
>     cd matrix-viewer
>     mkdir build && cd build
>     cmake .. && make
>     sudo make install
>     matrix-viewer

## CMake options

* CMAKE_BUILD_TYPE [ `Release` (default), `Debug`]
* GENERATE_MANPAGES [`ON` (default), `OFF`]: requires asciidoc; xmlto;
* COMPRESS_MANPAGES [`ON` (default), `OFF`]: requires gzip;
* OpenCV_DIR <Path>: absolute path to custom OpenCV build directory (windows)

## Integration with gdb

* copy the script `misc/matrix-viewer-gdb.py` in a local directory
* from a gdb shell, run:
>     source /path/to/matrix-viewer-gdb.py
* the gdb command "mdv m" will launch matrix-viewer to open the cv::Mat 'm'

## Logs

When the application is run from the terminal, it may output some informative
logs. These logs can be formatted using the `QT_MESSAGE_PATTERN` environment
variable (see this [blog post](https://woboq.com/blog/nice-debug-output-with-qt.html) for more information).

Append the following line to your `~/.bashrc`.

>     export QT_MESSAGE_PATTERN="`echo -e "\033[32m%{time h:mm:ss.zzz}%{if-category}\033[32m %{category}:%{endif} %{if-debug}\033[36m%{backtrace depth=1}%{endif}%{if-warning}\033[31m%{backtrace depth=3}%{endif}%{if-critical}\033[31m%{backtrace depth=3}%{endif}%{if-fatal}\033[31m%{backtrace depth=3}%{endif}\033[0m %{message}"`"

## Qt4 compatibility
If your operating system only have Qt4 support, use the `qt4` branch of this repository.

>     git clone git://github.com/crep4ever/matrix-viewer.git
>     cd matrix-viewer
>     git checkout qt4
>     mkdir build && cd build
>     cmake .. && make
>     sudo make install
>     matrix-viewer

# Documentation

The developer documentation can be generated with [Doxygen](www.doxygen.org).

* required packages: doxygen

>     cd matrix-viewer
>     doxygen Doxyfile
>     <browser> doc/html/index.html

# Contact
* romain.goffe@gmail.com
