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

#include "mfe.hh"
#include <QDebug>

MatrixFormatExchange::MatrixFormatExchange()
: m_header()
, m_comment("")
, m_data()
{}


MatrixFormatExchange::~MatrixFormatExchange()
{}

cv::Mat MatrixFormatExchange::data() const
{
    return m_data;
}

void MatrixFormatExchange::setData(const cv::Mat & p_mat)
{
    if (!p_mat.empty())
    {
        Q_ASSERT(p_mat.isContinuous());
    }
    m_data = p_mat;

    m_header.format[0] = 'M';
    m_header.format[1] = 'F';
    m_header.format[2] = 'E';
    m_header.offset = (uint32_t)m_comment.size() + (uint32_t)m_header.size();
    m_header.type  = p_mat.type();
    m_header.cols  = p_mat.cols;
    m_header.rows  = p_mat.rows;
    m_header.depth = p_mat.channels();
}

std::string MatrixFormatExchange::comment() const
{
    return m_comment;
}

void MatrixFormatExchange::setComment(const std::string & p_str)
{
    m_comment = p_str;
    m_header.offset = (uint32_t)m_comment.size() + (uint32_t)m_header.size();
}

std::string MatrixFormatExchange::toString() const
{
    std::stringstream stream;
    stream << "Matrix Format Exchange:" << std::endl;
    stream << "\tHeader:" << m_header.toString();
    return stream.str();
}

bool MatrixFormatExchange::write(const QString & p_path)
{
    std::ofstream stream;
    stream.open(p_path.toStdString().c_str(), std::ofstream::out | std::ofstream::binary);

    if (!stream.is_open())
    {
        qWarning() << "Can't write in file:" << p_path;
        return false;
    }

    // header
    m_header.write(stream);

    // comment
    stream.write(m_comment.c_str(), m_comment.size()); // skip the '\0' character

    // data
    stream.write(reinterpret_cast<char*>(m_data.data), m_data.rows * m_data.cols * m_data.elemSize());

    stream.flush();

    stream.close();

    return true;
}

bool MatrixFormatExchange::read(const QString & p_path)
{
    std::ifstream stream;
    stream.open(p_path.toStdString().c_str(), std::ifstream::in | std::ifstream::binary);

    if (!stream.is_open())
    {
        qWarning() << "Can't read from file:" << p_path;
        return false;
    }

    // header
    int size = m_header.size();
    m_header.read(stream);
    
    // comment
    size = m_header.offset - m_header.size();
    std::vector<char> info(size + 1);
    stream.read(info.data(), size);
    info[size] = '\0';
    setComment(std::string(info.begin(), info.end()));

    // data
    m_data.create(m_header.rows, m_header.cols, m_header.type);
    size = m_header.rows * m_header.cols * (uint32_t)m_data.elemSize();
    stream.read(reinterpret_cast<char*>(m_data.data), size);

    stream.close();

    return true;
}
