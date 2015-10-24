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

#ifndef __BENCHMARK_DIALOG_HH__
#define __BENCHMARK_DIALOG_HH__

#include <QDialog>
#include <QList>
#include <QDebug>

#include "benchmark-result.hh"

class QTabWidget;
class QCheckBox;
class QSpinBox;
class QProgressBar;
class QTextEdit;

class CMainWindow;
class CMatrixModel;

/*!
  \file  benchmark-dialog.hh
  \class CBenchmarkDialog
  \brief CBenchmarkDialog run OpenCV operations benchmarks
*/

class CBenchmarkDialog : public QDialog
{
  Q_OBJECT

public:
  /// Constructor.
  CBenchmarkDialog(QWidget *parent = 0);

  /// Destructor.
  virtual ~CBenchmarkDialog();

  CMainWindow* parent() const;
  CMatrixModel* model() const;

public slots:

  void run();

  void selectAll();
  void unselectAll();

private slots:
  void updateProgressRange();

private:

  void addHeaderInfo();
  int countOperations() const;
  
  CMainWindow *m_parent;
  QTabWidget *m_tabs;
  QSpinBox *m_iterations;
  QList<QCheckBox*> m_operations;
  QProgressBar *m_progressBar;
  QList<BenchmarkResult> m_results;
  QTextEdit *m_report;
};

#endif  // __BENCHMARK_DIALOG_HH__
