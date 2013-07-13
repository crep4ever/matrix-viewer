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
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QSettings>
#include <QAction>
#include <QMenu>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>

#include <opencv2/opencv.hpp>

#include "main-window.hh"
#include "position.hh"
#include "matrix-model.hh"

CImageView::CImageView(QWidget *p)
  : QGraphicsView(p)
  , m_parent(qobject_cast<CMainWindow*>(p))
  , m_image(0)
  , m_scene(new QGraphicsScene)
  , m_selectionBox(new QGraphicsRectItem(0, 0, 1, 1))
{
  m_selectionBox->setBrush(QBrush(QColor(255, 0, 0, 100)));
  setBackgroundRole(QPalette::Dark);
  setScene(m_scene);

  createActions();
}

CImageView::~CImageView()
{
  delete m_image;
  delete m_selectionBox;
  delete m_scene;
}

void CImageView::createActions()
{
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
  connect(m_fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

  m_normalSizeAct = new QAction(tr("&Original size"), this);
  m_normalSizeAct->setIcon(QIcon::fromTheme("zoom-original"));
  m_normalSizeAct->setStatusTip(tr("Original size of the image"));
  connect(m_normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));
}

void CImageView::setModel(CMatrixModel * model)
{
  if (m_image)
    delete m_image;

  cv::Mat mat = model->data();
  if (mat.type() == CV_8UC1)
    {
      m_image = new QImage(mat.cols, mat.rows, QImage::Format_Indexed8);
      for (int i = 0; i < mat.rows; ++i)
	memcpy(m_image->scanLine(i), mat.ptr(i), m_image->bytesPerLine());
    }
  else if (mat.type() == CV_8UC1)
    {
      cv::cvtColor(mat, mat, CV_BGR2RGB);
      m_image = new QImage(mat.cols, mat.rows, QImage::Format_RGB888);
      for (int i = 0; i < mat.rows; ++i)
	memcpy(m_image->scanLine(i), mat.ptr(i), m_image->bytesPerLine());
    }
  else if (mat.channels() == 1)
    {
      mat.convertTo(mat, CV_64FC1);
      m_image = new QImage(mat.cols, mat.rows, QImage::Format_ARGB32);
      for (int y = 0; y < mat.rows; ++y)
	{
	  const double *cvRow = mat.ptr<double>(y);
	  QRgb *qtRow = (QRgb*)m_image->scanLine(y);
	  for (int x = 0; x < mat.cols; ++x)
	    {
	      uint color = cvRow[x] * 255;
	      qtRow[x] = qRgba(color, color, color, 255);
	    }
	}
    }
  else if (mat.channels() == 3)
    {
      m_image = new QImage(mat.cols, mat.rows, QImage::Format_ARGB32);
      for (int y = 0; y < mat.rows; ++y)
	{
	  const cv::Vec3b *cvRow = mat.ptr<cv::Vec3b>(y);
	  QRgb *qtRow = (QRgb*)m_image->scanLine(y);
	  for (int x = 0; x < mat.cols; ++x)
	    {
	      qtRow[x] = qRgba(cvRow[x][2], cvRow[x][1], cvRow[x][0], 255);
	    }
	}
    }
  else
    {
      qWarning() << tr("Unsupported image conversion from type: %1").arg(model->typeString());
      return;
    }

  m_scene->setSceneRect(QRect(0, 0, m_image->width(), m_image->height()));
  m_scene->addPixmap(QPixmap::fromImage(*m_image));
  m_scene->addItem(m_selectionBox);

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

  centerOn(mapToScene(event->pos()));
}

void CImageView::mousePressEvent(QMouseEvent *event)
{
  const QPointF scenePoint = mapToScene(event->pos());
  // ensure scenePoint is within the scene range
  if (scenePoint.x() < sceneRect().width() &&
      scenePoint.y() < sceneRect().height())
    {
      selectItem(scenePoint.y(), scenePoint.x());
    }
}

void CImageView::keyPressEvent(QKeyEvent *event)
{
  if (!parent())
    {
      return QGraphicsView::keyPressEvent(event);
    }

  const uint row = parent()->positionWidget()->row();
  const uint col = parent()->positionWidget()->col();

  switch (event->key())
    {
    case Qt::Key_Left:
      selectItem(row, col - 1);
      break;

    case Qt::Key_Right:
      selectItem(row, col + 1);
      break;

    case Qt::Key_Down:
      selectItem(row + 1, col);
      break;

    case Qt::Key_Up:
      selectItem(row - 1, col);
      break;

    default:
      QGraphicsView::keyPressEvent(event);
    }
}

void CImageView::selectItem(int row, int col)
{
  m_selectionBox->setPos(col, row);

  if (parent())
    {
      parent()->positionWidget()->setRow(row);
      parent()->positionWidget()->setCol(col);
      QColor pixel(m_image->pixel(col, row));
      parent()->positionWidget()->setValue
	(QString("%1 | %2 | %3")
	 .arg(pixel.red()).arg(pixel.green()).arg(pixel.blue()));
    }
}

void CImageView::zoomIn()
{
  scale(1.2, 1.2);
}

void CImageView::zoomOut()
{
  scale(1 / 1.2, 1 / 1.2);
}

void CImageView::normalSize()
{
  resetTransform();
}

void CImageView::fitToWindow()
{
  fitInView(sceneRect(), Qt::KeepAspectRatio);
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
