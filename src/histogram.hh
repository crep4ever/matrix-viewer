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

#include <QGraphicsView>
#include <QVector>

class QResizeEvent;

class CHistogram : public QGraphicsView
{
  Q_OBJECT

  public:
  CHistogram(QWidget* parent = 0);
  virtual ~CHistogram();

  void setValues(const QVector<int>& values,
		 const QPen & pen = QPen(),
		 const QBrush & brush = QBrush());

protected:
  virtual void resizeEvent(QResizeEvent * event);

private:
  QGraphicsScene *m_scene;
};

#endif // __HISTOGRAM_HH__
