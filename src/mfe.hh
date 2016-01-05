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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <string.h>
#include <sstream>

#include <QString>
#include <QDebug>

#pragma pack(1)
struct MFEHeader
{
  MFEHeader() :
    format(),
    offset(0),
    type(0),
    cols(0),
    rows(0),
    depth(0)
  {
    format[0] = 'n';
    format[1] = '/';
    format[2] = 'a';
  }

  ~MFEHeader()
  {}

  char format[3];
  int32_t offset;
  int32_t type;
  int32_t cols;
  int32_t rows;
  int32_t depth;
};

class MFE
{

public:

  MFE() :
    m_header(),
    m_comment(""),
    m_data()
  {}

  ~MFE()
  {}

  void setData(const cv::Mat & p_mat)
  {
    if (!p_mat.isContinuous())
      {
        qWarning() << "OpenCV matrix must be continuous";
        return;
      }

    m_data = p_mat;

    m_header.format[0] = 'M';
    m_header.format[1] = 'F';
    m_header.format[2] = 'E';
    m_header.offset = (int32_t) m_comment.size() + (int32_t) sizeof(MFEHeader);
    m_header.type  = p_mat.type();
    m_header.cols  = p_mat.cols;
    m_header.rows  = p_mat.rows;
    m_header.depth = p_mat.channels();
  }

  cv::Mat data() const
  {
    return m_data;
  }

  void setComment(const std::string & p_str)
  {
    m_comment = p_str;
    m_header.offset = (int32_t) m_comment.size() + (int32_t) sizeof(MFEHeader);
  }

  std::string comment() const
  {
    return m_comment;
  }

  void write(const QString & p_path)
  {
    std::ofstream stream;
    stream.open(p_path.toStdString().c_str(),
                std::ofstream::out | std::ofstream::binary);

    // header
    stream.write(reinterpret_cast<char*>(&m_header),
                 sizeof(MFEHeader));

    // comment
    stream.write(m_comment.c_str(),
                 m_comment.size()); // skip trailing '\0'

    // data
    stream.write(reinterpret_cast<char*>(m_data.data),
                 m_data.rows * m_data.cols * m_data.elemSize());

    stream.flush();
    stream.close();
  }

  void read(const QString & p_path)
  {
    std::ifstream stream;
    stream.open(p_path.toStdString().c_str(),
                std::ifstream::in | std::ifstream::binary);

    // header
    int32_t size = (int32_t) sizeof(MFEHeader);
    stream.read(reinterpret_cast<char*>(&m_header), size);

    // comment
    size = (int32_t) m_header.offset - (int32_t) sizeof(MFEHeader);
    std::vector<char> comment(size + 1);
    stream.read(comment.data(), size);
    comment[size] = '\0';
    setComment(std::string(comment.begin(), comment.end()));

    // data
    m_data.create(m_header.rows, m_header.cols, m_header.type);
    size = (int32_t) m_header.rows * (int32_t) m_header.cols * (int32_t) m_data.elemSize();
    stream.read(reinterpret_cast<char*>(m_data.data), size);

    stream.close();
  }

private:
  MFEHeader m_header;
  std::string m_comment;
  cv::Mat m_data;
};

