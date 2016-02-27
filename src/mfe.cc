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
#include <QFile>
#include <QDataStream>

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
    QFile file(p_path);
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << "Can't  write in file:" << p_path;
        return false;
    }

    QDataStream stream(&file);

    // header
    m_header.write(stream);

    // comment
    if (stream.writeRawData(m_comment.c_str(), m_comment.size()) == -1) // skip the '\0' character
    {
        qWarning() << "Can't write MFE comment";
        return false;
    }

    // data
    if (stream.writeRawData(reinterpret_cast<char*>(m_data.data), m_data.rows * m_data.cols * m_data.elemSize()) == -1)
    {
        qWarning() << "Can't write MFE data";
        return false;
    }

    file.close(); // close automatically flush the file

    return true;
}

bool MatrixFormatExchange::read(const QString & p_path)
{
    QFile file(p_path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Can't read from file:" << p_path;
        return false;
    }

    QDataStream stream(&file);

    // header
    int size = m_header.size();
    if (!m_header.read(stream))
    {
        qWarning() << "Error decoding MFE header";
        return false;
    }

    // comment
    size = m_header.offset - m_header.size();
    std::vector<char> info(size + 1);

    if (stream.readRawData(info.data(), size) == -1)
    {
        qWarning() << "Error decoding MFE comment";
        return false;
    }
    info[size] = '\0';
    setComment(std::string(info.begin(), info.end()));

    // data
    m_data.create(m_header.rows, m_header.cols, m_header.type);
    size = m_header.rows * m_header.cols * (uint32_t)m_data.elemSize();
    if (stream.readRawData(reinterpret_cast<char*>(m_data.data), size) == -1)
    {
        qWarning() << "Error decoding MFE data";
        return false;
    }

    file.close();

    return true;
}
