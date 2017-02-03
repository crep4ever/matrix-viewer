// Copyright (C) 2015, Romain Goffe <romain.goffe@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.
//******************************************************************************

#pragma once

#include <QDebug>
#include <QString>
#include <opencv2/opencv.hpp>


QDebug operator<<(QDebug stream, const cv::Exception & e)
{
  QString info = QString("\nOpenCV Error\n\n"
			 " CODE: %1\n"
			 " FUNC: %2\n"
			 " FILE: %3\n"
			 " LINE: %4\n"
			 " INFO: %5\n")
    .arg(QString::number(e.code))
    .arg(QString::fromStdString(e.func))
    .arg(QString::fromStdString(e.file))
    .arg(QString::number(e.line))
    .arg(QString::fromStdString(e.err));

  stream << info;
  return stream;
}
