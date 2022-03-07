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

#include <QFileInfo>
#include <QString>
#include <opencv2/opencv.hpp>

class QFile;
class QTextStream;

class CProperty
{
public:
    const QString& key() const
    {
        return m_key;
    }

    void setKey(const QString& p_key)
    {
        m_key = p_key;
    }

    const QString& value() const
    {
        return m_value;
    }

    void setValue(const QString& p_value)
    {
        m_value = p_value;
    }

private:
    QString m_key;
    QString m_value;
};

class CMetadata : public QFileInfo
{
public:
    const QVector<CProperty>& properties() const
    {
        return m_properties;
    }

    void addProperty(const CProperty& p_property)
    {
        m_properties << p_property;
    }

    void updateProperty(const QString& p_key, const QString& p_value)
    {
        for (CProperty& property : m_properties)
        {
            if (p_key == property.key())
            {
                property.setValue(p_value);
                break;
            }
        }
    }

    QString value(const QString& p_key) const
    {
        for (const CProperty& property : properties())
        {
            if (p_key == property.key())
            {
                return property.value();
            }
        }
        return QString();
    }

    void clear()
    {
        m_properties.clear();
    }

private:
    QVector<CProperty> m_properties;
};

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

    bool write(const QString& p_path);

    bool read(const QString& p_path);

    bool isHeaderLine(const QString& p_line) const;
    bool isBeginHeaderLine(const QString& p_line) const;
    bool isEndHeaderLine(const QString& p_line) const;

    bool save(bool p_creatFile = false);

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

    const CMetadata& info() const;

private:
    qint64 m_headerStart;
    qint64 m_headerStop;

    cv::Mat m_data;

    CMetadata m_info;
    QFile* m_file;
    QTextStream* m_stream;
};
