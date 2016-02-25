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

/**
 * @brief MFE format header
 *
 * The header contains information about the
 * the matrix data it precedes such as the matrix
 * size, type etc.
 *
 * The structure MFEHeader contains 3 char + 5 int
 * hence a theoretical size of 3*1 + 5*4 = 23 bytes
 *
 * since integers are aligned on 4 bytes, by default :
 * sizeof(MFEHeader) = 24
 *
 * using the pragma '#pragma pack(1)' returns the expected value :
 * sizeof(MFEHeader) = 23
 *
 * Note that this pragma entails portability issues on windows with MVSCC
 */
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

/**
 * @class MFE
 * @brief Matrix Format Exchange
 *
 * The class MatrixFormatExchange reads and write OpenCV's cv::Mat in a binary
 * format that ensures floating values accuracy, fast load/unload times
 * and reasonnable storage size.
 *
 * A MFE file has the following content:
 *
 *  \li a fixed-sized header of 23bytes (see MFEHeader)
 *  \li an optional comment (string of variable length)
 *  \li binary data (OpenCV matrix values)
 *
 * Example:
 *
 * \code
 *   // create matrix of random values
 *   cv::Mat m(3, 2, CV_64FC1);
 *   cv::randu(m, cv::Scalar::all(0), cv::Scalar::all(255));
 *
 *   // write matrix in MFE format
 *   MatrixFormatExchange mfe;
 *   mfe.setData(m);
 *   mfe.setComment("hello world");
 *   mfe.write("/tmp/matrix.mfe");
 *
 *   // read matrix in MFE format
 *   MatrixFormatExchange mfe;
 *   mfe.read("/tmp/matrix.mfe");
 *   cv::Mat n = mfe.data();
 *   std::cout << "comment: " << mfe.comment() << std::endl;
 *
 *   // verify that saved+reloaded matrix is similar to original matrix
 *   cv::Mat diff;
 *   cv::absdiff(m, n, diff);
 *   assert(cv::countNonZero(diff) == 0);
 *
 * \endcode
 *
 */
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
    std::vector<char> info(size + 1);
    stream.read(info.data(), size);
    info[size] = '\0';
    setComment(std::string(info.begin(), info.end()));

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
