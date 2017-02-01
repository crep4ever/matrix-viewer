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
  \brief CMatrixConverter converts different file formats in OpenCV matrices

  ### Common image files

  All image formats that are supported by the `cv::imread` function
  are available. Those formats include bmp, jpg, png ...

  ### XML files

  [OpenCV XML persistance](http://docs.opencv.org/2.4/modules/core/doc/xml_yaml_persistence.html)

  ### MFE files

  Custom serialization in MFE binary format.

  ### TXT files

  Custom serialization in plain text format.
  This format stores width / height information on the first line and values
  on the second.
*/
class CMatrixConverter : public QObject
{
  Q_OBJECT
  Q_ENUMS(FileFormat)

public:

  enum FileFormat
  {
    Format_Unknown,
    Format_Txt,
    Format_Xml,
    Format_Bmp,
    Format_Jpg,
    Format_Png,
    Format_Raw,
    Format_Mfe,
    Format_Webp
  };

  /// Constructor.
  CMatrixConverter();

  /// Constructor from a filename
  CMatrixConverter(const QString & filename);

  /// Destructor.
  virtual ~CMatrixConverter();

  cv::Mat data() const;
  void setData(const cv::Mat & matrix);

  bool save(const QString & filename);
  bool load(const QString & filename);

  FileFormat format() const;

  void readSettings();

  void setRawWidth(const int p_value);
  void setRawHeight(const int p_value);
  void setRawType(const int p_value);


private:
  bool loadFromTxt(const QString & filename);
  bool saveToTxt(const QString & filename);

  bool loadFromXml(const QString & filename);
  bool saveToXml(const QString & filename);

  bool loadFromImage(const QString & filename);
  bool saveToImage(const QString & filename);

  bool loadFromRaw(const QString & filename);
  bool saveToRaw(const QString & filename);

  bool loadFromMfe(const QString & filename);
  bool saveToMfe(const QString & filename);

  cv::Mat m_data;
  FileFormat m_format;

  int m_rawType;
  int m_rawWidth;
  int m_rawHeight;
};

#endif  // __MATRIX_CONVERTER_HH__
