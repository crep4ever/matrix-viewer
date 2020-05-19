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
#include <QComboBox>
#include <QCheckBox>
#include <QSettings>
#include <QDebug>

#include "main-window.hh"
#include "tab.hh"
#include "matrix-model.hh"
#include "file-chooser.hh"
#include "double-spinbox.hh"
#include "common-widgets.hh"


COperationWidget::COperationWidget(const QString & p_title, CMatrixModel * p_model, QWidget* p_parent) : QWidget(p_parent)
, m_parent(qobject_cast<CMainWindow*>(p_parent))
, m_wasModified(false)
, m_backup(p_model->clone())
, m_applyButton(new QPushButton(tr("Apply"), this))
, m_openPath(QDir::homePath())
, m_savePath(QDir::homePath())
, m_title(p_title)
, m_model(p_model)
, m_parametersLayout(new QFormLayout)
{
  readSettings();

  if (m_parent != nullptr && m_parent->currentWidget() != nullptr)
  {
    m_wasModified = m_parent->currentWidget()->isModified();
  }

  connect(m_applyButton, SIGNAL(clicked()), this, SLOT(apply()));

  QGroupBox *operationGroupBox = new QGroupBox(p_title);
  operationGroupBox->setLayout(m_parametersLayout);

  QBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(operationGroupBox);
  mainLayout->addWidget(m_applyButton);
  setLayout(mainLayout);
  adjustSize();
}

COperationWidget::~COperationWidget()
{
  delete m_backup;
}

const QString & COperationWidget::title() const
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

void COperationWidget::addParameter(const QString & p_label, QWidget * p_widget)
{
  m_parametersLayout->addRow(p_label, p_widget);
}


void COperationWidget::readSettings()
{
  QSettings settings;
  settings.beginGroup("general");
  m_openPath = settings.value("openPath", QDir::homePath()).toString();
  m_savePath = settings.value("savePath", QDir::homePath()).toString();
  settings.endGroup();
}

void COperationWidget::writeSettings()
{
  QSettings settings;
  settings.beginGroup( "general" );
  settings.setValue( "openPath", m_openPath );
  settings.setValue( "savePath", m_savePath );
  settings.endGroup();
}

void COperationWidget::reset()
{
  if (!m_wasModified && m_parent != nullptr && m_parent->currentWidget() != nullptr)
  {
    m_parent->currentWidget()->setModified(false);
  }
}

/*
Format
*/

CFormatWidget::CFormatWidget(const QString & p_title, CMatrixModel * p_model, QWidget* p_parent) :
COperationWidget(p_title, p_model, p_parent)
, m_typeWidget(new QComboBox)
, m_alphaWidget(new CDoubleSpinBox)
, m_betaWidget(new CDoubleSpinBox)
{
  m_alphaWidget->setValue(1);
  m_betaWidget->setValue(0);

  m_typeWidget->addItem("8U");
  m_typeWidget->addItem("8S");
  m_typeWidget->addItem("16U");
  m_typeWidget->addItem("16S");
  m_typeWidget->addItem("32S");
  m_typeWidget->addItem("32F");
  m_typeWidget->addItem("64F");
  m_typeWidget->setCurrentIndex(model()->type() % 8);

  addParameter(tr("data type"), m_typeWidget);
  addParameter(tr("scale"), m_alphaWidget);
  addParameter(tr("shift"), m_betaWidget);
}

CFormatWidget::~CFormatWidget()
{
}

void CFormatWidget::reset()
{
  m_typeWidget->setCurrentIndex(model()->type() % 8);
  m_alphaWidget->setValue(1);
  m_betaWidget->setValue(0);
  COperationWidget::reset();
}

void CFormatWidget::apply()
{
  model()->convertTo(m_typeWidget->currentIndex() + 8 * (model()->channels() - 1),
  m_alphaWidget->value(),
  m_betaWidget->value());
}


/*
Scalar operations
*/

CScalarWidget::CScalarWidget(const QString & p_title, CMatrixModel * p_model, QWidget* p_parent) :
COperationWidget(p_title, p_model, p_parent)
, m_addWidget(new CDoubleSpinBox)
, m_multiplyWidget(new CDoubleSpinBox)
{
  m_addWidget->setValue(0);
  m_multiplyWidget->setValue(1);

  addParameter(tr("add"), m_addWidget);
  addParameter(tr("multiply"), m_multiplyWidget);
}

CScalarWidget::~CScalarWidget()
{
}

void CScalarWidget::reset()
{
  m_addWidget->setValue(0);
  m_multiplyWidget->setValue(1);
  COperationWidget::reset();
}

void CScalarWidget::apply()
{
  model()->add(m_addWidget->value());
  model()->multiply(m_multiplyWidget->value());
}

/*
Rotation widget
*/

CRotationWidget::CRotationWidget(const QString & p_title, CMatrixModel * p_model, QWidget* p_parent) :
COperationWidget(p_title, p_model, p_parent)
, m_centerWidget(new CPoint2DWidget)
, m_angleWidget(new CDoubleSpinBox)
, m_scaleWidget(new CDoubleSpinBox)
{
  m_centerWidget->setPoint(model()->center());

  m_angleWidget->setRange(-360, 360);
  m_angleWidget->setValue(0);

  m_scaleWidget->setValue(1);

  addParameter(tr("center"), m_centerWidget);
  addParameter(tr("angle"), m_angleWidget);
  addParameter(tr("scale"), m_scaleWidget);
}

CRotationWidget::~CRotationWidget()
{
}

void CRotationWidget::reset()
{
  m_centerWidget->setPoint(model()->center());
  m_angleWidget->setValue(0);
  m_scaleWidget->setValue(1);
  COperationWidget::reset();
}

void CRotationWidget::apply()
{
  model()->setData(m_backup->data().clone());

  model()->rotate(m_centerWidget->point(),
  m_angleWidget->value(),
  m_scaleWidget->value());
}

/*
Normalize widget
*/

CNormalizeWidget::CNormalizeWidget(const QString & p_title, CMatrixModel * p_model, QWidget* p_parent) :
COperationWidget(p_title, p_model, p_parent)
, m_alphaWidget(new CDoubleSpinBox)
, m_betaWidget(new CDoubleSpinBox)
, m_normWidget(new QComboBox)
{
  m_alphaWidget->setValue(1);
  m_betaWidget->setValue(0);

  m_normWidget->addItem("L1");
  m_normWidget->addItem("L2");
  m_normWidget->addItem("INF");
  m_normWidget->setCurrentIndex(1);

  addParameter(tr("alpha"), m_alphaWidget);
  addParameter(tr("beta"), m_betaWidget);
  addParameter(tr("norm"), m_normWidget);
}

CNormalizeWidget::~CNormalizeWidget()
{
}

void CNormalizeWidget::reset()
{
  m_alphaWidget->setValue(1);
  m_betaWidget->setValue(0);
  m_normWidget->setCurrentIndex(1);
  COperationWidget::reset();
}

void CNormalizeWidget::apply()
{
  model()->setData(m_backup->data().clone());

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

CTransformationsWidget::CTransformationsWidget(const QString & p_title, CMatrixModel * p_model, QWidget* p_parent) :
COperationWidget(p_title, p_model, p_parent)
, m_transposeWidget(new QPushButton(tr("Transpose"), this))
, m_verticalFlipWidget(new QPushButton(tr("Vertical flip"), this))
, m_horizontalFlipWidget(new QPushButton(tr("Horizontal flip"), this))
, m_mulTransposeWidget(new QPushButton(tr("Multiply by transposed"), this))
{
  m_applyButton->hide();

  addParameter("", m_transposeWidget);
  addParameter("", m_verticalFlipWidget);
  addParameter("", m_horizontalFlipWidget);
  addParameter("", m_mulTransposeWidget);

  connect(m_transposeWidget, SIGNAL(clicked()),
  model(), SLOT(transpose()));

  connect(m_verticalFlipWidget, SIGNAL(clicked()),
  model(), SLOT(verticalFlip()));

  connect(m_horizontalFlipWidget, SIGNAL(clicked()),
  model(), SLOT(horizontalFlip()));

  connect(m_mulTransposeWidget, SIGNAL(clicked()),
  model(), SLOT(mulTranspose()));
}

CTransformationsWidget::~CTransformationsWidget()
{
}

void CTransformationsWidget::reset()
{
  COperationWidget::reset();
}

void CTransformationsWidget::apply()
{
}


/*
Color map
*/

CColorMapWidget::CColorMapWidget(const QString & p_title, CMatrixModel * p_model, QWidget* p_parent) :
COperationWidget(p_title, p_model, p_parent)
, m_rangeWidget(nullptr)
, m_colorMapWidget(new QComboBox)
{
  if (model()->channels() == 1)
  {
    m_rangeWidget = new CPoint2DWidget;
    m_rangeWidget->setLabels(tr("min"), tr("max"));

    double min = 0, max = 0;
    model()->minMaxLoc(&min, &max);
    m_rangeWidget->setPoint(QPointF(min, max));

    addParameter(tr("range"), m_rangeWidget);
  }

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
}

CColorMapWidget::~CColorMapWidget()
{
}

void CColorMapWidget::reset()
{
  m_colorMapWidget->setCurrentIndex(0);

  if (model()->channels() == 1)
  {
    double min = 0, max = 0;
    model()->minMaxLoc(&min, &max);
    m_rangeWidget->setPoint(QPointF(min, max));
  }

  COperationWidget::reset();
}

void CColorMapWidget::apply()
{
  #if (CV_MAJOR_VERSION == 2) and (CV_MINOR_VERSION <= 3)
  qWarning() << tr("applyColorMap requires a more recent version of OpenCV.");
  qWarning() << tr("Current OpenCV version: %1.%2").arg(QString::number(CV_MAJOR_VERSION)).arg(QString::number(CV_MINOR_VERSION));
  qWarning() << tr("Minimum required OpenCV version: 2.4");
  return;
  #else
  cv::Mat m = m_backup->data().clone();

  if (model()->channels() == 1)
  {
    const double min = m_rangeWidget->point().x();
    const double max = m_rangeWidget->point().y();

    cv::Mat minMask = m < min;
    m.setTo(min, minMask);

    cv::Mat maxMask = m > max;
    m.setTo(max, maxMask);

    cv::normalize(m, m, 0, 255, cv::NORM_MINMAX, CV_8U);
  }

  model()->setData(m);

  const QString type = m_colorMapWidget->currentText();

  if (type == "NONE")
  {
    return;
  }

  int colorMap = 0;
  if (type == "AUTUMN")
  {
    colorMap = cv::COLORMAP_AUTUMN;
  }
  else if (type == "BONE")
  {
    colorMap = cv::COLORMAP_BONE;
  }
  else if (type == "JET")
  {
    colorMap = cv::COLORMAP_JET;
  }
  else if (type == "WINTER")
  {
    colorMap = cv::COLORMAP_WINTER;
  }
  else if (type == "RAINBOW")
  {
    colorMap = cv::COLORMAP_RAINBOW;
  }
  else if (type == "OCEAN")
  {
    colorMap = cv::COLORMAP_OCEAN;
  }
  else if (type == "SUMMER")
  {
    colorMap = cv::COLORMAP_SUMMER;
  }
  else if (type == "SPRING")
  {
    colorMap = cv::COLORMAP_SPRING;
  }
  else if (type == "COOL")
  {
    colorMap = cv::COLORMAP_COOL;
  }
  else if (type == "HSV")
  {
    colorMap = cv::COLORMAP_HSV;
  }
  else if (type == "PINK")
  {
    colorMap = cv::COLORMAP_PINK;
  }
  else if (type == "HOT")
  {
    colorMap = cv::COLORMAP_HOT;
  }

  model()->applyColorMap(colorMap);
  #endif
}

/*
Threshold
*/

CThresholdWidget::CThresholdWidget(const QString & p_title, CMatrixModel * p_model, QWidget* p_parent) :
COperationWidget(p_title, p_model, p_parent)
, m_thresholdValueWidget(new CDoubleSpinBox)
, m_maxValueWidget(new CDoubleSpinBox)
, m_typeWidget(new QComboBox)
, m_otsuWidget(new QCheckBox)
{
  m_thresholdValueWidget->setValue(0);
  m_maxValueWidget->setValue(255);

  m_typeWidget->addItem("BINARY");
  m_typeWidget->addItem("BINARY_INV");
  m_typeWidget->addItem("TRUNC");
  m_typeWidget->addItem("TOZERO");
  m_typeWidget->addItem("TOZERO_INV");
  m_typeWidget->setCurrentIndex(0);

  m_otsuWidget->setChecked(false);

  addParameter(tr("threshold value"), m_thresholdValueWidget);
  addParameter(tr("max value"), m_maxValueWidget);
  addParameter(tr("type"), m_typeWidget);
  addParameter(tr("otsu"), m_otsuWidget);
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
  COperationWidget::reset();
}

void CThresholdWidget::apply()
{
  model()->setData(m_backup->data().clone());

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

CMatrixWidget::CMatrixWidget(const QString & p_title, CMatrixModel * p_model, QWidget* p_parent) :
COperationWidget(p_title, p_model, p_parent)
, m_fileChooserWidget(new CFileChooser(this))
, m_absDiffWidget(new QPushButton(tr("Absolute difference"), this))
, m_multiplyElementsWidget(new QPushButton(tr("Element-wise multiply"), this))
, m_multiplyMatrixWidget(new QPushButton(tr("Matrix multiply"), this))
{
  m_applyButton->hide();

  readSettings();

  m_fileChooserWidget->setCaption(tr("Pick other data file"));
  m_fileChooserWidget->setFilter(tr("%1").arg(CMainWindow::_fileTypeFilters.join(" ")));
  m_fileChooserWidget->setPath(m_openPath);

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
  m_fileChooserWidget->setPath(m_openPath);
}

void CMatrixWidget::reset()
{
  m_fileChooserWidget->setPath(m_openPath);
  COperationWidget::reset();
}

void CMatrixWidget::apply()
{
}

void CMatrixWidget::absDiff()
{
  if (m_fileChooserWidget->path().isEmpty())
  {
    qWarning() << tr("Empty filename");
    return;
  }

  model()->setData(m_backup->data().clone());
  CMatrixModel other(m_fileChooserWidget->path());
  model()->absdiff(other.data());

  m_openPath = m_fileChooserWidget->path();
  writeSettings(); // update openPath
}

void CMatrixWidget::multiplyElements()
{
  if (m_fileChooserWidget->path().isEmpty())
  {
    qWarning() << tr("Empty filename");
    return;
  }

  CMatrixModel other(m_fileChooserWidget->path());
  model()->multiplyElements(other.data());

  m_openPath = m_fileChooserWidget->path();
  writeSettings(); // update openPath
}

void CMatrixWidget::multiplyMatrix()
{
  if (m_fileChooserWidget->path().isEmpty())
  {
    qWarning() << tr("Empty filename");
    return;
  }

  CMatrixModel other(m_fileChooserWidget->path());
  model()->multiplyMatrix(other.data());

  m_openPath = m_fileChooserWidget->path();
  writeSettings(); // update openPath
}


/*
Channels
*/

CChannelsWidget::CChannelsWidget(const QString & p_title, CMatrixModel * p_model, QWidget* p_parent) :
COperationWidget(p_title, p_model, p_parent)
, m_redFileChooserWidget(new CFileChooser(this))
, m_greenFileChooserWidget(new CFileChooser(this))
, m_blueFileChooserWidget(new CFileChooser(this))
, m_redOpenPath(m_openPath)
, m_greenOpenPath(m_openPath)
, m_blueOpenPath(m_openPath)
{
  m_applyButton->setText(tr("Merge"));

  m_redFileChooserWidget->setCaption(tr("Select red channel"));
  m_redFileChooserWidget->setFilter(tr("%1").arg(CMainWindow::_fileTypeFilters.join(" ")));
  m_redFileChooserWidget->setPath(m_redOpenPath);

  m_greenFileChooserWidget->setCaption(tr("Select green channel"));
  m_greenFileChooserWidget->setFilter(tr("%1").arg(CMainWindow::_fileTypeFilters.join(" ")));
  m_greenFileChooserWidget->setPath(m_blueOpenPath);

  m_blueFileChooserWidget->setCaption(tr("Select blue channel"));
  m_blueFileChooserWidget->setFilter(tr("%1").arg(CMainWindow::_fileTypeFilters.join(" ")));
  m_blueFileChooserWidget->setPath(m_greenOpenPath);

  addParameter("Red", m_redFileChooserWidget);
  addParameter("Green", m_greenFileChooserWidget);
  addParameter("Blue", m_blueFileChooserWidget);
}

CChannelsWidget::~CChannelsWidget()
{
}

void CChannelsWidget::reset()
{
  m_redFileChooserWidget->setPath(m_redOpenPath);
  m_greenFileChooserWidget->setPath(m_greenOpenPath);
  m_blueFileChooserWidget->setPath(m_blueOpenPath);
  COperationWidget::reset();
}

void CChannelsWidget::apply()
{
  m_redOpenPath   = m_redFileChooserWidget->path();
  m_greenOpenPath = m_greenFileChooserWidget->path();
  m_blueOpenPath  = m_blueFileChooserWidget->path();

  if (m_redOpenPath.isEmpty()   ||
  m_greenOpenPath.isEmpty() ||
  m_blueOpenPath.isEmpty())
  {
    qWarning() << tr("Empty filenames");
    return;
  }

  model()->setData(m_backup->data().clone());

  QStringList paths;
  paths << m_blueOpenPath;
  paths << m_greenOpenPath;
  paths << m_redOpenPath;

  model()->merge(paths);

  m_openPath = m_redOpenPath;
  writeSettings(); // update openPath
}
