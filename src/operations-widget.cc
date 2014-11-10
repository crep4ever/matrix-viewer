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

#include "operations-widget.hh"

#include <QFormLayout>
#include <QBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QDebug>

#include "main-window.hh"
#include "matrix-model.hh"
#include "matrix-converter.hh"
#include "file-chooser.hh"
#include "common-widgets.hh"

COperationWidget::COperationWidget(const QString & p_title,
				   CMatrixModel * p_model,
				   QWidget* p_parent) :
  QWidget(p_parent)
  , m_backup(p_model->data())
  , m_title(p_title)
  , m_model(p_model)
  , m_parametersLayout(new QFormLayout)
{
  QGroupBox *operationGroupBox = new QGroupBox(p_title);
  operationGroupBox->setLayout(m_parametersLayout);

  QBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(operationGroupBox);
  setLayout(layout);
}

COperationWidget::~COperationWidget()
{
}

QString COperationWidget::title() const
{
  return m_title;
}

CMatrixModel * COperationWidget::model() const
{
  return m_model;
}

int COperationWidget::channels() const
{
  return m_model->channels();
}

void COperationWidget::addParameter(const QString & p_label,
				    QWidget * p_widget)
{
  m_parametersLayout->addRow(p_label, p_widget);
}

/*
  Scalar operations
*/

CScalarWidget::CScalarWidget(const QString & p_title,
			     CMatrixModel * p_model,
			     QWidget* p_parent) :
  COperationWidget(p_title, p_model, p_parent)
  , m_addWidget(new QDoubleSpinBox)
  , m_multiplyWidget(new QDoubleSpinBox)
{

  m_addWidget->setRange(-DBL_MAX, DBL_MAX);
  m_multiplyWidget->setRange(-DBL_MAX, DBL_MAX);

  addParameter(tr("add"), m_addWidget);
  addParameter(tr("multiply"), m_multiplyWidget);

  connect(m_addWidget, SIGNAL(valueChanged(double)),
	  model(), SLOT(add(double)));

  connect(m_multiplyWidget, SIGNAL(valueChanged(double)),
	  model(), SLOT(multiply(double)));
}

CScalarWidget::~CScalarWidget()
{
}

void CScalarWidget::reset()
{
  m_addWidget->setValue(0);
  m_multiplyWidget->setValue(1);
}

/*
  Rotation widget
*/

CRotationWidget::CRotationWidget(const QString & p_title,
				 CMatrixModel * p_model,
				 QWidget* p_parent) :
  COperationWidget(p_title, p_model, p_parent)
  , m_centerWidget(new CPoint2DWidget)
  , m_angleWidget(new QDoubleSpinBox)
  , m_scaleWidget(new QDoubleSpinBox)
{
  m_angleWidget->setRange(-360, 360);

  QPushButton *apply = new QPushButton(tr("Apply"), this);
  connect(apply, SIGNAL(clicked()), this, SLOT(rotate()));

  addParameter(tr("center"), m_centerWidget);
  addParameter(tr("angle"), m_angleWidget);
  addParameter(tr("scale"), m_scaleWidget);
  addParameter("", apply);
}

CRotationWidget::~CRotationWidget()
{
}

void CRotationWidget::reset()
{
  const float x = model()->data().cols / 2.0;
  const float y = model()->data().rows / 2.0;
  m_centerWidget->setPoint(QPointF(x, y));

  m_angleWidget->setValue(0);
  m_scaleWidget->setValue(1);
}

void CRotationWidget::rotate()
{
  model()->setData(m_backup.clone());

  const cv::Point center(m_centerWidget->point().x(),
			 m_centerWidget->point().y());

  model()->rotate(center,
		  m_angleWidget->value(),
		  m_scaleWidget->value());
}

/*
  Normalize widget
*/

CNormalizeWidget::CNormalizeWidget(const QString & p_title,
				   CMatrixModel * p_model,
				   QWidget* p_parent) :
  COperationWidget(p_title, p_model, p_parent)
  , m_alphaWidget(new QDoubleSpinBox)
  , m_betaWidget(new QDoubleSpinBox)
  , m_normWidget(new QComboBox)
{
  m_alphaWidget->setRange(-DBL_MAX, DBL_MAX);
  m_betaWidget->setRange(-DBL_MAX, DBL_MAX);

  m_normWidget->addItem("L1");
  m_normWidget->addItem("L2");
  m_normWidget->addItem("INF");
  m_normWidget->setCurrentIndex(1);

  QPushButton *apply = new QPushButton(tr("Apply"), this);
  connect(apply, SIGNAL(clicked()), this, SLOT(normalize()));

  addParameter(tr("alpha"), m_alphaWidget);
  addParameter(tr("beta"), m_betaWidget);
  addParameter(tr("norm"), m_normWidget);
  addParameter("", apply);
}

CNormalizeWidget::~CNormalizeWidget()
{
}

void CNormalizeWidget::reset()
{
  m_alphaWidget->setValue(1);
  m_betaWidget->setValue(0);
  m_normWidget->setCurrentIndex(1);
}

void CNormalizeWidget::normalize()
{
  model()->setData(m_backup.clone());

  int norm = 0;
  if (m_normWidget->currentText() == "L1")
    {
      norm = cv::NORM_L1;
    }
  else if (m_normWidget->currentText() == "L2")
    {
      norm = cv::NORM_L2;
    }
  else if (m_normWidget->currentText() == "INF")
    {
      norm = cv::NORM_INF;
    }

  model()->normalize(m_alphaWidget->value(),
		     m_betaWidget->value(),
		     norm);
}

/*
  Transformations
*/

CTransformationsWidget::CTransformationsWidget(const QString & p_title,
					       CMatrixModel * p_model,
					       QWidget* p_parent) :
  COperationWidget(p_title, p_model, p_parent)
  , m_transposeWidget(new QPushButton(tr("Transpose"), this))
  , m_verticalFlipWidget(new QPushButton(tr("Vertical flip"), this))
  , m_horizontalFlipWidget(new QPushButton(tr("Horizontal flip"), this))
{
  addParameter("", m_transposeWidget);
  addParameter("", m_verticalFlipWidget);
  addParameter("", m_horizontalFlipWidget);

  connect(m_transposeWidget, SIGNAL(clicked()),
	  model(), SLOT(transpose()));

  connect(m_verticalFlipWidget, SIGNAL(clicked()),
	  model(), SLOT(verticalFlip()));

  connect(m_horizontalFlipWidget, SIGNAL(clicked()),
	  model(), SLOT(horizontalFlip()));
}

CTransformationsWidget::~CTransformationsWidget()
{
}

void CTransformationsWidget::reset()
{
}

/*
  Color map
*/

CColorMapWidget::CColorMapWidget(const QString & p_title,
				 CMatrixModel * p_model,
				 QWidget* p_parent) :
  COperationWidget(p_title, p_model, p_parent)
  , m_colorMapWidget(new QComboBox)
{
  m_colorMapWidget->addItem("NONE");
  m_colorMapWidget->addItem("AUTUMN");
  m_colorMapWidget->addItem("BONE");
  m_colorMapWidget->addItem("JET");
  m_colorMapWidget->addItem("WINTER");
  m_colorMapWidget->addItem("RAINBOW");
  m_colorMapWidget->addItem("OCEAN");
  m_colorMapWidget->addItem("SUMMER");
  m_colorMapWidget->addItem("SPRING");
  m_colorMapWidget->addItem("COOL");
  m_colorMapWidget->addItem("HSV");
  m_colorMapWidget->addItem("PINK");
  m_colorMapWidget->addItem("HOT");
  m_colorMapWidget->setCurrentIndex(0);

  addParameter(tr("color map"), m_colorMapWidget);

  connect(m_colorMapWidget, SIGNAL(currentIndexChanged(const QString &)),
	  this, SLOT(colorMap(const QString &)));
}

CColorMapWidget::~CColorMapWidget()
{
}

void CColorMapWidget::reset()
{
  m_colorMapWidget->setCurrentIndex(0);
}

void CColorMapWidget::colorMap(const QString & p_type)
{
  model()->setData(m_backup.clone());

  if (p_type == "NONE")
    return;

  int colorMap = 0;
  if (p_type == "AUTUMN")
    {
      colorMap = cv::COLORMAP_AUTUMN;
    }
  else if (p_type == "BONE")
    {
      colorMap = cv::COLORMAP_BONE;
    }
  else if (p_type == "JET")
    {
      colorMap = cv::COLORMAP_JET;
    }
  else if (p_type == "WINTER")
    {
      colorMap = cv::COLORMAP_WINTER;
    }
  else if (p_type == "RAINBOW")
    {
      colorMap = cv::COLORMAP_RAINBOW;
    }
  else if (p_type == "OCEAN")
    {
      colorMap = cv::COLORMAP_OCEAN;
    }
  else if (p_type == "SUMMER")
    {
      colorMap = cv::COLORMAP_SUMMER;
    }
  else if (p_type == "SPRING")
    {
      colorMap = cv::COLORMAP_SPRING;
    }
  else if (p_type == "COOL")
    {
      colorMap = cv::COLORMAP_COOL;
    }
  else if (p_type == "HSV")
    {
      colorMap = cv::COLORMAP_HSV;
    }
  else if (p_type == "PINK")
    {
      colorMap = cv::COLORMAP_PINK;
    }
  else if (p_type == "HOT")
    {
      colorMap = cv::COLORMAP_HOT;
    }

  model()->applyColorMap(colorMap);
}

/*
  Threshold
*/

CThresholdWidget::CThresholdWidget(const QString & p_title,
				   CMatrixModel * p_model,
				   QWidget* p_parent) :
  COperationWidget(p_title, p_model, p_parent)
  , m_thresholdValueWidget(new QDoubleSpinBox)
  , m_maxValueWidget(new QDoubleSpinBox)
  , m_typeWidget(new QComboBox)
  , m_otsuWidget(new QCheckBox)
{
  m_thresholdValueWidget->setRange(-DBL_MAX, DBL_MAX);
  m_maxValueWidget->setRange(-DBL_MAX, DBL_MAX);

  m_typeWidget->addItem("BINARY");
  m_typeWidget->addItem("BINARY_INV");
  m_typeWidget->addItem("TRUNC");
  m_typeWidget->addItem("TOZERO");
  m_typeWidget->addItem("TOZERO_INV");
  m_typeWidget->setCurrentIndex(0);

  QPushButton *apply = new QPushButton(tr("Apply"), this);
  connect(apply, SIGNAL(clicked()), this, SLOT(threshold()));

  addParameter(tr("threshold value"), m_thresholdValueWidget);
  addParameter(tr("max value"), m_maxValueWidget);
  addParameter(tr("type"), m_typeWidget);
  addParameter(tr("otsu"), m_otsuWidget);
  addParameter("", apply);
}

CThresholdWidget::~CThresholdWidget()
{
}

void CThresholdWidget::reset()
{
  m_thresholdValueWidget->setValue(0);
  m_maxValueWidget->setValue(255);
  m_typeWidget->setCurrentIndex(0);
  m_otsuWidget->setChecked(false);
}

void CThresholdWidget::threshold()
{
  model()->setData(m_backup.clone());

  int type = 0;
  if (m_typeWidget->currentText() == "BINARY")
    {
      type = cv::THRESH_BINARY;
    }
  else if (m_typeWidget->currentText() == "BINARY_INV")
    {
      type = cv::THRESH_BINARY_INV;
    }
  else if (m_typeWidget->currentText() == "TRUNC")
    {
      type = cv::THRESH_TRUNC;
    }
  else if (m_typeWidget->currentText() == "TOZERO")
    {
      type = cv::THRESH_TOZERO;
    }
  else if (m_typeWidget->currentText() == "TOZERO_INV")
    {
      type = cv::THRESH_TOZERO_INV;
    }

  if (m_otsuWidget->isChecked())
    {
      type += cv::THRESH_OTSU;
    }

  model()->threshold(m_thresholdValueWidget->value(),
		     m_maxValueWidget->value(),
		     type);
}

/*
  Matrix
*/

CMatrixWidget::CMatrixWidget(const QString & p_title,
			   CMatrixModel * p_model,
			   QWidget* p_parent) :
  COperationWidget(p_title, p_model, p_parent)
  , m_fileChooserWidget(new CFileChooser(this))
  , m_absDiffWidget(new QPushButton(tr("Absolute difference"), this))
  , m_multiplyElementsWidget(new QPushButton(tr("Element-wise multiply"), this))
  , m_multiplyMatrixWidget(new QPushButton(tr("Matrix multiply"), this))
{
  m_fileChooserWidget->setCaption(tr("Pick other data file"));
  m_fileChooserWidget->setFilter(tr("Data files (%1)").arg(CMainWindow::_filters.join(" ")));

  addParameter("", m_fileChooserWidget);
  addParameter("", m_absDiffWidget);
  addParameter("", m_multiplyElementsWidget);
  addParameter("", m_multiplyMatrixWidget);

  connect(m_absDiffWidget, SIGNAL(clicked()),
	  this, SLOT(absDiff()));

  connect(m_multiplyElementsWidget, SIGNAL(clicked()),
	  this, SLOT(multiplyElements()));

  connect(m_multiplyMatrixWidget, SIGNAL(clicked()),
	  this, SLOT(multiplyMatrix()));
}

CMatrixWidget::~CMatrixWidget()
{
}

void CMatrixWidget::reset()
{
  m_fileChooserWidget->setPath(QDir::homePath());
}

void CMatrixWidget::absDiff()
{
  if (m_fileChooserWidget->path().isEmpty())
    {
      qWarning() << tr("Empty filename");
      return;
    }

  model()->setData(m_backup.clone());
  CMatrixConverter converter(m_fileChooserWidget->path());
  model()->absdiff(converter.data());
}

void CMatrixWidget::multiplyElements()
{
  if (m_fileChooserWidget->path().isEmpty())
    {
      qWarning() << tr("Empty filename");
      return;
    }

  CMatrixConverter converter(m_fileChooserWidget->path());
  model()->multiplyElements(converter.data());
}

void CMatrixWidget::multiplyMatrix()
{
  if (m_fileChooserWidget->path().isEmpty())
    {
      qWarning() << tr("Empty filename");
      return;
    }

  CMatrixConverter converter(m_fileChooserWidget->path());
  model()->multiplyMatrix(converter.data());
}


