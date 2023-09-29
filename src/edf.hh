// Copyright (C) 2022, Romain Goffe <romain.goffe@gmail.com>
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

#include "metadata.hh"

#include <QString>
#include <opencv2/opencv.hpp>

class QFile;
class QTextStream;

/**
 * @class CEdfFile
 * @brief File format for EDF images
 *
 * Qt/OpenCV implementation for The ESRF (european synchrotron) image format following the specification available at:
 * https://www.esrf.fr/home/UsersAndScience/Experiments/CBS/ID02/available_software/saxs-program-package/implementation.html
 */
class CEdfFile
{
public:
    CEdfFile(const QString& p_filePath = QString());
    ~CEdfFile();

    cv::Mat data() const;
    void setData(const cv::Mat& p_mat);

    const CMetadata& metadata() const;
    void setMetadata(const CMetadata& p_md);

    bool write(const QString& p_path);

    bool read(const QString& p_path);

    bool isHeaderLine(const QString& p_line) const;
    bool isBeginHeaderLine(const QString& p_line) const;
    bool isEndHeaderLine(const QString& p_line) const;

    bool save(bool p_createFile = false);

    QString path() const;

    CProperty parseHeaderLine(const QString& p_line, const QString& p_startMarker = "#", const QString& p_endMarker = ";") const;

protected:
    void clear();

    bool loadData();
    bool loadHeader();

private:
    bool checkRequiredHeaderProperties();

    unsigned int headerSize() const;
    unsigned int readBinarySize() const;

    unsigned int numericValue(const QString& p_property) const;
    unsigned int readRowCount() const;
    unsigned int readColumnCount() const;

    int readDataType() const;

    bool saveHeader();
    bool saveMatrix();

    void updateHeader();
    QString matrixTypeToDataType(const int p_type) const;

    QTextStream* stream() const;

    bool initStream();

    void closeStream();

    bool isStreamInitialized() const;

private:
    cv::Mat m_data;
    CMetadata m_metadata;

    qint64 m_headerStart;
    qint64 m_headerStop;

    QFile* m_file;
    QTextStream* m_stream;
};
