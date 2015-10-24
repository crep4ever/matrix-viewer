// Copyright (C) 2015, Romain Goffe <romain.goffe@gmail.com>
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
#include "benchmark-dialog.hh"

#include <unistd.h>

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QAction>
#include <QSpinBox>
#include <QCheckBox>
#include <QScrollArea>
#include <QSettings>
#include <QBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QSysInfo>
#include <QMutexLocker>
#include <QDebug>

#include "main-window.hh"
#include "tab.hh"
#include "matrix-model.hh"
#include "progress-bar.hh"
#include "benchmark-thread.hh"

CBenchmarkDialog::CBenchmarkDialog(QWidget *parent)
  : QDialog(parent)
  , m_parent(qobject_cast<CMainWindow*>(parent))
  , m_tabs(new QTabWidget)
  , m_progressBar(new CProgressBar)
  , m_operations()
  , m_iterations(new QSpinBox)
  , m_report(new QTextEdit)
  , m_savePath(QDir::homePath())
  , m_cancelRequested(false)
  , m_progress(0)
{
  setWindowTitle(tr("Benchmark"));
  readSettings();

  // -----------------------------------------
  // Operations tab
  // -----------------------------------------

  // Options
  m_iterations->setValue(10);
  m_iterations->setMinimum(1);
  m_iterations->setMaximum(1000);

  QFormLayout *parametersLayout = new QFormLayout;
  parametersLayout->addRow(tr("Iterations:"), m_iterations);

  // Checkbox list of operations
  const QStringList names = model()->benchmark_operations();

  QVBoxLayout *namesLayout = new QVBoxLayout;
  foreach(const QString & name, names)
    {
      QCheckBox *item = new QCheckBox(name);
      item->setChecked(true);
      connect(item, SIGNAL(clicked()), this, SLOT(updateProgressRange()));
      m_operations.append(item);
      namesLayout->addWidget(item);
    }

  QWidget *checkboxContainer = new QWidget(this);
  checkboxContainer->setLayout(namesLayout);

  QScrollArea *scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  scrollArea->setWidget(checkboxContainer);

  // Progress bar
  updateProgressRange();
  connect(m_progressBar, SIGNAL(canceled()), this, SLOT(cancel()));

  // Context menu actions
  scrollArea->setContextMenuPolicy(Qt::ActionsContextMenu);

  QAction* action = new QAction(tr("Select all"), this);
  connect(action, SIGNAL(triggered()), this, SLOT(selectAll()));
  scrollArea->addAction(action);

  action = new QAction(tr("Unselect all"), this);
  connect(action, SIGNAL(triggered()), this, SLOT(unselectAll()));
  scrollArea->addAction(action);

  // Actions
  QPushButton *runButton = new QPushButton(tr("&Run"));
  runButton->setDefault(true);
  connect(runButton, SIGNAL(clicked()), this, SLOT(run()));

  QPushButton *exportButton = new QPushButton(tr("&Export"));
  connect(exportButton, SIGNAL(clicked()), this, SLOT(save()));

  QDialogButtonBox *buttons = new QDialogButtonBox;
  buttons->addButton(exportButton, QDialogButtonBox::ActionRole);
  buttons->addButton(runButton, QDialogButtonBox::ApplyRole);

  QBoxLayout *operationsLayout = new QVBoxLayout;
  operationsLayout->addLayout(parametersLayout);
  operationsLayout->addWidget(scrollArea);

  QWidget *operationsTab = new QWidget;
  operationsTab->setLayout(operationsLayout);

  // -----------------------------------------
  // Report tab
  // -----------------------------------------

  m_report->setReadOnly(true);

  QBoxLayout *reportLayout = new QVBoxLayout;
  reportLayout->addWidget(m_report);

  QWidget *reportTab = new QWidget;
  reportTab->setLayout(reportLayout);


  // -----------------------------------------
  // Dialog
  // -----------------------------------------

  m_tabs->addTab(operationsTab, tr("Operations"));
  m_tabs->addTab(reportTab, tr("Report"));

  QBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(m_tabs);
  mainLayout->addWidget(m_progressBar);
  mainLayout->addWidget(buttons);

  setLayout(mainLayout);
  resize(600, 500);
}

CBenchmarkDialog::~CBenchmarkDialog()
{
}

void CBenchmarkDialog::readSettings()
{
  QSettings settings;
  settings.beginGroup("general");
  m_savePath = settings.value("savePath", QDir::homePath()).toString();
  settings.endGroup();
}

void CBenchmarkDialog::writeSettings()
{
  QSettings settings;
  settings.beginGroup( "general" );
  settings.setValue( "savePath", m_savePath );
  settings.endGroup();
}


CMainWindow* CBenchmarkDialog::parent() const
{
  if (!m_parent)
    qWarning() << "CBenchmarkDialog::parent invalid parent";
  return m_parent;
}

CMatrixModel* CBenchmarkDialog::model() const
{
  return parent()->currentModel();
}

void CBenchmarkDialog::run()
{
  m_cancelRequested = false;

  m_tabs->setCurrentIndex(1);

  m_progressBar->reset();

  m_report->clear();
  addHeaderInfo();

  m_progress = 0;

  // Ensure that dataChanged is not emitted
  // as it would update matrix and image views
  model()->blockSignals(true);

  foreach (QCheckBox *checkBox, m_operations)
    {
      if (!m_cancelRequested && checkBox->isChecked())
	{
	  BenchmarkThread worker(checkBox->text(),
				 m_iterations->value(),
				 model());

	  connect(&worker, SIGNAL(resultReady(const BenchmarkResult &)),
		  this, SLOT(handleResult(const BenchmarkResult &)));

	  connect(m_progressBar, SIGNAL(canceled()),
		  &worker, SLOT(cancel()));

	  worker.run();
	}
    }

  // Restore normal signal use
  model()->blockSignals(false);
  parent()->currentWidget()->setModified(false);
}


void CBenchmarkDialog::handleResult(const BenchmarkResult & p_result)
{
  m_progressBar->setValue(++m_progress);

  m_report->append(QString("<b>%1 (%2)</b>")
		   .arg(p_result.title())
		   .arg(p_result.statusStr()));

  m_report->append(p_result.timeStr());
}

void CBenchmarkDialog::cancel()
{
  m_cancelRequested = true;
}

void CBenchmarkDialog::selectAll()
{
  foreach (QCheckBox *checkBox, m_operations)
    {
      checkBox->setChecked(true);
    }

  updateProgressRange();
}

void CBenchmarkDialog::unselectAll()
{
  foreach (QCheckBox *checkBox, m_operations)
    {
      checkBox->setChecked(false);
    }

  updateProgressRange();
}

int CBenchmarkDialog::countOperations() const
{
  int count = 0;
  foreach (QCheckBox *checkBox, m_operations)
    {
      if (checkBox->isChecked())
	++count;
    }

  return count;
}

void CBenchmarkDialog::updateProgressRange()
{
  const int count = countOperations();

  m_progressBar->reset();
  if (count == 0) // avoid k2000
    {
      m_progressBar->setMinimum(1);
      m_progressBar->setMaximum(1);
    }
  else
    {
      m_progressBar->setMinimum(0);
      m_progressBar->setMaximum(count);
    }
}

void CBenchmarkDialog::addHeaderInfo()
{
  QString rule = QString("-----------------------------------------------------------");

  m_report->append(rule);

  QString cvInfo = tr("OpenCV: %1.%2.%3")
    .arg(QString::number(CV_MAJOR_VERSION))
    .arg(QString::number(CV_MINOR_VERSION))
    .arg(QString::number(CV_SUBMINOR_VERSION));;

  m_report->append(cvInfo);

  QString modelInfo = tr("Matrix: %1 x %2 %3C%4")
    .arg(QString::number(model()->rowCount()))
    .arg(QString::number(model()->columnCount()))
    .arg(model()->typeString())
    .arg(QString::number(model()->channels()));

  m_report->append(modelInfo);

  QString benchmarkInfo = tr("Benchmark: %1 operations with %2 iterations")
    .arg(QString::number(countOperations()))
    .arg(QString::number(m_iterations->value()));

  m_report->append(benchmarkInfo);

  m_report->append(rule);
}


void CBenchmarkDialog::save()
{
  QString filename = QFileDialog::getSaveFileName(this,
                                                  tr("Save benchmark report"),
                                                  m_savePath,
                                                  tr("Data files (*.txt *.html)"));
  QFileInfo fi(filename);
  if (filename.isEmpty())
    return;

  m_savePath = fi.absolutePath();
  QFile data(filename);
  if (data.open(QFile::WriteOnly | QFile::Truncate))
    {
      QTextStream out(&data);
      if (fi.completeSuffix() == "html")
	{
	  out << m_report->toHtml();
	}
      else
	{
	  out << m_report->toPlainText();
	}
    }

  writeSettings(); //update savePath
}


