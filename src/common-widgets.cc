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

#include "common-widgets.hh"

#include <QBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QDebug>

#include <opencv2/opencv.hpp>

/*
  Point2D widget
*/

CPoint2DWidget::CPoint2DWidget(QWidget * p_parent) :
  QWidget(p_parent)
  , m_point(QPointF(0, 0))
  , p_xLabel(new QLabel("x"))
  , p_yLabel(new QLabel("y"))
{
  m_xInput = new QDoubleSpinBox;
  m_xInput->setRange(-DBL_MAX, DBL_MAX);
  m_xInput->setDecimals(4);
  connect(m_xInput, SIGNAL(valueChanged(double)),
	  this, SLOT(updateXValue(double)));

  m_yInput = new QDoubleSpinBox;
  m_yInput->setRange(-DBL_MAX, DBL_MAX);
  m_yInput->setDecimals(4);
  connect(m_yInput, SIGNAL(valueChanged(double)),
	  this, SLOT(updateYValue(double)));

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addStretch();
  layout->addWidget(p_xLabel);
  layout->addWidget(m_xInput);
  layout->addWidget(p_yLabel);
  layout->addWidget(m_yInput);
  layout->setContentsMargins(0, 0, 0, 0);

  setLayout(layout);
}

CPoint2DWidget::~CPoint2DWidget()
{
}

QPointF CPoint2DWidget::point() const
{
  return m_point;
}

void CPoint2DWidget::setPoint(const QPointF & p_point)
{
  if (m_point != p_point)
    {
      m_point = p_point;
      m_xInput->setValue(m_point.x());
      m_yInput->setValue(m_point.y());
    }
}

void CPoint2DWidget::updateXValue(double p_value)
{
  m_point.setX(p_value);
}

void CPoint2DWidget::updateYValue(double p_value)
{
  m_point.setY(p_value);
}

void CPoint2DWidget::setLabels(const QString & p_x,
			       const QString & p_y)
{
  p_xLabel->setText(p_x);
  p_yLabel->setText(p_y);
}

