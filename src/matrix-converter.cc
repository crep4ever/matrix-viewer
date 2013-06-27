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
#include <QDebug>

CMatrixConverter::CMatrixConverter()
  : QObject()
  , m_data()
{
}

CMatrixConverter::CMatrixConverter(const QString & filename)
  : QObject()
  , m_data()
{
  if (!load(filename))
    qWarning() << "Can't load file: " << filename;
}


CMatrixConverter::~CMatrixConverter()
{
}

bool CMatrixConverter::load(const QString & filename)
{
  if (filename.endsWith(".xml"))
    return loadFromXml(filename);
  else if (filename.endsWith(".txt"))
    return loadFromTxt(filename);

  return false;
}

bool CMatrixConverter::save(const QString & filename)
{
  if (filename.endsWith(".xml"))
    return saveToXml(filename);
  else if (filename.endsWith(".txt"))
    return saveToTxt(filename);

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
      QString line = stream.readLine();
      QStringList values = line.split(" ");
      const uint rows = values[1].toInt(), cols = values[0].toInt();
      if (rows == 0 || cols == 0)
        {
          qWarning() << tr("CMatrixConverter::loadFromTxt invalid dimensions for matrix: ") << filename;
          return false;
        }

      m_data = cv::Mat::zeros(rows, cols, CV_64FC1);
      //qDebug() << "CMatrixConverter::loadFromTxt cols: " << cols << " rows: " << rows;

      // second line contains matrix values
      line = stream.readLine();
      values = line.split(" ");
      //qDebug() << "CMatrixConverter::loadFromTxt values: " << values;

      int count = 0;
      for (uint j = 0; j < rows; ++j)
	for (uint i = 0; i < cols; ++i)
	  m_data.at< double >(j, i) = values[count++].toDouble();

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
  QFile file(filename);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      QTextStream stream(&file);
      stream << m_data.cols << " " << m_data.rows << "\n";

      for (int j = 0; j < m_data.rows; ++j)
	for (int i = 0; i < m_data.cols; ++i)
	  stream << m_data.at< double >(j, i) << " ";

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
  cv::FileStorage fs;
  if (!fs.open(filename.toStdString(), cv::FileStorage::READ))
    return false;

  try
    {
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
    return false;

  fs << "matrix" << m_data;
  fs.release();

  return true;

}

void CMatrixConverter::print() const
{
  for (int i = 0; i < m_data.rows; ++i)
    {
      for (int j = 0; j < m_data.cols; ++j)
	{
	  std::cout << m_data.at< double >(i, j);
	}
      std::cout << std::endl;
    }
}
