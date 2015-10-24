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

#ifndef __BENCHMARK_RESULT_HH__
#define __BENCHMARK_RESULT_HH__

#include <QString>
#include <QDebug>


struct BenchmarkResult
{
  BenchmarkResult() : m_nsMin(0), m_nsMax(0), m_nsAvg(0){};
  ~BenchmarkResult(){};

  QString toString() const
  {
    QString unit = "ns";
    double min = m_nsMin;
    double max = m_nsMax;
    double avg = m_nsAvg;

    const double us = 1000;
    const double ms = us * 1000;
    const double s  = ms * 1000;
    if (min > s)
      {
	min /= s;
	max /= s;
	avg /= s;
	unit = "s";
      }
    else if (min > ms)
      {
	min /= ms;
	max /= ms;
	avg /= ms;
	unit = "ms";
      }
    else if (min > us)
      {
	min /= us;
	max /= us;
	avg /= us;
	unit = "µs";
      }
    
    return QString("avg: %1 %4\nmin: %2 %4\nmax: %3 %4\n")
      .arg(avg)
      .arg(min)
      .arg(max)
      .arg(unit);
  }
  
  double m_nsMin;
  double m_nsMax;
  double m_nsAvg;
};

#endif
