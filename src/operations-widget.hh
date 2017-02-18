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

#include <QWidget>
#include <QString>

class QFormLayout;
class QDoubleSpinBox;
class QComboBox;
class QPushButton;
class QCheckBox;

class CMainWindow;
class CMatrixModel;
class CPoint2DWidget;
class CFileChooser;

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
  CMatrixModel *m_backup;
  QPushButton *m_applyButton;

  QString m_openPath;
  QString m_savePath;

private:
  QString m_title;
  CMatrixModel * m_model;
  QFormLayout *m_parametersLayout;
};

/*
  Format
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
  void reset();
  void apply();

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
  void reset();
  void apply();

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
  void reset();
  void apply();

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
  void reset();
  void apply();

private:
  QPushButton *m_transposeWidget;
  QPushButton *m_verticalFlipWidget;
  QPushButton *m_horizontalFlipWidget;
  QPushButton *m_mulTransposeWidget;
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
  void reset();
  void apply();

private:
  CPoint2DWidget *m_rangeWidget;
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
  void reset();
  void apply();

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

/*
  Channels
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
