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

/*
  Point2D widget
*/

CPoint2DWidget::CPoint2DWidget(QWidget * p_parent) :
  QWidget(p_parent)
  , m_point(QPointF(0, 0))
{
  m_xinput = new QDoubleSpinBox;
  m_xinput->setRange(0, 100000);
  connect(m_xinput, SIGNAL(valueChanged(double)),
	  this, SLOT(updateXValue(double)));

  m_yinput = new QDoubleSpinBox;
  m_yinput->setRange(0, 100000);
  connect(m_yinput, SIGNAL(valueChanged(double)),
	  this, SLOT(updateYValue(double)));

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addStretch();
  layout->addWidget(new QLabel("x"));
  layout->addWidget(m_xinput);
  layout->addWidget(new QLabel("y"));
  layout->addWidget(m_yinput);
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
      m_xinput->setValue(m_point.x());
      m_yinput->setValue(m_point.y());
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

