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

#pragma once

#include <QWidget>
#include <QColor>
#include <QSize>

class QPaintEvent;
class CHistogram;

/*!
  \file histogram-widget.hh
  \class CHistogramWidget
  \brief CHistogramWidget displays the histogram of an image

  CHistogramWidget wraps three CHistogram corresponding
  to RGB channels of a QImage.
*/
class CHistogramWidget : public QWidget
{
  Q_OBJECT

public:
  /// Constructor.
  CHistogramWidget(QWidget *parent = 0);

  /// Destructor.
  virtual ~CHistogramWidget();

  void setImage(QImage *image);

protected:
  virtual QSize sizeHint() const;
  void paintEvent(QPaintEvent* event);

private:
  CHistogram *m_redHistogram;
  CHistogram *m_greenHistogram;
  CHistogram *m_blueHistogram;

  static const QColor _red;
  static const QColor _green;
  static const QColor _blue;
};
