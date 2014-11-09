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

#ifndef __OPERATIONS_WIDGET_HH__
#define __OPERATIONS_WIDGET_HH__

#include <QWidget>
#include <QString>

#include <opencv2/opencv.hpp>

class QFormLayout;
class QDoubleSpinBox;
class QComboBox;
class QPushButton;
class QCheckBox;
class CMatrixModel;
class CPoint2DWidget;
class CFileChooser;

class COperationWidget : public QWidget
{
  Q_OBJECT

public:
  COperationWidget(const QString & p_title,
		   CMatrixModel * p_model,
		   QWidget* p_parent = 0);

  virtual ~COperationWidget();

  QString title() const;
  CMatrixModel * model() const;
  int channels() const;

  virtual void reset() = 0;

  void addParameter(const QString & p_label,
		    QWidget * p_widget);

protected:
  cv::Mat m_backup;

private:
  QString m_title;
  CMatrixModel * m_model;
  QFormLayout *m_parametersLayout;
};

/*
  Scalar operations
*/

class CScalarWidget : public COperationWidget
{
  Q_OBJECT

public:
  CScalarWidget(const QString & p_title,
		CMatrixModel * p_model,
		QWidget* p_parent = 0);

  virtual ~CScalarWidget();
  virtual void reset();

private:
  QDoubleSpinBox *m_addWidget;
  QDoubleSpinBox *m_multiplyWidget;
};

/*
  Rotation widget
*/

class CRotationWidget : public COperationWidget
{
  Q_OBJECT

  public:
  CRotationWidget(const QString & p_title,
		  CMatrixModel * p_model,
		  QWidget* p_parent = 0);

  virtual ~CRotationWidget();
  virtual void reset();

private slots:
  void rotate();

private:
  CPoint2DWidget *m_centerWidget;
  QDoubleSpinBox *m_angleWidget;
  QDoubleSpinBox *m_scaleWidget;
};

/*
  Normalize
*/

class CNormalizeWidget : public COperationWidget
{
  Q_OBJECT

  public:
  CNormalizeWidget(const QString & p_title,
		   CMatrixModel * p_model,
		   QWidget* p_parent = 0);

  virtual ~CNormalizeWidget();
  virtual void reset();

private slots:
  void normalize();

private:
  QDoubleSpinBox *m_alphaWidget;
  QDoubleSpinBox *m_betaWidget;
  QComboBox *m_normWidget;
};

/*
  Transformations
*/

class CTransformationsWidget : public COperationWidget
{
  Q_OBJECT

  public:
  CTransformationsWidget(const QString & p_title,
			 CMatrixModel * p_model,
			 QWidget* p_parent = 0);

  virtual ~CTransformationsWidget();
  virtual void reset();

private:
  QPushButton *m_transposeWidget;
  QPushButton *m_verticalFlipWidget;
  QPushButton *m_horizontalFlipWidget;
};

/*
  ColorMap
*/

class CColorMapWidget : public COperationWidget
{
  Q_OBJECT

  public:
  CColorMapWidget(const QString & p_title,
		  CMatrixModel * p_model,
		  QWidget* p_parent = 0);

  virtual ~CColorMapWidget();
  virtual void reset();

private slots:
  void colorMap(const QString & p_type);

private:
  QComboBox *m_colorMapWidget;
};

/*
  Threshold
*/

class CThresholdWidget : public COperationWidget
{
  Q_OBJECT

  public:
  CThresholdWidget(const QString & p_title,
		   CMatrixModel * p_model,
		   QWidget* p_parent = 0);

  virtual ~CThresholdWidget();
  virtual void reset();

private slots:
  void threshold();

private:
  QDoubleSpinBox *m_thresholdValueWidget;
  QDoubleSpinBox *m_maxValueWidget;
  QComboBox *m_typeWidget;
  QCheckBox *m_otsuWidget;
};

/*
  Matrix-Matrix
*/

class CMatrixWidget : public COperationWidget
{
  Q_OBJECT

  public:
  CMatrixWidget(const QString & p_title,
		CMatrixModel * p_model,
		QWidget* p_parent = 0);

  virtual ~CMatrixWidget();
  virtual void reset();

private slots:
  void absDiff();
  void multiplyElements();
  void multiplyMatrix();

private:
  CFileChooser *m_fileChooserWidget;
  QPushButton *m_absDiffWidget;
  QPushButton *m_multiplyElementsWidget;
  QPushButton *m_multiplyMatrixWidget;
};


#endif // __OPERATIONS_WIDGET_HH__
