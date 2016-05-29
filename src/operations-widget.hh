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

class CMainWindow;
class CMatrixModel;
class CPoint2DWidget;
class CFileChooser;

/*!
  \file operations-widget.hh
  \class COperationWidget
  \brief Base widget class for applying OpenCV operations on current matrix

  A COperationWidget defines an interface to the CMatrixModel with apply() and
  reset() actions. It owns the layout but derived classes can append their
  own widgets through the addParameter() method.
*/
class COperationWidget : public QWidget
{
  Q_OBJECT

public:
  COperationWidget(const QString & p_title,
		   CMatrixModel * p_model,
		   QWidget* p_parent);

  virtual ~COperationWidget();

  QString title() const;
  CMatrixModel * model() const;
  int channels() const;

public slots:
  virtual void reset();
  virtual void apply() = 0;

protected:
  virtual void addParameter(const QString & p_label,
			    QWidget * p_widget);

  virtual void readSettings();
  virtual void writeSettings();

  void setModified(bool p_modified);

private:
  CMainWindow *m_parent;
  bool m_wasModified;

protected:
  cv::Mat m_backup;
  QPushButton *m_applyButton;

  QString m_openPath;
  QString m_savePath;

private:
  QString m_title;
  CMatrixModel * m_model;
  QFormLayout *m_parametersLayout;
};

/*!
  \class CFormatWidget
  \brief Operation widget to change the OpenCV format of the matrix

  This widget relies on cv::convertTo() method.
*/
class CFormatWidget : public COperationWidget
{
  Q_OBJECT

public:
  CFormatWidget(const QString & p_title,
		CMatrixModel * p_model,
		QWidget* p_parent = 0);

  virtual ~CFormatWidget();
  void reset();
  void apply();

private:
  QComboBox *m_typeWidget;
  QDoubleSpinBox *m_alphaWidget;
  QDoubleSpinBox *m_betaWidget;
};

/*!
  \class CScalarWidget
  \brief Operation widget to apply matrix | scalar operations

  This widget wraps cv::add() and cv::multiply() methods.
*/
class CScalarWidget : public COperationWidget
{
  Q_OBJECT

public:
  CScalarWidget(const QString & p_title,
		CMatrixModel * p_model,
		QWidget* p_parent = 0);

  virtual ~CScalarWidget();
  void reset();
  void apply();

private:
  QDoubleSpinBox *m_addWidget;
  QDoubleSpinBox *m_multiplyWidget;
};

/*!
  \class CRotationWidget
  \brief Operation widget to apply rotation on the matrix

  This widget wraps cv::warpAffine() method.
*/
class CRotationWidget : public COperationWidget
{
  Q_OBJECT

  public:
  CRotationWidget(const QString & p_title,
		  CMatrixModel * p_model,
		  QWidget* p_parent = 0);

  virtual ~CRotationWidget();
  void reset();
  void apply();

private:
  CPoint2DWidget *m_centerWidget;
  QDoubleSpinBox *m_angleWidget;
  QDoubleSpinBox *m_scaleWidget;
};

/*!
  \class CNormalizeWidget
  \brief Operation widget to apply normalization on the matrix

  This widget wraps cv::normalize() method.
*/
class CNormalizeWidget : public COperationWidget
{
  Q_OBJECT

  public:
  CNormalizeWidget(const QString & p_title,
		   CMatrixModel * p_model,
		   QWidget* p_parent = 0);

  virtual ~CNormalizeWidget();
  void reset();
  void apply();

private:
  QDoubleSpinBox *m_alphaWidget;
  QDoubleSpinBox *m_betaWidget;
  QComboBox *m_normWidget;
};


/*!
  \class CTransformationsWidget
  \brief Operation widget to apply flip and transposition on the matrix

  This widget wraps cv::flip() and cv::t() methods.
*/
class CTransformationsWidget : public COperationWidget
{
  Q_OBJECT

  public:
  CTransformationsWidget(const QString & p_title,
			 CMatrixModel * p_model,
			 QWidget* p_parent = 0);

  virtual ~CTransformationsWidget();
  void reset();
  void apply();

private:
  QPushButton *m_transposeWidget;
  QPushButton *m_verticalFlipWidget;
  QPushButton *m_horizontalFlipWidget;
  QPushButton *m_mulTransposeWidget;
};

/*!
  \class CColorMapWidget
  \brief Operation widget to apply a color map on the matrix

  This widget wraps cv::colormap().
  Note that this feature requires OpenCV 2.4.
*/
class CColorMapWidget : public COperationWidget
{
  Q_OBJECT

  public:
  CColorMapWidget(const QString & p_title,
		  CMatrixModel * p_model,
		  QWidget* p_parent = 0);

  virtual ~CColorMapWidget();
  void reset();
  void apply();

private:
  CPoint2DWidget *m_rangeWidget;
  QComboBox *m_colorMapWidget;
};

/*!
  \class CThresholdWidget
  \brief Operation widget to apply thresholding on the matrix

  This widget wraps cv::threshold().
*/
class CThresholdWidget : public COperationWidget
{
  Q_OBJECT

  public:
  CThresholdWidget(const QString & p_title,
		   CMatrixModel * p_model,
		   QWidget* p_parent = 0);

  virtual ~CThresholdWidget();
  void reset();
  void apply();

private:
  QDoubleSpinBox *m_thresholdValueWidget;
  QDoubleSpinBox *m_maxValueWidget;
  QComboBox *m_typeWidget;
  QCheckBox *m_otsuWidget;
};

/*!
  \class CMatrixWidget
  \brief Operation widget to apply matrix | matrix operations

  This widget wraps cv::absDiff(), cv::multiply(), and cv::Mat::operator*().
*/
class CMatrixWidget : public COperationWidget
{
  Q_OBJECT

  public:
  CMatrixWidget(const QString & p_title,
		CMatrixModel * p_model,
		QWidget* p_parent = 0);

  virtual ~CMatrixWidget();
  void reset();
  void apply();

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

/*!
  \class CChannelsWidget
  \brief Operation widget to merge matrix channels

  This widget wraps cv::split(), cv::merge().
  Its main use is to combine separate RGB layers into a single CV_8UC3 image.
*/
class CChannelsWidget : public COperationWidget
{
  Q_OBJECT

  public:
  CChannelsWidget(const QString & p_title,
		  CMatrixModel * p_model,
		  QWidget* p_parent = 0);

  virtual ~CChannelsWidget();
  void reset();
  void apply();

private:
  CFileChooser *m_redFileChooserWidget;
  CFileChooser *m_greenFileChooserWidget;
  CFileChooser *m_blueFileChooserWidget;
  QString m_redOpenPath;
  QString m_greenOpenPath;
  QString m_blueOpenPath;
};


#endif // __OPERATIONS_WIDGET_HH__
