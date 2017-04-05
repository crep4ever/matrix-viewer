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

#pragma once

#include <QString>

#include "benchmark-result.hh"

class CMatrixModel;

class BenchmarkTask : public QObject
{
  Q_OBJECT
public:
  BenchmarkTask(const QString & p_operationName,
                const int p_nbIterations,
                CMatrixModel * p_model);

  ~BenchmarkTask();

  void execute();

signals:
  void resultReady(const BenchmarkResult & res);

public slots:
  void cancel();

private:
  QString m_name;
  int m_iterations;
  CMatrixModel *m_model;
  bool m_cancelRequested;
};