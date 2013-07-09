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
#include <QAction>
#include <QMenu>
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

  m_zoomInAct = new QAction(tr("Zoom &In"), this);
  m_zoomInAct->setIcon(QIcon::fromTheme("zoom-in"));
  m_zoomInAct->setShortcut(QKeySequence::ZoomIn);
  m_zoomInAct->setStatusTip(tr("Zoom in"));
  connect(m_zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

  m_zoomOutAct = new QAction(tr("Zoom &Out"), this);
  m_zoomOutAct->setIcon(QIcon::fromTheme("zoom-out"));
  m_zoomOutAct->setShortcut(QKeySequence::ZoomOut);
  m_zoomOutAct->setStatusTip(tr("Zoom out"));
  connect(m_zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

  m_fitToWindowAct = new QAction(tr("&Fit window"), this);
  m_fitToWindowAct->setIcon(QIcon::fromTheme("zoom-fit-best"));
  m_fitToWindowAct->setStatusTip(tr("Fit the image within the window"));
  m_fitToWindowAct->setCheckable(true);
  connect(m_fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

  m_normalSizeAct = new QAction(tr("&Original size"), this);
  m_normalSizeAct->setIcon(QIcon::fromTheme("zoom-original"));
  m_normalSizeAct->setStatusTip(tr("Original size of the image"));
  connect(m_normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));
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
  fitToWindow();
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

void CImageView::fitToWindow()
{
  const bool fitToWindow = m_fitToWindowAct->isChecked();
  setWidgetResizable(fitToWindow);
  if (!fitToWindow)
    normalSize();
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

void CImageView::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu *menu = new QMenu;

  menu->addAction(m_zoomInAct);
  menu->addAction(m_zoomOutAct);
  menu->addAction(m_fitToWindowAct);
  menu->addAction(m_normalSizeAct);

  menu->exec(event->globalPos());
  delete menu;
}
