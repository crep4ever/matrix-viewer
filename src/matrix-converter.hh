// Copyright (C) 2013, Romain Goffe <romain.goffe@gmail.com>
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

#ifndef __MATRIX_CONVERTER_HH__
#define __MATRIX_CONVERTER_HH__

#include <QObject>
#include <opencv2/opencv.hpp>

/*!
  \file matrix-converter.hh
  \class CMatrixConverter
  \brief CMatrixConverter is a helper that may convert
  different matrix data files in OpenCV matrix
*/

class CMatrixConverter : public QObject
{
  Q_OBJECT
  Q_ENUMS(FileFormat)

public:

  enum FileFormat
  {
    Format_Unknown,
    Format_Bmp,
    Format_Txt,
    Format_Xml
  };

  /// Constructor.
  CMatrixConverter();

  /// Constructor from a filename
  CMatrixConverter(const QString & filename);

  /// Destructor.
  virtual ~CMatrixConverter();

  cv::Mat data() const;
  void setData(const cv::Mat & matrix);

  void print() const;

  bool save(const QString & filename);
  bool load(const QString & filename);

  bool isFormatData() const;
  bool isFormatImage() const;

private:
  bool loadFromTxt(const QString & filename);
  bool saveToTxt(const QString & filename);

  bool loadFromXml(const QString & filename);
  bool saveToXml(const QString & filename);

  bool loadFromBmp(const QString & filename);
  bool saveToBmp(const QString & filename);

  cv::Mat m_data;
  FileFormat m_format;
};

#endif  // __MATRIX_CONVERTER_HH__
