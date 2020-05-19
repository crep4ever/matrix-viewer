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

#pragma once

#include <QWidget>
#include <QPointF>

class QLabel;
class QDoubleSpinBox;

class CPoint2DWidget : public QWidget
{
  Q_OBJECT

public:
  CPoint2DWidget(QWidget * p_parent = nullptr);
  ~CPoint2DWidget() override;

  QPointF point() const;
  void setPoint(const QPointF & p_point);
  void setLabels(const QString & p_x,
		 const QString & p_y);

public slots:
  void updateXValue(double p_value);
  void updateYValue(double p_value);

private:
  QPointF m_point;
  QLabel *p_xLabel;
  QLabel *p_yLabel;
  QDoubleSpinBox *m_xInput;
  QDoubleSpinBox *m_yInput;
};
