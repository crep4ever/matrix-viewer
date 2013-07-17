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
  m_selectionBox->setPen(Qt::NoPen);
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

QImage* CImageView::imageFromCvMat(const cv::Mat & mat)
{
  cv::Mat data = mat.clone();

  if (mat.type() == CV_16U) // assuming range 0-65535
    {
      data /= 256;
    }
  else if (mat.type() == CV_32F || mat.type() == CV_64F)
    {
      double min = 0, max = 0;
      cv::minMaxLoc(data, &min, &max);
      if (max <= 1) // assuming range 0-1
	{
	  data *= 255; //scale up
	}
      else // try to keep dynamic of image
	{
	  double min2 = 0, max2 = 0;
	  cv::Mat tmp = data - min;
	  cv::minMaxLoc(tmp, &min2, &max2);
	  data = tmp * 255 / max2;
	}
    }

  data.convertTo(data, CV_8U);
  cv::cvtColor(data, data, mat.channels() < 3 ? CV_GRAY2RGB : CV_BGR2RGB);

  QImage *image = new QImage(data.cols, data.rows, QImage::Format_RGB888);
  for (int i = 0; i < data.rows; ++i)
    memcpy(image->scanLine(i), data.ptr(i), image->bytesPerLine());

  return image;
}

void CImageView::setModel(CMatrixModel * model)
{
  if (m_image)
    delete m_image;

  m_image = imageFromCvMat(model->data());

  m_scene->setSceneRect(QRect(0, 0, m_image->width(), m_image->height()));
  m_scene->addPixmap(QPixmap::fromImage(*m_image));
  m_scene->addItem(m_selectionBox);
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

  const int row = parent()->positionWidget()->row();
  const int col = parent()->positionWidget()->col();

  switch (event->key())
    {
    case Qt::Key_Left:
      selectItem(row, qMax(0, col - 1));
      break;

    case Qt::Key_Right:
      selectItem(row, qMin((int) sceneRect().width() - 1, col + 1));
      break;

    case Qt::Key_Down:
      selectItem(qMin((int) sceneRect().height() - 1, row + 1), col);
      break;

    case Qt::Key_Up:
      selectItem(qMax(0, row - 1), col);
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
