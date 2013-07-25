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

#include "histogram.hh"

#include <QtCore/qmath.h>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QBoxLayout>
#include <QLabel>
#include <QDebug>

CHistogram::CHistogram(QWidget* parent) :
  QWidget(parent)
  , m_color(Qt::black)
  , m_scene(new QGraphicsScene)
  , m_view(new QGraphicsView)
  , m_count(0)
  , m_min(255)
  , m_max(0)
  , m_mean(0)
  , m_standardDeviation(0)
{
}

CHistogram::CHistogram(const QColor & color, QWidget* parent) :
  QWidget(parent)
  , m_color(color)
  , m_scene(new QGraphicsScene)
  , m_view(new QGraphicsView)
  , m_count(0)
  , m_min(255)
  , m_max(0)
  , m_mean(0)
  , m_standardDeviation(0)
{
}

CHistogram::~CHistogram()
{
}

void CHistogram::update()
{
  m_view->setBackgroundRole(QPalette::Light);
  m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_view->setScene(m_scene);

  QBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(m_view);
  vlayout1->addLayout(makeAxisBar());
  vlayout1->addStretch();

  QBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addWidget(new QLabel(tr("Min: %1").arg(m_min)));
  vlayout2->addWidget(new QLabel(tr("Max: %1").arg(m_max)));
  vlayout2->addWidget(new QLabel(tr("Mean: %1").arg(m_mean)));
  vlayout2->addStretch();

  QBoxLayout *vlayout3 = new QVBoxLayout;
  vlayout3->addWidget(new QLabel(tr("Count: %1").arg(m_count)));
  vlayout3->addWidget(new QLabel(tr("StdDev: %1").arg(m_standardDeviation)));
  vlayout3->addStretch();

  QBoxLayout *layout = new QHBoxLayout;
  layout->addLayout(vlayout1);
  layout->addLayout(vlayout2);
  layout->addLayout(vlayout3);

  setLayout(layout);
}

QColor CHistogram::color() const
{
  return m_color;
}

void CHistogram::setColor(const QColor & color)
{
  m_color = color;
}

void CHistogram::setValues(const QVector<int>& values,
			   const QPen & pen,
			   const QBrush & brush)
{
  uint squareSum = 0;
  uint sum = 0;

  for (int i = 0; i < values.size(); ++i)
    {
      if (values[i] > 0)
	{
	  m_scene->addRect(QRect(i, 0, 1, -values[i]), pen, brush);
	  sum += i * values[i];
	  squareSum += i * i * values[i];
	}

      m_min = qMin(m_min, i);
      m_max = qMax(m_max, i);
      m_count += values[i];
    }

  m_mean = sum / (double) m_count;
  const double meanSquare = squareSum / (double) m_count;
  m_standardDeviation = qSqrt(qAbs(meanSquare - m_mean * m_mean));

  update();
}

QBoxLayout * CHistogram::makeAxisBar() const
{
  QString css = QString("QLabel{background-color: "
			"qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, "
			"stop: 0 black, stop: 1 %1);}").arg(m_color.name());

  QLabel *begin = new QLabel("0");
  QLabel *end = new QLabel("255");

  QLabel *gradient = new QLabel;
  gradient->setStyleSheet(css);

  QBoxLayout * layout = new QHBoxLayout;
  layout->addWidget(begin);
  layout->addWidget(gradient, 1);
  layout->addWidget(end);

  return layout;
}

void CHistogram::resizeEvent(QResizeEvent * event)
{
  QWidget::resizeEvent(event);
  m_view->fitInView(m_scene->sceneRect());
}

int CHistogram::count() const
{
  return m_count;
}

int CHistogram::min() const
{
  return m_min;
}

int CHistogram::max() const
{
  return m_max;
}

double CHistogram::mean() const
{
  return m_mean;
}

double CHistogram::standardDeviation() const
{
  return m_standardDeviation;
}
