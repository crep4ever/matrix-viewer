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

#include "benchmark-thread.hh"

#include <QStringList>
#include <QDebug>
#include <QApplication>

#include "matrix-model.hh"
#include "elapsed-timer.hh"

BenchmarkThread::BenchmarkThread(const QString & p_name,
				 const int p_iterations,
				 CMatrixModel* p_model)
  : QObject()
  , m_name(p_name)
  , m_iterations(p_iterations)
  , m_model(p_model)
  , m_cancelRequested(false)
{
}

BenchmarkThread::~BenchmarkThread()
{
}

void BenchmarkThread::run()

{
  if (!m_model)
    return;

  m_cancelRequested = false;

  cv::Mat ref    = m_model->data().clone();
  cv::Mat backup = m_model->data().clone();

  BenchmarkResult result(m_name);

  CElapsedTimer timer;

  qint64 ns = 0;
  double min =  DBL_MAX;
  double max = -DBL_MAX;
  double sum = 0;
  for (int i = 0; i < m_iterations; ++i)
    {
      // check for cancel signals from benchmark-dialog
      QApplication::processEvents();

      if (m_cancelRequested)
	{
	  result.setStatus(BenchmarkResult::Canceled);
	  emit resultReady(result);
	  return;
	}

      if (m_name == "total")
	{
	  timer.start();
	  m_model->total();
	  ns = timer.nsecsElapsed();
	}
      else if (m_name == "countNonZeros")
	{
	  timer.start();
	  Q_UNUSED(m_model->countNonZeros());
	  ns = timer.nsecsElapsed();
	}
      else if (m_name == "minMaxLoc")
	{
	  timer.start();
	  double minVal, maxVal;
	  cv::Point  minLoc, maxLoc;
	  m_model->minMaxLoc(&minVal, &maxVal,
		    &minLoc, &maxLoc);
	  ns = timer.nsecsElapsed();
	}
      else if (m_name == "meanStdDev")
	{
	  timer.start();
	  double mean, stdDev;
	  m_model->meanStdDev(&mean, &stdDev);
	  ns = timer.nsecsElapsed();
	}
      else if (m_name == "add")
	{
	  timer.start();
	  m_model->add(1);
	  ns = timer.nsecsElapsed();
	  m_model->setData(backup.clone());
	}
      else if (m_name == "multiply")
	{
	  timer.start();
	  m_model->multiply(2);
	  ns = timer.nsecsElapsed();
	  m_model->setData(backup.clone());
	}
      else if (m_name == "transpose")
	{
	  timer.start();
	  m_model->transpose();
	  ns = timer.nsecsElapsed();
	  m_model->setData(backup.clone());
	}
      else if (m_name == "verticalFlip")
	{
	  timer.start();
	  m_model->verticalFlip();
	  ns = timer.nsecsElapsed();
	  m_model->setData(backup.clone());
	}
      else if (m_name == "horizontalFlip")
	{
	  timer.start();
	  m_model->horizontalFlip();
	  ns = timer.nsecsElapsed();
	  m_model->setData(backup.clone());
	}
      else if (m_name == "rotate")
	{
	  timer.start();
	  const float x = (float) (m_model->columnCount() / 2.0);
	  const float y = (float) (m_model->rowCount() / 2.0);
	  m_model->rotate(cv::Point2f(x, y), 60, 1);
	  ns = timer.nsecsElapsed();
	  m_model->setData(backup.clone());
	}
      else if (m_name == "normalize")
	{
	  timer.start();
	  m_model->normalize(1, 0, cv::NORM_L2);
	  ns = timer.nsecsElapsed();
	  m_model->setData(backup.clone());
	}
      else if (m_name == "mulTranspose")
	{
	  timer.start();
	  m_model->mulTranspose();
	  ns = timer.nsecsElapsed();
	  m_model->setData(backup.clone());
	}
      else
	{
	  qWarning() << "unsupported operation " << m_name;
	  result.setStatus(BenchmarkResult::Ignored);
	  emit resultReady(result);
	  return;
	}

      if (ns < min) min = ns;
      if (ns > max) max = ns;
      sum += ns;
    }

  // Check data integrity
  cv::Mat diff;
  cv::absdiff(ref, m_model->data(), diff);
  if (cv::sum(diff)[0] != 0)
    {
      qWarning() << m_name << " benchmark has modified current data";
    }

  result.setNsMin(min);
  result.setNsMax(max);
  result.setNsAvg(sum / (double) m_iterations);
  result.setStatus(BenchmarkResult::Success);

  emit resultReady(result);
}

void BenchmarkThread::cancel()
{
  m_cancelRequested = true;
}
