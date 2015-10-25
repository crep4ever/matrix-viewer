# Description
This is a graphical frontend in Qt4/C++ to OpenCV matrices format

# Screenshot
![MatrixViewer](http://www.patacrep.com/data/images/matrix-viewer.png)

# Matrix-viewer
* required packages: cmake, libqt4-dev, libopencv-dev
* optionnal packages: python-opencv

* build and run:

>     git clone git://github.com/crep4ever/matrix-viewer.git
>     cd matrix-viewer
>     mkdir build && cd build
>     cmake -DCMAKE_BUILD_TYPE=Release .. && make
>     sudo make install
>     matrix-viewer

# Integration with gdb
* copy the script misc/matrix-viewer-gdb.py in a local directory
* from a gdb shell, run: "source /path/to/matrix-viewer-gdb.py"
* the gdb command "mdv m" will launch matrix-viewer to open the cv::Mat 'm'

# Contact
* romain.goffe@gmail.com
