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
#include "image-view.hh"

#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QScrollBar>
#include <QWheelEvent>
#include <QSettings>
#include <QDebug>

#include <opencv2/opencv.hpp>

#include "main-window.hh"
#include "position.hh"
#include "matrix-model.hh"

CImageView::CImageView(QWidget *p)
  : QScrollArea(p)
  , m_parent(qobject_cast<CMainWindow*>(p))
  , m_image(0)
  , m_imageLabel(new QLabel)
  , m_scaleFactor(1.0)
{
  m_imageLabel->setBackgroundRole(QPalette::Base);
  m_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  m_imageLabel->setScaledContents(true);
  
  setBackgroundRole(QPalette::Dark);
  setWidget(m_imageLabel);
  //setWidgetResizable(true);
}

CImageView::~CImageView()
{
}

void CImageView::currentChanged(const QModelIndex & index, const QModelIndex & previous)
{
  Q_UNUSED(previous);
  parent()->positionWidget()->setRow(index.row());
  parent()->positionWidget()->setCol(index.column());
  parent()->positionWidget()->setValue(index.data().toDouble());
}

void CImageView::setModel(CMatrixModel * model)
{
  if (m_image)
    delete m_image;

  cv::Mat data = model->data();
  m_image = new QImage(data.cols, data.rows, QImage::Format_ARGB32);

  for (int y = 0; y < data.rows; ++y)
    {
      const double *cvRow = data.ptr<double>(y);
      QRgb *qtRow = (QRgb*)m_image->scanLine(y);

      for (int x = 0; x < data.cols; ++x)
	{
	  uint color = cvRow[x] * 255;
	  qtRow[x] = qRgba(color, color, color, 255);
	}
    }

  m_imageLabel->setPixmap(QPixmap::fromImage(*m_image));
  normalSize();
}

CMainWindow* CImageView::parent() const
{
  if (!m_parent)
    qWarning() << "CImageView::parent invalid parent";

  return m_parent;
}

void CImageView::wheelEvent(QWheelEvent *event)
{
  if (event->delta() > 0)
    zoomIn();
  else
    zoomOut();
}

void CImageView::zoomIn()
{
  scaleImage(1.25);
}

void CImageView::zoomOut()
{
  scaleImage(0.8);
}

void CImageView::normalSize()
{
  m_imageLabel->adjustSize();
  m_scaleFactor = 1.0;
}

void CImageView::scaleImage(const double factor)
{
  if (!m_imageLabel->pixmap())
    return;

  m_scaleFactor *= factor;
  m_imageLabel->resize(m_scaleFactor * m_imageLabel->pixmap()->size());

  adjustScrollBar(horizontalScrollBar(), factor);
  adjustScrollBar(verticalScrollBar(), factor);
}

void CImageView::adjustScrollBar(QScrollBar *scrollBar, const double factor)
{
  scrollBar->setValue(int(factor * scrollBar->value()
			  + ((factor - 1) * scrollBar->pageStep()/2)));
}
