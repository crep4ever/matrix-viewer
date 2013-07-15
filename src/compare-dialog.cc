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
#include "compare-dialog.hh"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QSpinBox>
#include <QSettings>
#include <QFormLayout>
#include <QBoxLayout>
#include <QSettings>
#include <QStatusBar>
#include <QDebug>

#include "file-chooser.hh"
#include "matrix-converter.hh"
#include "matrix-model.hh"
#include "matrix-view.hh"
#include "image-view.hh"
#include "main-window.hh"
#include "tab.hh"
#include "delegate.hh"


CCompareDialog::CCompareDialog(QWidget *parent)
  : QDialog(parent)
  , m_parent(qobject_cast<CMainWindow*>(parent))
  , m_fileChooser(new CFileChooser)
  , m_absoluteThresholdSpinBox(new QSpinBox)
  , m_percentageThresholdSpinBox(new QSpinBox)
{
  setWindowTitle(tr("Compare matrix"));
  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close);
  connect(buttons, SIGNAL(accepted()), this, SLOT(compare()));
  connect(buttons, SIGNAL(rejected()), this, SLOT(close()));

  m_fileChooser->setCaption(tr("Open comparison data file"));
  m_fileChooser->setFilter(tr("Data files (*.xml *.txt *.bmp *.jpg *.png *.raw)"));

  QFormLayout *parametersLayout = new QFormLayout;
  parametersLayout->addRow(tr("Absolute threshold:"), m_absoluteThresholdSpinBox);
  parametersLayout->addRow(tr("Percentage threshold:"), m_percentageThresholdSpinBox);

  QGroupBox *compareParametersGroupBox = new QGroupBox(tr("Thresholds"));
  compareParametersGroupBox->setLayout(parametersLayout);

  QBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(m_fileChooser);
  layout->addWidget(compareParametersGroupBox);
  layout->addWidget(buttons);

  setLayout(layout);
  setMinimumSize(400, 60);
  readSettings();
}

CCompareDialog::~CCompareDialog()
{
  writeSettings();
}


void CCompareDialog::readSettings()
{
  QSettings settings;
  settings.beginGroup("compare");
  m_fileChooser->setPath(settings.value("path", QDir::homePath()).toString());
  m_absoluteThresholdSpinBox->setValue(settings.value("absoluteThreshold", true).toDouble());
  m_percentageThresholdSpinBox->setValue(settings.value("percentageThreshold", true).toDouble());
  settings.endGroup();
}

void CCompareDialog::writeSettings()
{
  QSettings settings;
  settings.beginGroup("compare");
  settings.setValue("path", m_fileChooser->path());
  settings.setValue("absoluteThreshold", m_absoluteThresholdSpinBox->value());
  settings.setValue("percentageThreshold", m_percentageThresholdSpinBox->value());
  settings.endGroup();
}

bool CCompareDialog::checkData(const cv::Mat & first,
			       const cv::Mat & second)
{
  if (first.rows == 0 || first.cols == 0)
    {
      parent()->statusBar()->showMessage(tr("Empty matrix cannot be compared."));
      return false;
    }

  if (first.rows != second.rows || first.cols != second.cols)
    {
      parent()->statusBar()->showMessage(tr("Different sizes: M1: %1 x %2 M2: %3 x %4")
					 .arg(first.rows).arg(first.cols)
					 .arg(second.rows).arg(second.cols));
      return false;
    }

  return true;
}

void CCompareDialog::compare()
{
  if (m_fileChooser->path().isEmpty())
    {
      qWarning() << tr("Empty filename");
      return;
    }

  CMatrixConverter converter(m_fileChooser->path());
  cv::Mat originalData = parent()->currentData();
  cv::Mat newData = converter.data();

  if (!checkData(originalData, converter.data()))
    {
      close();
      return;
    }
  writeSettings(); //updates thresholds for delegate

  // Diff model/view
  cv::Mat diffData;
  cv::absdiff(originalData, newData, diffData);

  CMatrixModel *diffModel = new CMatrixModel();
  diffModel->setData(diffData);

  CMatrixView *diffView = new CMatrixView(parent());
  diffView->setModel(diffModel);
  diffView->setItemDelegate(new CDelegate);
  parent()->currentWidget()->addWidget(diffView);

  CImageView *imageDiffView = new CImageView(parent());
  imageDiffView->setModel(diffModel);
  parent()->currentWidget()->addWidget(imageDiffView);

  accept();
}

CMainWindow* CCompareDialog::parent() const
{
  if (!m_parent)
    qWarning() << "CCompareDialog::parent invalid parent";
  return m_parent;
}



