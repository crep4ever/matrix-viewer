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

#include <QScrollArea>
#include <QModelIndex>

class CMainWindow;
class CMatrixModel;

class QWheelEvent;
class QScrollBar;
class QImage;
class QLabel;

/*!
  \file image-view.hh
  \class CImageView
  \brief CImageView is a view that presents matrix data as an image
*/

class CImageView : public QScrollArea
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

public slots:
  void currentChanged(const QModelIndex & index, const QModelIndex & previous);

  void zoomIn();
  void zoomOut();
  void normalSize();
  
private:
  void scaleImage(const double factor);
  void adjustScrollBar(QScrollBar *scrollBar, const double factor);
  
  CMainWindow *m_parent;
  QImage *m_image;
  QLabel *m_imageLabel;
  double m_scaleFactor;
};

#endif  // __IMAGE_VIEW_HH__
