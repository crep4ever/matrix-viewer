// Copyright (C) 2014, Romain Goffe <romain.goffe@gmail.com>
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
#include "histogram-widget.hh"

#include "histogram.hh"

#include <QBoxLayout>
#include <QDebug>
#include <QPainter>

const QColor CHistogramWidget::_red(239, 41, 41);
const QColor CHistogramWidget::_green(138, 226, 52);
const QColor CHistogramWidget::_blue(114, 159, 207);

CHistogramWidget::CHistogramWidget(QWidget *p_parent)
    : QWidget(p_parent)
    , m_redHistogram(new CHistogram(_red))
    , m_greenHistogram(new CHistogram(_green))
    , m_blueHistogram(new CHistogram(_blue))
{
    setStyleSheet("background: transparent;");
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_redHistogram);
    mainLayout->addWidget(m_greenHistogram);
    mainLayout->addWidget(m_blueHistogram);
    setLayout(mainLayout);

    resize(sizeHint());
}

CHistogramWidget::~CHistogramWidget() { }

void CHistogramWidget::setImage(QImage *p_image)
{
    QImage copy = p_image->convertToFormat(QImage::Format_ARGB32);
    QVector<uint> redValues(256, 0), greenValues(256, 0), blueValues(256, 0);
    for (int j = 0; j < p_image->height(); ++j)
    {
        QRgb *row = reinterpret_cast<QRgb *>(copy.scanLine(j));
        for (int i = 0; i < p_image->width(); ++i)
        {
            ++redValues[qRed(row[i])];
            ++greenValues[qGreen(row[i])];
            ++blueValues[qBlue(row[i])];
        }
    }

    m_redHistogram->setValues(redValues);
    m_greenHistogram->setValues(greenValues);
    m_blueHistogram->setValues(blueValues);
}

QSize CHistogramWidget::sizeHint() const
{
    return QSize(500, 300);
}

void CHistogramWidget::paintEvent(QPaintEvent *p_event)
{
    Q_UNUSED(p_event);
    QColor backgroundColor = palette().dark().color();
    backgroundColor.setAlpha(150);
    QPainter customPainter(this);
    customPainter.fillRect(rect(), backgroundColor);
}
