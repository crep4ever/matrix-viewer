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

#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <string.h>
#include <sstream>

#include <QString>
#include <QDataStream>
#include <QDebug>


/**
 * @class MatrixFormatExchange
 * @brief Matrix Format Exchange for Matlab/C++ data
 *
 * The class MatrixFormatExchange reads and write cv::Mat in a binary format
 * that is compatible with c++ and matlab.
 *
 * A MFE file has the following content:
 *
 *  \li header  (fixed size of 23bytes @see MFEHeader)
 *  \li comment (string of variable length)
 *  \li data    (matrix values)
 *
 * Example:
 *
 * \code
 *   // create matrix
 *   cv::Mat m(3, 2, CV_64FC1);
 *   cv::randu(m, cv::Scalar::all(0), cv::Scalar::all(255));
 *
 *   // write matrix in MFE format
 *   MatrixFormatExchange mfe;
 *   mfe.SetData(m);
 *   mfe.SetComment("hello world");
 *   mfe.Write("/tmp/matrix.mfe");
 *
 *   // read matrix in MFE format
 *   MatrixFormatExchange mfe;
 *   mfe.Read("/tmp/matrix.mfe");
 *   cv::Mat n = mfe.GetData();
 *   std::cout << "comment: " << mfe.GetComment() << std::endl;
 *
 *   // Compare matrix
 *   cv::Mat diff;
 *   cv::absdiff(m, n, diff);
 *   assert(cv::countNonZero(diff) == 0);
 *
 * \endcode
 *
 */
class MatrixFormatExchange
{
private:
    struct MFEHeader
    {
        MFEHeader():
            format(std::vector<char>(3)),
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

        ~MFEHeader() // explicitly non-virtual
        {
        }

        std::string toString() const
        {
            std::stringstream stream;
            stream << "format: "  << format[0] << format[1] << format[2] << std::endl;
            stream << "offset: "  << offset << std::endl;
            stream << "type: "    << type << std::endl;
            stream << "cols: "    << cols << std::endl;
            stream << "rows: "    << rows << std::endl;
            stream << "depth: "   << depth << std::endl;
            return stream.str();
        }

        /**
         * @brief Return the size in bytes of this structure
         *
         * The size returned by this method is different from sizeof(MFEHeader)
         *
         * Indeed, the structure MFEHeader contains 3 char + 5 int
         * hence a theoretical size of 3*1 + 5*4 = 23 bytes
         *
         * since integers are aligned on 4 bytes, by default :
         * sizeof(MFEHeader) = 24
         *
         * using the pragma '#pragma pack(1)' returns the expected value :
         * sizeof(MFEHeader) = 23
         *
         * however, this pragma entail portability issues on windows with MVSCC
         */
         int size() const
         {
             const int s = (int)format.size() +
             sizeof(offset) +
             sizeof(type) +
             sizeof(cols) +
             sizeof(rows) +
             sizeof(depth);

             Q_ASSERT(s == 23);
             return s;
         }

        bool read(QDataStream& p_stream)
        {
            if (p_stream.readRawData(&format[0], 3) == -1)
            {
                qWarning() << "Can't read MFE header offset";
                return false;
            }

            if (p_stream.readRawData(reinterpret_cast<char*>(&offset), sizeof(offset)) == -1)
            {
                qWarning() << "Can't read MFE header offset";
                return false;
            }

            if (p_stream.readRawData(reinterpret_cast<char*>(&type), sizeof(type)) == -1)
            {
                qWarning() << "Can't read MFE header data type";
                return false;
            }

            if (p_stream.readRawData(reinterpret_cast<char*>(&cols), sizeof(cols)) == -1)
            {
                qWarning() << "Can't read MFE header nb columns";
                return false;
            }

            if (p_stream.readRawData(reinterpret_cast<char*>(&rows), sizeof(rows)) == -1)
            {
                qWarning() << "Can't read MFE header nb rows";
                return false;
            }

            if (p_stream.readRawData(reinterpret_cast<char*>(&depth), sizeof(depth)) == -1)
            {
                qWarning() << "Can't read MFE header nb channels";
                return false;
            }

            return true;
        }
        bool write(QDataStream& p_stream)
        {
            if (p_stream.writeRawData(&format[0],3) == -1)
            {
                qWarning() << "Can't write MFE header format";
                return false;
            }

            if (p_stream.writeRawData(reinterpret_cast<char*>(&offset), sizeof(offset)) == -1)
            {
                qWarning() << "Can't write MFE header offset";
                return false;
            }

            if (p_stream.writeRawData(reinterpret_cast<char*>(&type), sizeof(type)) == -1)
            {
                qWarning() << "Can't write MFE header data type";
                return false;
            }

            if (p_stream.writeRawData(reinterpret_cast<char*>(&cols), sizeof(cols)) == -1)
            {
                qWarning() << "Can't write MFE header nb columns";
                return false;
            }

            if (p_stream.writeRawData(reinterpret_cast<char*>(&rows), sizeof(rows)) == -1)
            {
                qWarning() << "Can't write MFE header nb rows";
                return false;
            }

            if (p_stream.writeRawData(reinterpret_cast<char*>(&depth), sizeof(depth)) == -1)
            {
                qWarning() << "Can't write MFE header nb channels";
                return false;
            }

            return true;
        }

        std::vector<char> format;
        uint32_t offset;
        uint32_t type;
        uint32_t cols;
        uint32_t rows;
        uint32_t depth;
    };

public:

    MatrixFormatExchange();

    ~MatrixFormatExchange();

    cv::Mat data() const;

    void setData(const cv::Mat & p_mat);

    std::string comment() const;

    void setComment(const std::string & p_str);

    std::string toString() const;

    bool write(const QString & p_path);

    bool read(const QString & p_path);

private:
    MFEHeader m_header;
    std::string m_comment;
    cv::Mat m_data;
};
