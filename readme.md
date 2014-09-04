# Description
This is a Qt4/C++ client to visualize and edit matrix data from OpenCV serialization

# Screenshot
![MatrixViewer](http://www.patacrep.com/data/images/matrix-viewer.png)

# Matrix-viewer
* required packages: cmake, libqt4-dev, libopencv-dev
* optionnal packages: pyhton-opencv

* build and run:

>     git clone git://github.com/crep4ever/matrix-viewer.git
>     cd matrix-viewer
>     make && sudo make install
>     matrix-viewer

# Integration with gdb
* copy the script misc/matrix-viewer-gdb.py in a local directory
* from a gdb shell, run: "source /path/to/matrix-viewer-gdb.py"
* the gdb command "mdv m" will launch matrix-viewer to open the cv::Mat 'm'

# Contact
* romain.goffe@gmail.com
