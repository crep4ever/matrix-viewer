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

#include <QObject>
#include <QMetaType>
#include <QString>
#include <QDebug>

class BenchmarkResult : public QObject
{
  Q_OBJECT
public:

  enum Status
    {
      Ignored = 0,
      Canceled,
      Error,
      Success
    };

  BenchmarkResult();
  BenchmarkResult(const QString & p_name);
  BenchmarkResult(const BenchmarkResult & p_other);
  ~BenchmarkResult() override;

  const QString & title() const;
  void setTitle(const QString & p_str);

  double nsMin() const;
  void setNsMin(const double p_value);

  double nsMax() const;
  void setNsMax(const double p_value);

  double nsAvg() const;
  void setNsAvg(const double p_value);

  Status status() const;
  void setStatus(const Status s);
  QString statusStr() const;

  QString timeStr() const;

private:
  QString m_title;
  double m_nsMin;
  double m_nsMax;
  double m_nsAvg;
  Status m_status;
};

Q_DECLARE_METATYPE(BenchmarkResult)
