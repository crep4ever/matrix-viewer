#Filename: matrix-viewer-gdb.py
#Date: Sun, 28 Jul 2013
#Author: Romain Goffe <romain.goffe@gmail.com>
#Description: Enable OpenCV matrix to be opened with
# the matrix-viewer application directly from gdb
#
# From a gdb shell, run:
#  source /path/to/matrix-viewer.py
#  mdv matrix
#
# where matrix is a cv::Mat instance
#
# This script requires the package python-opencv


import gdb
import cv2.cv as cv
import sys
from subprocess import call

class MatrixViewerCommand(gdb.Command):
    def __init__(self):
        super(MatrixViewerCommand, self).__init__("mdv",
                                                  gdb.COMMAND_DATA,
                                                  gdb.COMPLETE_SYMBOL)
    def invoke(self, arg, from_tty):
        args = gdb.string_to_argv(arg)

        v = gdb.parse_and_eval(args[0])
        strType = gdb.execute("print "+ args[0] + ".type()", False, True)
        # strType contains gdb answers as a string of the form "$2 = 42"
        img = cv.CreateMat(v['rows'], v['cols'], int(strType.split(" ")[2]))

        # convert v['data'] to char*
        char_type = gdb.lookup_type("char")
        char_pointer_type = char_type.pointer()
        buffer = v['data'].cast(char_pointer_type)

        # read bytes from inferior's process memory
        buf = v['step']['buf']
        bytes = buf[0] * v['rows']
        inferior = gdb.selected_inferior()
        mem = inferior.read_memory(buffer, bytes)

        # set the matrix raw data
        cv.SetData(img, mem)

        # save matrix as an xml file and open it with matrix viewer
        cv.Save("/tmp/dump.xml", img, "matrix")
        call(["matrix-viewer", "/tmp/dump.xml"])

MatrixViewerCommand()
