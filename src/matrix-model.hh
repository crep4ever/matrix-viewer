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

#include <QAbstractTableModel>
#include <QStringList>
#include <opencv2/opencv.hpp>

#include "matrix-converter.hh"

/*!
  \file matrix-model.hh
  \class CMatrixModel
  \brief CMatrixModel is a model that represent matrix data
*/

class QImage;

class CMatrixModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  /// Default constructor.
  CMatrixModel();

  /// Copy constructor.
  CMatrixModel(const CMatrixModel & p_other);

  /// Loader constructor.
  CMatrixModel(const QString & p_filePath);

  /// OpenCV wrapper constructor.
  CMatrixModel(const int p_rows, const int p_cols,
               const int p_type,
               const double p_value1, const double p_value2, const double p_value3);

  /// Destructor.
  ~CMatrixModel() override;

  CMatrixModel* clone() const;

  bool isFormatData() const;
  bool isFormatImage() const;

  const QString & filePath() const;

  cv::Mat data() const;
  void setData(const cv::Mat & p_matrix);

  int rowCount(const QModelIndex & p_parent = QModelIndex()) const override;
  int columnCount(const QModelIndex & p_parent = QModelIndex()) const override;
  QVariant data(const QModelIndex & p_index, int p_role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex & p_index, const QVariant & p_value, int p_role = Qt::EditRole) override;
  void sort(int p_column, Qt::SortOrder p_order = Qt::AscendingOrder) override;
  QVariant headerData(int p_section, Qt::Orientation p_orientation, int p_role = Qt::DisplayRole) const override;

  Qt::ItemFlags flags(const QModelIndex & p_index) const override;

  bool removeRows   (int p_row,    int p_count, const QModelIndex & p_parent = QModelIndex()) override;
  bool removeColumns(int p_column, int p_count, const QModelIndex & p_parent = QModelIndex()) override;

  bool insertRows   (int p_row,    int p_count, const QModelIndex & p_parent = QModelIndex()) override;
  bool insertColumns(int p_column, int p_count, const QModelIndex & p_parent = QModelIndex()) override;


  void setProfile(const QString & p_profile);

  QImage* toQImage() const;

  QString valueDescription() const;

  static bool compare(CMatrixModel *p_model,
                      CMatrixModel *p_other);

  // OpenCV wrappers
  int channels() const;
  int type() const;
  QString typeString(const bool p_full = false) const;

  size_t total() const;
  int countNonZeros() const;
  void minMaxLoc(double *p_minVal, double *p_maxVal = nullptr,
                 QPoint *p_minLoc = nullptr, QPoint *p_maxLoc = nullptr);

  void meanStdDev(double* p_mean, double *p_stddev);

  QPointF center() const;

public slots:

  // format
  void convertTo(const int p_type,
                 const double p_alpha,
                 const double p_beta);

  // channels
  void merge(const QStringList & p_channels);

  // scalar
  void add(const double p_value);

  void multiply(const double p_value);

  // transform
  void transpose();

  void mulTranspose();

  void verticalFlip();

  void horizontalFlip();

  void rotate(const QPointF & p_center,
              const double p_angle_dg,
              const double p_scaleFactor);

  void normalize(const double p_alpha,
                 const double p_beta,
                 const int p_norm);

  // matrix-matrix
  void absdiff(const cv::Mat & p_other);

  void multiplyElements(const cv::Mat & p_other);

  void multiplyMatrix(const cv::Mat & p_other);

  // image
  void applyColorMap(const int p_colorMap);

  void threshold(const double p_threshold,
                 const double p_maxValue,
                 const int p_type);

private:

  QString m_filePath;
  cv::Mat m_data;
  CMatrixConverter::FileFormat m_format;
  QStringList m_horizontalHeaderLabels;
  QStringList m_verticalHeaderLabels;
};
