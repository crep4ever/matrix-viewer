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
#include <QPixmap>
#include <QPainter>
#include <QBoxLayout>
#include <QLabel>
#include <QDebug>

CHistogram::CHistogram(const QColor & color, QWidget* parent) :
QWidget(parent)
, m_color(color)
, m_values()
, m_pixmapLabel(new QLabel(this))
, m_count(new QLabel(this))
, m_min(new QLabel(this))
, m_max(new QLabel(this))
, m_mean(new QLabel(this))
, m_standardDeviation(new QLabel(this))
{
  QBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(m_pixmapLabel);
  vlayout1->addLayout(makeAxisBar());
  vlayout1->addStretch();

  QBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addWidget(m_min);
  vlayout2->addWidget(m_max);
  vlayout2->addWidget(m_mean);
  vlayout2->addStretch();

  QBoxLayout *vlayout3 = new QVBoxLayout;
  vlayout3->addWidget(m_count);
  vlayout3->addWidget(m_standardDeviation);
  vlayout3->addStretch();

  QBoxLayout *layout = new QHBoxLayout;
  layout->addLayout(vlayout1);
  layout->addStretch();
  layout->addLayout(vlayout2);
  layout->addLayout(vlayout3);

  setLayout(layout);
}

CHistogram::~CHistogram()
{
}

void CHistogram::setValues(const QVector<uint>& p_values)
{
  m_values = p_values;

  drawPixmap();
  computeStats();
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

void CHistogram::drawPixmap()
{
  // normalize histogram
  uint max = 0;
  for (int i = 0; i < m_values.size(); ++i)
  {
    if (m_values[i] > max)
    max = m_values[i];
  }

  QVector<qreal> normalizedValues(m_values.size(), 0);
  if (max > 0.0)
  {
    for (int i = 0; i < 256; ++i)
    {
      normalizedValues[i] = m_values[i] / (qreal) max;
    }
  }

  QPixmap pixmap(300, 50);
  QPainter painter(&pixmap);

  const qreal width  = pixmap.width();
  const qreal height = pixmap.height();
  const qreal barWidth = width / (qreal)normalizedValues.size();

  for (int i = 0; i < normalizedValues.size(); ++i)
  {
    const qreal barHeight = normalizedValues[i] * height;

    // draw bar
    painter.fillRect(barWidth * i, height - barHeight, /* origin */
      barWidth * (i + 1), height, /* size */
      m_color);

    // fill the rest of the pixmap
    painter.fillRect(barWidth * i, 0, /* origin */
      barWidth * (i + 1), height - barHeight, /* size */
      Qt::white);
  }

  m_pixmapLabel->setPixmap(pixmap);
}

void CHistogram::computeStats()
{
  uint squareSum = 0;
  uint sum = 0;
  int min = 255;
  int max = 0;
  int count = 0;

  for (int i = 0; i < m_values.size(); ++i)
  {
    sum += i * m_values[i];
    squareSum += i * i * m_values[i];

    min = qMin(min, i);
    max = qMax(max, i);
    count += m_values[i];
  }

  double mean = sum / (double) count;
  const double meanSquare = squareSum / (double) count;
  double standardDeviation = qSqrt(qAbs(meanSquare - mean * mean));

  // update labels
  m_count->setText(tr("Count: %1").arg(count));
  m_min->setText(tr("Min: %1").arg(min));
  m_max->setText(tr("Max: %1").arg(max));
  m_mean->setText(tr("Mean: %1").arg(mean));
  m_standardDeviation->setText(tr("StdDev: %1").arg(standardDeviation));
}
