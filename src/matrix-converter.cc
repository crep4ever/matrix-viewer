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
#include "matrix-converter.hh"

#include "config.hh"
#include "edf.hh"
#include "mfe.hh"

#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QStringList>
#include <QTextStream>

#if defined(LIBEXIV2_ENABLED)
#    include <exiv2/exiv2.hpp>
#endif

const QStringList CMatrixConverter::s_imageExtensions = QStringList() << "bmp"
                                                                      << "jpg"
                                                                      << "jpeg"
                                                                      << "jpe"
                                                                      << "png"
                                                                      << "tif"
                                                                      << "tiff"
                                                                      << "webp";

const QStringList CMatrixConverter::s_fileStorageExtensions = QStringList() << "xml"
                                                                            << "yaml"
                                                                            << "json";

CMatrixConverter::CMatrixConverter() : QObject(), m_format(Format_Unknown), m_data(), m_metadata(), m_rawType(0), m_rawWidth(0), m_rawHeight(0) { }

CMatrixConverter::CMatrixConverter(const QString& p_filename)
    : QObject()
    , m_format(Format_Unknown)
    , m_data()
    , m_metadata()
    , m_rawType(0)
    , m_rawWidth(0)
    , m_rawHeight(0)
{
    if (!load(p_filename))
    {
        qWarning() << "Can't load file: " << p_filename;
    }
}

CMatrixConverter::~CMatrixConverter() { }

void CMatrixConverter::readSettings()
{
    QSettings settings;
    settings.beginGroup("image");
    m_rawType   = settings.value("raw-type", 0).toInt();
    m_rawWidth  = settings.value("raw-width", 2160).toInt();
    m_rawHeight = settings.value("raw-height", 1944).toInt();
    settings.endGroup();
}

void CMatrixConverter::setRawWidth(const int p_value)
{
    m_rawWidth = p_value;
}

CMatrixConverter::FileFormat CMatrixConverter::format() const
{
    return m_format;
}

void CMatrixConverter::setRawHeight(const int p_value)
{
    m_rawHeight = p_value;
}

void CMatrixConverter::setRawType(const int p_value)
{
    m_rawType = p_value;
}

bool CMatrixConverter::load(const QString& p_filename)
{
    const QString suffix = QFileInfo(p_filename).suffix().toLower();

    if (s_fileStorageExtensions.contains(suffix))
    {
        m_format = Format_FileStorage;
        return loadFromFileStorage(p_filename);
    }

    if (s_imageExtensions.contains(suffix))
    {
        m_format = Format_Image;
        return loadFromImage(p_filename);
    }

    if (p_filename.endsWith(".txt", Qt::CaseInsensitive))
    {
        m_format = Format_Txt;
        return loadFromTxt(p_filename);
    }

    if (p_filename.endsWith(".raw", Qt::CaseInsensitive))
    {
        m_format = Format_Raw;
        return loadFromRaw(p_filename);
    }

    if (p_filename.endsWith(".mfe", Qt::CaseInsensitive))
    {
        m_format = Format_Mfe;
        return loadFromMfe(p_filename);
    }

    if (p_filename.endsWith(".edf", Qt::CaseInsensitive))
    {
        m_format = Format_Edf;
        return loadFromEdf(p_filename);
    }

    m_format = Format_Unknown;
    return loadFromImage(p_filename);
}

bool CMatrixConverter::save(const QString& p_filename)
{
    const QString suffix = QFileInfo(p_filename).suffix().toLower();

    if (s_fileStorageExtensions.contains(suffix))
    {
        m_format = Format_FileStorage;
        return saveToFileStorage(p_filename);
    }

    if (s_imageExtensions.contains(suffix))
    {
        m_format = Format_Image;
        return saveToImage(p_filename);
    }

    if (suffix == "txt")
    {
        m_format = Format_Txt;
        return saveToTxt(p_filename);
    }

    if (suffix == "raw")
    {
        m_format = Format_Raw;
        return saveToRaw(p_filename);
    }

    if (suffix == "mfe")
    {
        m_format = Format_Mfe;
        return saveToMfe(p_filename);
    }

    if (suffix == "edf")
    {
        m_format = Format_Edf;
        return saveToEdf(p_filename);
    }

    return saveToImage(p_filename);
}

cv::Mat CMatrixConverter::data() const
{
    return m_data;
}

void CMatrixConverter::setData(const cv::Mat& p_matrix)
{
    m_data = p_matrix;
}

const CMetadata& CMatrixConverter::metadata()
{
    return m_metadata;
}

bool CMatrixConverter::loadFromTxt(const QString& p_filename)
{
    QFile file(p_filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);

        // first line contains the number of columns and rows of the matrix
        QStringList values = stream.readLine().split(" ");
        if (values.size() != 2)
        {
            qWarning() << QString("CMatrixConverter::loadFromTxt first line of file [%1] should contain COL ROW information but has [%2]")
                              .arg(p_filename)
                              .arg(values.join(" "));
            return false;
        }

        const uint rows = values[1].toInt(), cols = values[0].toInt();
        if (rows == 0 || cols == 0)
        {
            qWarning() << "CMatrixConverter::loadFromTxt invalid dimensions for matrix:" << p_filename;
            return false;
        }

        m_data.create(rows, cols, CV_64FC1);

        // second line contains matrix values
        values = stream.readLine().split(" ");

        for (uint v = 0; v < rows * cols; ++v)
        {
            int row                     = v / cols;
            int col                     = v % cols;
            m_data.at<double>(row, col) = values[v].toDouble();
        }

        file.close();

        return true;
    }

    qWarning() << "CMatrixConverter::loadFromTxt unable to open:" << p_filename;
    return false;
}

bool CMatrixConverter::saveToTxt(const QString& p_filename)
{
    m_data.convertTo(m_data, CV_64FC1);

    QFile file(p_filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << m_data.cols << " " << m_data.rows << "\n";

        for (int j = 0; j < m_data.rows; ++j)
        {
            for (int i = 0; i < m_data.cols; ++i)
            {
                stream << m_data.at<double>(j, i) << " ";
            }
        }

        file.close();
        return true;
    }

    qWarning() << "CMatrixConverter::saveToTxt unable to open:" << p_filename;
    return false;
}

bool CMatrixConverter::loadFromFileStorage(const QString& p_filename)
{
    try
    {
        cv::FileStorage fs;
        if (!fs.open(p_filename.toStdString(), cv::FileStorage::READ))
        {
            return false;
        }

        fs["matrix"] >> m_data;
        fs.release();
    }
    catch (cv::Exception& e)
    {
        qWarning() << "CMatrixConverter::loadFromXml invalid matrix:" << p_filename;
        return false;
    }

    return true;
}

bool CMatrixConverter::saveToFileStorage(const QString& p_filename)
{
    cv::FileStorage fs;
    if (!fs.open(p_filename.toStdString(), cv::FileStorage::WRITE))
    {
        return false;
    }

    fs << "matrix" << m_data;
    fs.release();

    return true;
}

bool CMatrixConverter::loadFromImage(const QString& p_filename)
{
    try
    {
        m_data = cv::imread(p_filename.toStdString(), -1);
        if (m_data.type() == CV_8UC4)
        {
            cv::cvtColor(m_data, m_data, cv::COLOR_BGRA2BGR); // remove alpha channel
        }
    }
    catch (cv::Exception& e)
    {
        qWarning() << "CMatrixConverter::loadFromImage invalid matrix:" << p_filename;
        qWarning() << " -- OpenCV error:" << e.what();
        return false;
    }

#if defined(LIBEXIV2_ENABLED)
    try
    {
        auto image = Exiv2::ImageFactory::open(p_filename.toStdString());
        image->readMetadata();

        const Exiv2::ExifData& exifData = image->exifData();
        if (exifData.empty())
        {
            qDebug() << "No exif metadata found for file:" << p_filename;
        }

        for (auto it = exifData.begin(); it != exifData.end(); ++it)
        {
            QString key         = QString::fromStdString(it->key()).simplified();
            const QString value = QString::fromStdString(it->value().toString()).simplified();

            // - Ignore keys 'tags' (eg: skip Exif.Image.0x9002)
            if (key.isEmpty() || key.contains("0x") || value.isEmpty())
            {
                continue;
            }

            m_metadata.addProperty(CProperty(key, value));
        }
    }
    catch (std::exception& e)
    {
        qDebug() << "Can't read exif metadata for file:" << p_filename;
        qDebug() << "-- error:" << e.what();
    }
    catch (...)
    {
        qDebug() << "Can't read exif metadata for file:" << p_filename;
    }
#endif

    return true;
}

bool CMatrixConverter::saveToImage(const QString& p_filename)
{
    bool ret;
    try
    {
        const QString suffix = QFileInfo(p_filename).suffix().toLower();

        std::vector<int> compressionParams;
        if (suffix == "png")
        {
            compressionParams.push_back(cv::IMWRITE_PNG_STRATEGY);
            compressionParams.push_back(cv::IMWRITE_PNG_STRATEGY_DEFAULT);
            compressionParams.push_back(cv::IMWRITE_PNG_COMPRESSION);
            compressionParams.push_back(9);
        }
        else if (suffix == "jpg" || suffix == "jpeg" || suffix == "jpe")
        {
            compressionParams.push_back(cv::IMWRITE_JPEG_QUALITY);
            compressionParams.push_back(80);
        }

        ret = cv::imwrite(p_filename.toStdString(), m_data, compressionParams);
    }
    catch (cv::Exception& e)
    {
        qWarning() << "CMatrixConverter::saveToImage invalid matrix:" << p_filename;
        qWarning() << "OpenCV error:" << QString::fromStdString(e.msg);
        return false;
    }
    return ret;
}

bool CMatrixConverter::loadFromRaw(const QString& p_filename)
{
    if (m_rawType == 0)
    {
        unsigned short* buffer = new unsigned short[m_rawWidth * m_rawHeight];
        FILE* fd               = fopen(p_filename.toStdString().c_str(), "rb");
        if (fd != nullptr)
        {
            size_t readBytes = fread(buffer, sizeof(unsigned short), m_rawWidth * m_rawHeight, fd);
            fclose(fd);

            if (readBytes == 0)
            {
                qWarning() << "Nothing to read from raw image file:" << p_filename;
                delete[] buffer;
                return false;
            }
        }
        else
        {
            qWarning() << "Can't open raw image file in read mode:" << p_filename;
            delete[] buffer;
            return false;
        }

        cv::Mat rawData(cv::Size(m_rawWidth, m_rawHeight), CV_16U, buffer);

        const double ratio = 127.5 / 511.5; // conversion from 10bits to 8 bits
        m_data             = rawData * ratio;
        m_data.convertTo(m_data, CV_8UC1);
        delete[] buffer;
    }
    else
    {
        qWarning() << "CMatrixConverter::loadFromRaw format not supported yet for type:" << m_rawType;
        return false;
    }

    return true;
}

bool CMatrixConverter::saveToRaw(const QString& p_filename)
{
    Q_UNUSED(p_filename);
    qWarning() << "CMatrixConverter::saveToRaw not implemented yet";
    return false;
}

bool CMatrixConverter::loadFromMfe(const QString& p_filename)
{
    try
    {
        MatrixFormatExchange mfe;
        mfe.read(p_filename);
        m_data = mfe.data();
        m_metadata.addProperty(CProperty("Comment", QString::fromStdString(mfe.comment())));
    }
    catch (cv::Exception& e)
    {
        qWarning() << "OpenCV error while reading MFE matrix";
        qWarning() << "file: " << p_filename;
        qWarning() << "error: " << e.what();
        return false;
    }

    return true;
}

bool CMatrixConverter::saveToMfe(const QString& p_filename)
{
    try
    {
        MatrixFormatExchange mfe;
        mfe.setData(m_data);
        mfe.setComment("MatrixViewer");
        mfe.write(p_filename);
    }
    catch (cv::Exception& e)
    {
        qWarning() << "OpenCV error while writing MFE matrix";
        qWarning() << "file: " << p_filename;
        qWarning() << "error: " << e.what();
        return false;
    }

    return true;
}

bool CMatrixConverter::loadFromEdf(const QString& p_filename)
{
    try
    {
        CEdfFile edf;
        edf.read(p_filename);
        m_data     = edf.data();
        m_metadata = edf.metadata();
    }
    catch (cv::Exception& e)
    {
        qWarning() << "OpenCV error while reading EDF matrix";
        qWarning() << "file: " << p_filename;
        qWarning() << "error: " << e.what();
        return false;
    }

    return true;
}

bool CMatrixConverter::saveToEdf(const QString& p_filename)
{
    try
    {
        CEdfFile edf;
        edf.setData(m_data);
        edf.setMetadata(m_metadata);
        edf.write(p_filename);
    }
    catch (cv::Exception& e)
    {
        qWarning() << "OpenCV error while writing EDF matrix";
        qWarning() << "file: " << p_filename;
        qWarning() << "error: " << e.what();
        return false;
    }

    return true;
}

bool CMatrixConverter::isFilenameSupported(const QString& p_filename)
{
    static const QStringList s_extensions = QStringList() << CMatrixConverter::s_fileStorageExtensions << CMatrixConverter::s_imageExtensions << "mfe"
                                                          << "edf"
                                                          << "txt"
                                                          << "raw";
    return s_extensions.contains(QFileInfo(p_filename).suffix().toLower());
}
