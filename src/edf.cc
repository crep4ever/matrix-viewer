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

#include "edf.hh"

#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

CEdfFile::CEdfFile(const QString& p_filePath) : m_data(), m_metadata(), m_headerStart(-1), m_headerStop(-1), m_file(nullptr), m_stream(nullptr)
{
    read(p_filePath);
}

CEdfFile::~CEdfFile() { }

cv::Mat CEdfFile::data() const
{
    return m_data;
}

void CEdfFile::setData(const cv::Mat& p_mat)
{
    m_data = p_mat;
}

bool CEdfFile::write(const QString& p_path)
{
    m_metadata.setFile(p_path);
    return saveMatrix();
}

bool CEdfFile::read(const QString& p_path)
{
    if (p_path.isEmpty())
    {
        return false;
    }

    clear();
    m_metadata.setFile(p_path);
    return loadData();
}

bool CEdfFile::loadData()
{
    m_data.release();

    if (!isStreamInitialized() && !initStream())
    {
        qWarning() << "Invalid image stream reader";
        return false;
    }

    stream()->seek(0);

    //Read in whole content of a file. Will have to be done later anyways for the matrix
    QFile file(path());
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Can't read from edf file:" << path();
        qWarning() << "-- error:" << file.errorString();
        return false;
    }

    const QByteArray& fileContent = file.readAll();
    closeStream();

    QTextStream localStream(fileContent);
    int lineCount = 0;

    //For performance reasons we keep track of if the first line was already found.
    //This allows us to call the very expensive function pos() ideally only twice: At the first line and for the end of the header
    bool startLineFound = false;

    qint64 startLinePos = -1;
    while (!localStream.atEnd())
    {
        ++lineCount;

        if (!startLineFound)
        {
            startLinePos = localStream.pos();
        }
        const QString& line = localStream.readLine().trimmed();

        if (isBeginHeaderLine(line))
        {
            m_headerStart  = startLinePos;
            startLineFound = true;
            continue;
        }

        if (!line.isEmpty() && m_headerStart < 0)
        {
            qWarning() << "Skip reading badly formed edf image: content found before header section";
            qWarning() << " -- file:" << path();
            qWarning() << " -- line" << lineCount << "starting with:" << line.left(10);
            qWarning() << " -- expecting '{' character to start header section";
            Q_ASSERT(false);
            return false;
        }

        if (isEndHeaderLine(line))
        {
            m_headerStop = localStream.pos();
            break;
        }

        if (m_headerStart < 0 || line.isEmpty())
        {
            //qDebug() << "Skip empty / comment line" << line;
            continue;
        }

        if (isHeaderLine(line))
        {
            m_metadata.addProperty(parseHeaderLine(line));
            //qDebug() << "Add property from header line" << line;
            continue;
        }
    }

    if (!checkRequiredHeaderProperties())
    {
        qWarning() << "Invalid .edf image file format: missing header properties";
        return false;
    }

    QDataStream in(fileContent); // read the data serialized from the file

    // Start reading binary data after the header section
    // Beware of cases where there exist some characters before headerStart mark
    // (ie: m_headerStart > 0): we need to skip more bytes than just 'headerSize()'
    in.skipRawData(static_cast<int>(m_headerStop));

    const int binarySize = static_cast<int>(readBinarySize());
    const int rows       = static_cast<int>(readRowCount());
    const int cols       = static_cast<int>(readColumnCount());

#if 0
    const int length = binarySize;
    //char *buffer = new char[length];
    const int bytes = in.readRawData(buffer, length);
    if (bytes < 0)
    {
        qWarning() << "Can't read raw binary data from edf image file" << info().fileName();
        delete[] buffer;
        return false;
    }
#endif

    cv::Mat matrix(rows, cols, readDataType());
    const int bytes = in.readRawData(reinterpret_cast<char*>(matrix.data), binarySize);
    if (bytes < 0)
    {
        qWarning() << "Can't read raw binary data from edf image file" << metadata().fileName();
        return false;
    }

    setData(matrix);
    return true;
}

bool CEdfFile::loadHeader()
{
    clear();

    if (!isStreamInitialized() && !initStream())
    {
        return false;
    }

    stream()->seek(0);
    while (!stream()->atEnd())
    {
        const QString& line = stream()->readLine().trimmed();

        if (line.isEmpty() || isBeginHeaderLine(line))
        {
            //qDebug() << "Skip empty / comment line" << line;
            continue;
        }

        if (isHeaderLine(line))
        {
            m_metadata.addProperty(parseHeaderLine(line));
            //qDebug() << "Add property from header line" << line;
            continue;
        }

        if (isEndHeaderLine(line))
        {
            break;
        }

        break;
    }
    closeStream();

    return true;
}

bool CEdfFile::isHeaderLine(const QString& p_line) const
{
    return p_line.endsWith(";") && p_line.contains(" = ");
}

bool CEdfFile::isBeginHeaderLine(const QString& p_line) const
{
    // Warning: can't compare the line with single char '{'
    // because it may start with some 'invalid' characters.
    // Also note that we cannot use the contains() method
    // because of other properties such as 'filename'
    // that may contain '{' characters
    return p_line.trimmed().endsWith("{");
}

bool CEdfFile::isEndHeaderLine(const QString& p_line) const
{
    return p_line.trimmed().endsWith("}");
}

bool CEdfFile::checkRequiredHeaderProperties()
{
    static const QStringList required {"Dim_1", "Dim_2", "DataType"};
    for (const QString& key : required)
    {
        bool found = false;
        for (const CProperty& property : metadata().properties())
        {
            if (property.key() == key)
            {
                found = true;
            }
        }
        if (!found)
        {
            qWarning() << "-- missing required property:" << key;
            return false;
        }
    }
    return true;
}

unsigned int CEdfFile::numericValue(const QString& p_property) const
{
    bool ok                  = false;
    const unsigned int value = metadata().value(p_property).toUInt(&ok);
    if (!ok)
    {
        qWarning() << "Invalid numeric value for header property:" << p_property;
        return 0;
    }
    return value;
}

unsigned int CEdfFile::headerSize() const
{
    const unsigned int size = static_cast<unsigned int>(m_headerStop - m_headerStart);
    return (size > 0) ? size : numericValue("EDF_HeaderSize");
}

unsigned int CEdfFile::readBinarySize() const
{
    static const QHash<int, unsigned int> sizes {{CV_8SC1, sizeof(char)},
                                                 {CV_8UC1, sizeof(unsigned char)},
                                                 {CV_16SC1, sizeof(short)},
                                                 {CV_16UC1, sizeof(unsigned short)},
                                                 {CV_32SC1, sizeof(int)},
                                                 {CV_32FC1, sizeof(float)},
                                                 {CV_64FC1, sizeof(double)}};

    const int type = readDataType();
    Q_ASSERT_X(sizes.contains(type), "EDF reader", "Can't determine binary size from dimensions and datatype (unsupported image type)");

    const unsigned int elements = readRowCount() * readColumnCount();
    return sizes.contains(type) ? sizes.value(type) * elements : numericValue("EDF_BinarySize");
}

unsigned int CEdfFile::readRowCount() const
{
    return numericValue("Dim_2");
}

unsigned int CEdfFile::readColumnCount() const
{
    return numericValue("Dim_1");
}

int CEdfFile::readDataType() const
{
    static const QHash<QString, int> types {{"SignedByte", CV_8SC1},
                                            {"UnsignedByte", CV_8UC1},
                                            {"SignedShort", CV_16SC1},
                                            {"UnsignedShort", CV_16UC1},
                                            {"SignedInteger", CV_32SC1},
                                            {"FloatValue", CV_32FC1},
                                            {"DoubleValue", CV_64FC1}};

    // Warning: some .edf images are badly formatted
    // There notably exist some images with "DataType = UnsignedInteger"
    // that should be decoded as float images

    const QString& type = metadata().value("DataType").simplified();
    return types.contains(type) ? types.value(type) : CV_32FC1;
}

void CEdfFile::clear()
{
    m_headerStart = -1;
    m_headerStop  = -1;
    m_data.release();
    m_metadata.clear();
    closeStream();
}

bool CEdfFile::save(bool p_createFile)
{
    if (p_createFile && !metadata().exists())
    {
        if (!QFile(metadata().absoluteFilePath()).open(QIODevice::ReadWrite))
        {
            qWarning() << "Failed to create file " << metadata().absoluteFilePath();
        }
    }
    updateHeader();
    saveHeader();
    saveMatrix();
    return true;
}

bool CEdfFile::saveHeader()
{
    if (metadata().properties().isEmpty())
    {
        qDebug() << "No header properties to save for edf file:" << path();
        return false;
    }

    QFile file(path());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qWarning() << "Can't write edf header in file:" << path();
        qWarning() << "-- error:" << file.errorString();
        return false;
    }

    QTextStream stream(&file);
    stream << Qt::endl << "{" << Qt::endl;
    const auto& properties = metadata().properties();
    for (const auto& property : properties)
    {
        stream << property.key() << " = " << property.value() << " ; " << Qt::endl;
    }
    stream << "}" << Qt::endl;

    return true;
}

bool CEdfFile::saveMatrix()
{
    if (m_data.empty())
    {
        qDebug() << "No image data to save for edf file:" << path();
        return false;
    }

    QFile file(metadata().absoluteFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qWarning() << "Can't write edf image data in file:" << path();
        qWarning() << "-- error:" << file.errorString();
        return false;
    }

    QDataStream stream(&file);
    if (stream.writeRawData(reinterpret_cast<char*>(m_data.data), m_data.rows * m_data.cols * m_data.elemSize()) == -1)
    {
        qWarning() << "Can't write EDF data";
        return false;
    }

    return true;
}

void CEdfFile::updateHeader()
{
    m_metadata.updateProperty("DataType", matrixTypeToDataType(m_data.type()));
    m_metadata.updateProperty("Dim_1", QString::number(m_data.cols));
    m_metadata.updateProperty("Dim_2", QString::number(m_data.rows));
    m_metadata.updateProperty("ByteOrder", "LowByteFirst");
}

QString CEdfFile::matrixTypeToDataType(const int p_type) const
{
    static const QHash<int, QString> types {{CV_8SC1, "SignedByte"},
                                            {CV_8UC1, "UnsignedByte"},
                                            {CV_16SC1, "SignedShort"},
                                            {CV_16UC1, "UnsignedShort"},
                                            {CV_32SC1, "SignedInteger"},
                                            {CV_32FC1, "FloatValue"},
                                            {CV_64FC1, "DoubleValue"}};
    return types.contains(p_type) ? types.value(p_type) : QString();
}

QTextStream* CEdfFile::stream() const
{
    return m_stream;
}

bool CEdfFile::initStream()
{
    if (m_file || m_stream)
    {
        qWarning() << "Skip stream initialization for file" << metadata().fileName();
        return false;
    }

    QString path = metadata().absoluteFilePath();
    m_file       = new QFile(metadata().absoluteFilePath());
    if (!m_file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Can't read from file:" << metadata().fileName();
        return false;
    }

    m_stream = new QTextStream(m_file);
    return true;
}

void CEdfFile::closeStream()
{
    if (m_file)
    {
        m_file->close();
    }

    delete m_file;
    m_file = nullptr;

    delete m_stream;
    m_stream = nullptr;
}

bool CEdfFile::isStreamInitialized() const
{
    return m_stream != nullptr;
}

const CMetadata& CEdfFile::metadata() const
{
    return m_metadata;
}

void CEdfFile::setMetadata(const CMetadata& p_md)
{
    m_metadata = p_md;
}

QString CEdfFile::path() const
{
    return metadata().absoluteFilePath();
}

CProperty CEdfFile::parseHeaderLine(const QString& p_line, const QString& p_startMarker, const QString& p_endMarker) const
{
    static const QStringList separators = QStringList() << "    "
                                                        << " : "
                                                        << " = "
                                                        << "="
                                                        << ":"
                                                        << "  "
                                                        << " ";

    // Strategy:
    // 1. Test separators until we get at least two non-empty fields for property name and value
    // 2. If no separator can split the string, use the line content as property key with empty value
    // 3. Remove start and end markers

    // Special rule:  if the value starts with "=", the current separator may not be the best one
    //
    // Consider the line: "DataType       = SignedInteger ;"
    // The separator "    " splits the line as key: "DataType" ; value: "= SignedInteger ;"
    // but the separator " = " would be better.
    // However, beware of lines such as "equation     a = 12" where "    " is a better separator.

    QString line(p_line);

    if (line.startsWith(p_startMarker))
    {
        line = line.remove(0, p_startMarker.length());
    }

    line = line.trimmed();
    if (line.isEmpty())
    {
        return CProperty();
    }

    CProperty result;
    bool keyValuePairFound = false;
    for (const QString& separator : separators)
    {
        QStringList tokens = line.split(separator, Qt::SkipEmptyParts);
        if (tokens.count() >= 2)
        {
            QString key   = tokens.takeFirst().trimmed();
            QString value = tokens.join(separator).trimmed();

            // Special rule when value starts with "="
            if (value.startsWith(QStringLiteral("=")))
            {
                QStringList specialTokens = line.split(QStringLiteral("="), Qt::SkipEmptyParts);
                if (specialTokens.count() == 2)
                {
                    key   = specialTokens.takeFirst().trimmed();
                    value = specialTokens.join(separator).trimmed();
                }
            }

            if (value.endsWith(p_endMarker))
            {
                value.chop(p_endMarker.count());
            }

            result.setKey(key);
            result.setValue(value.trimmed());
            keyValuePairFound = true;
            break;
        }
    }

    if (!keyValuePairFound)
    {
        result.setKey(line);
    }

    return result;
}
