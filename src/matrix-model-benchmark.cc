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

#include "matrix-model.hh"

#include <QStringList>
#include <QElapsedTimer>
#include <QDebug>

QStringList CMatrixModel::benchmark_operations()
{
  QStringList res;
  res << "total";
  res << "countNonZeros";
  res << "minMaxLoc";
  res << "meanStdDev";
  res << "add";
  res << "multiply";
  res << "transpose";
  res << "verticalFlip";
  res << "horizontalFlip";
  res << "rotate";
  res << "normalize";
  return res;
}

BenchmarkResult CMatrixModel::benchmark(const QString & p_name,
					const int p_iterations)

{
  blockSignals(true);
  cv::Mat ref    = m_data.clone();
  cv::Mat backup = m_data.clone();
  
  BenchmarkResult result;

  QElapsedTimer timer;

  qint64 ns = 0;
  double min =  DBL_MAX;
  double max = -DBL_MAX;
  double sum = 0;
  for (int i = 0; i < p_iterations; ++i)
    {
      if (p_name == "total")
	{
	  timer.start();
	  total();
	  ns = timer.nsecsElapsed();
	}
      else if (p_name == "countNonZeros")	
	{
	  timer.start();
	  Q_UNUSED(countNonZeros());
	  ns = timer.nsecsElapsed();
	}
      else if (p_name == "minMaxLoc")	
	{
	  timer.start();
	  double minVal, maxVal;
	  cv::Point  minLoc, maxLoc;
	  minMaxLoc(&minVal, &maxVal,
		    &minLoc, &maxLoc);
	  ns = timer.nsecsElapsed();
	}
      else if (p_name == "meanStdDev")
	{
	  timer.start();
	  double mean, stdDev;
	  meanStdDev(&mean, &stdDev);
	  ns = timer.nsecsElapsed();
	}
      else if (p_name == "add")
	{
	  timer.start();
	  add(1);
	  ns = timer.nsecsElapsed();
	  m_data = backup.clone();
	}
      else if (p_name == "multiply")
	{
	  timer.start();
	  multiply(2);
	  ns = timer.nsecsElapsed();
	  m_data = backup.clone();
	}
      else if (p_name == "transpose")
	{
	  timer.start();
	  transpose();
	  ns = timer.nsecsElapsed();
	  m_data = backup.clone();
	}
      else if (p_name == "verticalFlip")
	{
	  timer.start();
	  verticalFlip();
	  ns = timer.nsecsElapsed();
	  m_data = backup.clone();
	}
      else if (p_name == "horizontalFlip")
	{
	  timer.start();
	  horizontalFlip();
	  ns = timer.nsecsElapsed();
	  m_data = backup.clone();
	}
      else if (p_name == "rotate")
	{
	  timer.start();
	  const float x = m_data.cols / 2.0;
	  const float y = m_data.rows / 2.0;
	  rotate(cv::Point(x, y), 60, 1);
	  ns = timer.nsecsElapsed();
	  m_data = backup.clone();
	}
      else if (p_name == "normalize")
	{
	  timer.start();
	  normalize(1, 0, cv::NORM_L2);
	  ns = timer.nsecsElapsed();
	  m_data = backup.clone();
	}
      else
	{
	  qWarning() << "unsupported operation " << p_name;
	  blockSignals(false);
	  return BenchmarkResult();
	}

      if (ns < min) min = ns;
      if (ns > max) max = ns;
      sum += ns;
    }

  // Check data integrity
  cv::Mat diff;
  cv::absdiff(ref, m_data, diff);
  if (cv::sum(diff)[0] != 0)
    {
      qWarning() << p_name << " benchmark has modified current data";
    }
  
  result.m_nsMin = min;
  result.m_nsMax = max;
  result.m_nsAvg = sum / (double) p_iterations;

  blockSignals(false);
  return result;
}
