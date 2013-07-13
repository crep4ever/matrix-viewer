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

#ifndef __IMAGE_VIEW_HH__
#define __IMAGE_VIEW_HH__

#include <QGraphicsView>
#include <QModelIndex>

class CMainWindow;
class CMatrixModel;

class QAction;
class QImage;
class QWheelEvent;
class QMouseEvent;
class QKeyEvent;
class QGraphicsScene;
class QGraphicsRectItem;

/*!
  \file image-view.hh
  \class CImageView
  \brief CImageView is a view that presents matrix data as an image
*/

class CImageView : public QGraphicsView
{
  Q_OBJECT

public:
  /// Constructor.
  CImageView(QWidget *parent = 0);

  /// Destructor.
  virtual ~CImageView();

  CMainWindow* parent() const;

  virtual void setModel(CMatrixModel *model);

  void wheelEvent(QWheelEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void keyPressEvent(QKeyEvent *event);

public slots:
  void selectItem(int row, int col);

  void zoomIn();
  void zoomOut();
  void normalSize();
  void fitToWindow();

protected:
  /*!
    Provides custom context menu with specific actions that are relevant to the image view.
    For example, zoom actions
  */
  void contextMenuEvent(QContextMenuEvent *event);

private:
  void createActions();
  
  CMainWindow *m_parent;
  QImage *m_image;
  QGraphicsScene *m_scene;
  QGraphicsRectItem *m_selectionBox;

  // context menu actions
  QAction *m_zoomInAct;
  QAction *m_zoomOutAct;
  QAction *m_normalSizeAct;
  QAction *m_fitToWindowAct;
};

#endif  // __IMAGE_VIEW_HH__
