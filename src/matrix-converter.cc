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

#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QSettings>
#include <QDebug>

#include "config.hh"

#include "mfe.hh"

CMatrixConverter::CMatrixConverter() :
QObject()
, m_data()
, m_format(Format_Unknown)
, m_rawType(0)
, m_rawWidth(0)
, m_rawHeight(0)
{
}

CMatrixConverter::CMatrixConverter(const QString & filename) :
QObject()
, m_data()
, m_format(Format_Unknown)
, m_rawType(0)
, m_rawWidth(0)
, m_rawHeight(0)
{
  if (!load(filename))
  {
    qWarning() << "Can't load file: " << filename;
  }
}

CMatrixConverter::~CMatrixConverter()
{
}

void CMatrixConverter::readSettings()
{
  QSettings settings;
  settings.beginGroup("image");
  m_rawType = settings.value("raw-type", 0).toInt();
  m_rawWidth = settings.value("raw-width", 2160).toInt();
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

bool CMatrixConverter::load(const QString & filename)
{
  if (filename.endsWith(".xml", Qt::CaseInsensitive))
  {
    m_format = Format_Xml;
    return loadFromXml(filename);
  }
  else if (filename.endsWith(".txt", Qt::CaseInsensitive))
  {
    m_format = Format_Txt;
    return loadFromTxt(filename);
  }
  else if (filename.endsWith(".raw", Qt::CaseInsensitive))
  {
    m_format = Format_Raw;
    return loadFromRaw(filename);
  }
  else if (filename.endsWith(".mfe", Qt::CaseInsensitive))
  {
    m_format = Format_Mfe;
    return loadFromMfe(filename);
  }
  else if (filename.endsWith(".bmp", Qt::CaseInsensitive))
  {
    m_format = Format_Bmp;
    return loadFromImage(filename);
  }
  else if (filename.endsWith(".jpg", Qt::CaseInsensitive) || filename.endsWith(".jpeg", Qt::CaseInsensitive))
  {
    m_format = Format_Jpg;
    return loadFromImage(filename);
  }
  else if (filename.endsWith(".png", Qt::CaseInsensitive))
  {
    m_format = Format_Png;
    return loadFromImage(filename);
  }
  else
  {
    m_format = Format_Unknown;
    return loadFromImage(filename);
  }

  return false;
}

bool CMatrixConverter::save(const QString & filename)
{
  if (filename.endsWith(".xml", Qt::CaseInsensitive))
  {
    m_format = Format_Xml;
    return saveToXml(filename);
  }
  else if (filename.endsWith(".txt", Qt::CaseInsensitive))
  {
    m_format = Format_Txt;
    return saveToTxt(filename);
  }
  else if (filename.endsWith(".raw", Qt::CaseInsensitive))
  {
    m_format = Format_Raw;
    return saveToRaw(filename);
  }
  else if (filename.endsWith(".mfe", Qt::CaseInsensitive))
  {
    m_format = Format_Mfe;
    return saveToMfe(filename);
  }
  else if (filename.endsWith(".bmp", Qt::CaseInsensitive))
  {
    m_format = Format_Bmp;
    return saveToImage(filename);
  }
  else if (filename.endsWith(".jpg", Qt::CaseInsensitive) || filename.endsWith(".jpeg", Qt::CaseInsensitive))
  {
    m_format = Format_Jpg;
    return saveToImage(filename);
  }
  else if (filename.endsWith(".png", Qt::CaseInsensitive))
  {
    m_format = Format_Png;
    return saveToImage(filename);
  }
  else
  {
    m_format = Format_Unknown;
    return saveToImage(filename);
  }

  return false;
}

cv::Mat CMatrixConverter::data() const
{
  return m_data;
}

void CMatrixConverter::setData(const cv::Mat & matrix)
{
  m_data = matrix;
}

bool CMatrixConverter::loadFromTxt(const QString & filename)
{
  QFile file(filename);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream stream(&file);

    // first line contains the number of columns and rows of the matrix
    QStringList values = stream.readLine().split(" ");
    if (values.size() != 2)
    {
      qWarning() << tr("CMatrixConverter::loadFromTxt first line of file [%1] should contain COL ROW information but has [%2]").arg(filename).arg(values.join(" "));
      return false;
    }

    const uint rows = values[1].toInt(), cols = values[0].toInt();
    if (rows == 0 || cols == 0)
    {
      qWarning() << tr("CMatrixConverter::loadFromTxt invalid dimensions for matrix: ") << filename;
      return false;
    }

    m_data.create(rows, cols, CV_64FC1);

    // second line contains matrix values
    values = stream.readLine().split(" ");

    for (uint v = 0; v < rows * cols; ++v)
    {
      int row = v / cols;
      int col = v % cols;
      m_data.at< double >(row, col) = values[v].toDouble();
    }

    file.close();

    return true;
  }
  else
  {
    qWarning() << tr("CMatrixConverter::loadFromTxt unable to open: ") << filename;
    return false;
  }
}

bool CMatrixConverter::saveToTxt(const QString & filename)
{
  m_data.convertTo(m_data, CV_64FC1);

  QFile file(filename);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QTextStream stream(&file);
    stream << m_data.cols << " " << m_data.rows << "\n";

    for (int j = 0; j < m_data.rows; ++j)
    {
      for (int i = 0; i < m_data.cols; ++i)
      {
        stream << m_data.at< double >(j, i) << " ";
      }
    }

    file.close();
    return true;
  }
  else
  {
    qWarning() << tr("CMatrixConverter::saveToTxt unable to open: ") << filename;
    return false;
  }
}

bool CMatrixConverter::loadFromXml(const QString & filename)
{
  try
  {
    cv::FileStorage fs;
    if (!fs.open(filename.toStdString(), cv::FileStorage::READ))
    return false;

    fs["matrix"] >> m_data;
    fs.release();
  }
  catch (cv::Exception & e)
  {
    qWarning() << tr("CMatrixConverter::loadFromXml invalid matrix: ") << filename;
    return false;
  }

  return true;
}

bool CMatrixConverter::saveToXml(const QString & filename)
{
  cv::FileStorage fs;
  if (!fs.open(filename.toStdString(), cv::FileStorage::WRITE))
  {
    return false;
  }

  fs << "matrix" << m_data;
  fs.release();

  return true;
}

bool CMatrixConverter::loadFromImage(const QString & filename)
{
  try
  {
    m_data = cv::imread(filename.toStdString(), -1);
    if (m_data.type() == CV_8UC4)
    {
      cv::cvtColor(m_data, m_data, CV_BGRA2BGR);  // remove alpha channel
    }
  }
  catch (cv::Exception & e)
  {
    qWarning() << tr("CMatrixConverter::loadFromImage invalid matrix: ") << filename;
    return false;
  }

  return true;
}

bool CMatrixConverter::saveToImage(const QString & filename)
{
  bool ret;
  try
  {
    std::vector<int> compression_params;
    if (m_format == Format_Png)
    {
      compression_params.push_back(CV_IMWRITE_PNG_STRATEGY);
      compression_params.push_back(CV_IMWRITE_PNG_STRATEGY_DEFAULT);
      compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
      compression_params.push_back(9);
    }
    else if (m_format == Format_Jpg)
    {
      compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
      compression_params.push_back(80);
    }

    ret = cv::imwrite(filename.toStdString(), m_data, compression_params);
  }
  catch (cv::Exception & e)
  {
    qWarning() << tr("CMatrixConverter::saveToImage invalid matrix: ") << filename;
    return false;
  }
  return ret;
}

bool CMatrixConverter::loadFromRaw(const QString & filename)
{
  if (m_rawType == 0)
  {
    unsigned short *buffer = new unsigned short[m_rawWidth * m_rawHeight];
    FILE* fd = fopen(filename.toStdString().c_str(), "rb");
    if (fd != NULL)
    {
      size_t readBytes = fread(buffer, sizeof(unsigned short), m_rawWidth * m_rawHeight, fd);
      fclose(fd);

      if (readBytes == 0)
      {
        qWarning() << tr("Nothing to read from raw image file: %1").arg(filename);
        delete [] buffer;
        return false;
      }
    }
    else
    {
      qWarning() << tr("Can't open raw image file in read mode: %1").arg(filename);
      delete [] buffer;
      return false;
    }

    cv::Mat rawData(cv::Size(m_rawWidth, m_rawHeight), CV_16U, buffer);

    const double ratio = 127.5 / 511.5; // conversion from 10bits to 8 bits
    m_data = rawData * ratio;
    m_data.convertTo(m_data, CV_8UC1);
    delete [] buffer;
  }
  else
  {
    qWarning() << "CMatrixConverter::loadFromRaw format not supported yet";
    return false;
  }

  return true;
}

bool CMatrixConverter::saveToRaw(const QString & filename)
{
  Q_UNUSED(filename);
  qWarning() << "CMatrixConverter::saveToRaw not implemented yet";
  return false;
}

bool CMatrixConverter::loadFromMfe(const QString & filename)
{
  try
  {
    MatrixFormatExchange mfe;
    mfe.read(filename);
    m_data = mfe.data();
  }
  catch (cv::Exception & e)
  {
    qWarning() << "OpenCV error while reading MFE matrix";
    qWarning() << "file: " << filename;
    qWarning() << "error: " << e.what();
    return false;
  }

  return true;
}

bool CMatrixConverter::saveToMfe(const QString & filename)
{
  try
  {
    MatrixFormatExchange mfe;
    mfe.setData(m_data);
    mfe.setComment("MatrixViewer");
    mfe.write(filename);
  }
  catch (cv::Exception & e)
  {
    qWarning() << "OpenCV error while reading MFE matrix";
    qWarning() << "file: " << filename;
    qWarning() << "error: " << e.what();
    return false;
  }

  return true;
}
