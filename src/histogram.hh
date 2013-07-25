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
#include <QPen>
#include <QBrush>
#include <QColor>

class QResizeEvent;
class QGraphicsView;
class QGraphicsScene;
class QBoxLayout;

class CHistogram : public QWidget
{
  Q_OBJECT

  public:
  CHistogram(QWidget* parent = 0);
  CHistogram(const QColor & color, QWidget* parent = 0);
  virtual ~CHistogram();

  void setValues(const QVector<int>& values,
		 const QPen & pen = QPen(),
		 const QBrush & brush = QBrush());

  QColor color() const;
  void setColor(const QColor & color);

  int count() const;
  int min() const;
  int max() const;
  double mean() const;
  double standardDeviation() const;

protected:
  virtual void resizeEvent(QResizeEvent * event);

private:
  void update();
  QBoxLayout * makeAxisBar() const;

private:
  QColor m_color;

  QGraphicsScene *m_scene;
  QGraphicsView *m_view;

  int m_count;
  int m_min;
  int m_max;
  double m_mean;
  double m_standardDeviation;
};

#endif // __HISTOGRAM_HH__
