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
#include "matrix-model.hh"

#include <QDebug>
#include <QXmlStreamReader>
#include <QFile>

#include "logger.hh"

CMatrixModel::CMatrixModel()
  : QAbstractTableModel()
  , m_data()
{
}

CMatrixModel::~CMatrixModel()
{
}

cv::Mat CMatrixModel::data() const
{
  return m_data;
}

void CMatrixModel::setData(const cv::Mat & matrix)
{
  m_data = matrix;
  emit(dataChanged(QModelIndex(), QModelIndex()));
}

int CMatrixModel::rowCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return data().rows;
}

int CMatrixModel::columnCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return data().cols;
}

QVariant CMatrixModel::data(const QModelIndex & index, int role) const
{
  switch (role)
    {
    case Qt::TextAlignmentRole:
      return Qt::AlignCenter;

    case Qt::EditRole:
    case Qt::DisplayRole:
      {
	cv::Vec2b point2b;
	cv::Vec3b point3b;
	cv::Vec2f point2f;
	cv::Vec3f point3f;
	cv::Vec2d point2d;
	cv::Vec3d point3d;
	switch(type())
	  {
	  case CV_8UC1:
	    return data().at< uchar >(index.row(), index.column());

	  case CV_8UC2:
	    point2b = data().at< cv::Vec2b >(index.row(), index.column());
	    return QString("%1 | %2").arg(point2b[0]).arg(point2b[1]);

	  case CV_8UC3:
	    point3b = data().at< cv::Vec3b >(index.row(), index.column());
	    return QString("%1 | %2 | %3").arg(point3b[0]).arg(point3b[1]).arg(point3b[2]);

	  case CV_16UC1:
	    return data().at< unsigned short >(index.row(), index.column());

	  case CV_16UC2:
	    point2b = data().at< cv::Vec2b >(index.row(), index.column());
	    return QString("%1 | %2").arg(point2b[0]).arg(point2b[1]);

	  case CV_16UC3:
	    point3b = data().at< cv::Vec3b >(index.row(), index.column());
	    return QString("%1 | %2 | %3").arg(point3b[0]).arg(point3b[1]).arg(point3b[2]);


	  case CV_32SC1:
	    return data().at< int >(index.row(), index.column());

	  case CV_32FC1:
	    return data().at< float >(index.row(), index.column());

	  case CV_32FC2:
	    point2f = data().at< cv::Vec2f >(index.row(), index.column());
	    return QString("%1 | %2").arg(point2f[0]).arg(point2f[1]);

	  case CV_32FC3:
	    point3f = data().at< cv::Vec3f >(index.row(), index.column());
	    return QString("%1 | %2 | %3").arg(point3f[0]).arg(point3f[1]).arg(point3f[2]);

	  case CV_64FC1:
	    return data().at< double >(index.row(), index.column());

	  case CV_64FC2:
	    point2d = data().at< cv::Vec2d >(index.row(), index.column());
	    return QString("%1 | %2").arg(point2d[0]).arg(point2d[1]);

	  case CV_64FC3:
	    point3d = data().at< cv::Vec3d >(index.row(), index.column());
	    return QString("%1 | %2 | %3").arg(point3d[0]).arg(point3d[1]).arg(point3d[2]);

	  default:
	    return QVariant();
	  }
	break;
      }

    default:
      data(index, Qt::DisplayRole);
    }

  return QVariant();
}

bool CMatrixModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (role == Qt::EditRole)
    {
      if (channels() > 1)
        {
          qWarning() << "Edition on multi-channel matrices not supported yet";
          return false;
        }

      cv::Mat tmp;
      m_data.convertTo(tmp, CV_64FC1);
      tmp.at< double >(index.row(), index.column()) = value.toDouble();
      tmp.convertTo(m_data, m_data.type());

      emit(dataChanged(QModelIndex(), QModelIndex()));
      return true;
    }

  qWarning() << "setdata role not supported yet";
  return false;
}

QVariant CMatrixModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole)
    {
      if (orientation == Qt::Horizontal)
        {
          return section < m_horizontalHeaderLabels.size() ?
			   m_horizontalHeaderLabels.at(section) : QString::number(section);
        }
      else if (orientation == Qt::Vertical)
        {
          return section < m_verticalHeaderLabels.size() ?
			   m_verticalHeaderLabels.at(section) : QString::number(section);
        }
    }
  return QVariant();
}

Qt::ItemFlags CMatrixModel::flags(const QModelIndex & index) const
{
  (void) index;
  return Qt::ItemIsSelectable | Qt::ItemIsEditable  | Qt::ItemIsEnabled;
}


void CMatrixModel::sort(int column, Qt::SortOrder order)
{
  if (channels() != 1)
    {
      qWarning() << tr("Sorting matrix with multiple channels is not implemented yet");
      return;
    }

  const int cvOrder = (order == Qt::AscendingOrder) ? CV_SORT_ASCENDING : CV_SORT_DESCENDING;

  cv::Mat sortedData(m_data.size(), m_data.type());
  cv::Mat sortedColumnIndex;
  cv::sortIdx(m_data.col(column), sortedColumnIndex, CV_SORT_EVERY_COLUMN | cvOrder);
  for (int i = 0; i < sortedColumnIndex.rows; ++i)
    {
      cv::Mat tmp = sortedData.row(i);
      m_data.row(sortedColumnIndex.at<int>(i, 0)).copyTo(tmp);
    }
  sortedData.assignTo(m_data);

  emit(dataChanged(QModelIndex(), QModelIndex()));
}

int CMatrixModel::channels() const
{
  return m_data.channels();
}

int CMatrixModel::type() const
{
  return m_data.type();
}

QString CMatrixModel::typeString() const
{
  switch (type() % 8)
    {
    case 0:
      return "8U";

    case 1:
      return "8S";

    case 2:
      return "16U";

    case 3:
      return "16S";

    case 4:
      return "32S";

    case 5:
      return "32F";

    case 6:
      return "64F";

    default:
      return "unknown";
    }
}

void CMatrixModel::setProfile(const QString & profile)
{
  if (profile.isEmpty())
    return;

  // Parse profile for rows/columns labels
  m_horizontalHeaderLabels.clear();
  m_verticalHeaderLabels.clear();

  QFile file(profile);
  if (!file.open(QIODevice::ReadOnly))
    {
      qWarning() << tr("Can't open profile in read mode: %1").arg(profile);
      return;
    }

  QXmlStreamReader xml(&file);
  while (!xml.atEnd())
    {
      if (xml.readNext())
        {
          if (xml.name() == "column")
            {
              QString label = xml.attributes().value("label").toString().simplified();
              if (!label.isEmpty())
                m_horizontalHeaderLabels << label;
            }
          else if (xml.name() == "row")
            {
              QString label = xml.attributes().value("label").toString().simplified();
              if (!label.isEmpty())
                m_verticalHeaderLabels << label;
            }
        }
    }

  if (xml.hasError())
    {
      qWarning() << tr("Badly formed xml document: %1").arg(profile);
      qWarning() << tr("Error: %1").arg(xml.errorString());
    }

  file.close();

  emit(headerDataChanged(Qt::Horizontal, 0, m_horizontalHeaderLabels.size()));
  emit(headerDataChanged(Qt::Vertical, 0, m_verticalHeaderLabels.size()));
}



/*
  Operations
*/

int CMatrixModel::total() const
{
  int res = 0;

  try
    {
      res = m_data.total();
    }
  catch (cv::Exception & e)
    {
      qWarning() << e;
    }

  return res;
}

int CMatrixModel::countNonZeros() const
{
  int res = 0;

  try
    {
      res = cv::countNonZero(m_data);
    }
  catch (cv::Exception & e)
    {
      qWarning() << e;
    }

  return res;
}

void CMatrixModel::minMaxLoc(double* p_minVal, double* p_maxVal,
			     cv::Point* p_minLoc, cv::Point* p_maxLoc)
{
  try
    {
      cv::minMaxLoc(m_data, p_minVal, p_maxVal, p_minLoc, p_maxLoc);
    }
  catch (cv::Exception & e)
    {
      qWarning() << e;
    }
}

void CMatrixModel::meanStdDev(double* p_mean,
			      double* p_stddev)
{
  try
    {
      cv::Scalar mean, stddev;
      cv::meanStdDev(m_data, mean, stddev);
      *p_mean = mean[0];     //first channel
      *p_stddev = stddev[0]; //first channel
    }
  catch (cv::Exception & e)
    {
      qWarning() << e;
    }
}

void CMatrixModel::convertTo(const int p_type,
			     const double p_alpha,
			     const double p_beta)
{
  if (p_type != type() || p_alpha != 1 || p_beta != 0)
    {
      try
        {
          m_data.convertTo(m_data, p_type, p_alpha, p_beta);
          emit(dataChanged(QModelIndex(), QModelIndex()));
        }
      catch (cv::Exception & e)
        {
	  qWarning() << e;
        }
    }
}

void CMatrixModel::add(double p_value)
{
  if (p_value != 0)
    {
      try
	{
	  m_data += p_value;
	  emit(dataChanged(QModelIndex(), QModelIndex()));
	}
      catch (cv::Exception & e)
	{
	  qWarning() << e;
	}
    }
}

void CMatrixModel::multiply(double p_value)
{
  if (p_value != 1)
    {
      try
	{
	  m_data *= p_value;
	  emit(dataChanged(QModelIndex(), QModelIndex()));
	}
      catch (cv::Exception & e)
	{
	  qWarning() << e;
	}
    }
}

void CMatrixModel::transpose()
{
  try
    {
      m_data = m_data.t();
      emit(dataChanged(QModelIndex(), QModelIndex()));
    }
  catch (cv::Exception & e)
    {
      qWarning() << e;
    }
}

void CMatrixModel::verticalFlip()
{
  try
    {
      cv::flip(m_data, m_data, 0);
      emit(dataChanged(QModelIndex(), QModelIndex()));
    }
  catch (cv::Exception & e)
    {
      qWarning() << e;
    }
}

void CMatrixModel::horizontalFlip()
{
  try
    {
      cv::flip(m_data, m_data, 1);
      emit(dataChanged(QModelIndex(), QModelIndex()));
    }
  catch (cv::Exception & e)
    {
      qWarning() << e;
    }
}


void CMatrixModel::rotate(const cv::Point & p_center,
                          const double p_angle_dg,
                          const double p_scaleFactor)
{
  try
    {
      cv::Mat rotation = getRotationMatrix2D(p_center, p_angle_dg, p_scaleFactor);

      cv::Mat dst;
      cv::warpAffine(m_data, dst, rotation, m_data.size());
      m_data = dst;

      emit(dataChanged(QModelIndex(), QModelIndex()));
    }
  catch (cv::Exception & e)
    {
      qWarning() << e;
    }
}

void CMatrixModel::normalize(const double p_alpha,
                             const double p_beta,
                             const int p_norm)
{
  try
    {
      cv::normalize(m_data, m_data, p_alpha, p_beta, p_norm);
      emit(dataChanged(QModelIndex(), QModelIndex()));
    }
  catch (cv::Exception & e)
    {
      qWarning() << e;
    }
}

void CMatrixModel::absdiff(const cv::Mat & p_other)
{
  try
    {
      cv::absdiff(m_data, p_other, m_data);
      emit(dataChanged(QModelIndex(), QModelIndex()));
    }
  catch (cv::Exception & e)
    {
      qWarning() << e;
    }
}

void CMatrixModel::multiplyElements(const cv::Mat & p_other)
{
  try
    {
      m_data = m_data.mul(p_other);
      emit(dataChanged(QModelIndex(), QModelIndex()));
    }
  catch (cv::Exception & e)
    {
      qWarning() << e;
    }
}

void CMatrixModel::multiplyMatrix(const cv::Mat & p_other)
{
  try
    {
      m_data = m_data * p_other;
      emit(dataChanged(QModelIndex(), QModelIndex()));
    }
  catch (cv::Exception & e)
    {
      qWarning() << e;
    }
}

void CMatrixModel::applyColorMap(const int p_colorMap)
{
#if (CV_MAJOR_VERSION == 2) and (CV_MINOR_VERSION <= 3)
  Q_UNUSED(p_colorMap);
  qWarning() << tr("cv::applyColorMap requires a more recent version of OpenCV.");
  qWarning() << tr("Current OpenCV version: %1.%2")
    .arg(QString::number(CV_MAJOR_VERSION))
    .arg(QString::number(CV_MINOR_VERSION));
#else
  try
    {
      cv::applyColorMap(m_data, m_data, p_colorMap);
      emit(dataChanged(QModelIndex(), QModelIndex()));
    }
  catch (cv::Exception & e)
    {
      qWarning() << e;
    }
#endif
}

void CMatrixModel::threshold(const double p_threshold,
			     const double p_maxValue,
			     const int p_type)
{
  try
    {
      cv::threshold(m_data, m_data, p_threshold, p_maxValue, p_type);
      emit(dataChanged(QModelIndex(), QModelIndex()));
    }
  catch (cv::Exception & e)
    {
      qWarning() << e;
    }
}
