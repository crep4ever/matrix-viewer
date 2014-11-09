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

#ifndef __HISTOGRAM_HH__
#define __HISTOGRAM_HH__

#include <QWidget>
#include <QVector>
#include <QColor>

class QBoxLayout;
class QLabel;

class CHistogram : public QWidget
{
  Q_OBJECT

  public:
  CHistogram(const QColor & color, QWidget* parent = 0);
  virtual ~CHistogram();

  void setValues(const QVector<uint>& values);

private:
  void drawPixmap();
  void computeStats();
  QBoxLayout * makeAxisBar() const;

private:
  QColor m_color;
  QVector<uint> m_values;

  QLabel *m_pixmapLabel;
  QLabel * m_count;
  QLabel * m_min;
  QLabel * m_max;
  QLabel * m_mean;
  QLabel * m_standardDeviation;
};

#endif // __HISTOGRAM_HH__
