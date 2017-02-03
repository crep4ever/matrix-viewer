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

#pragma once

#include <QTabWidget>
#include <QTabBar>
#include <QString>
/*!
  \file tab-widget.hh
  \class CTabWidget
  \brief CTabWidget is the widget that manages tabs
*/

class CTabWidget : public QTabWidget
{
  Q_OBJECT

public:
  /// Constructor.
  CTabWidget(QWidget *parent = 0);

  /// Destructor.
  virtual ~CTabWidget();

  void addTab(QWidget * page, const QString & label);

public slots:
  void changeTabText(const QString& str);
};

/*!
  \file tab-widget.hh
  \class CTabBar
  \brief CTabBar is the widget representing the tabs in the mainwindow

  A CTabBar allows tabs to be closed when middle-clicked with a mouse.

  \image html tab-widget.png

*/

class QMouseEvent;

class CTabBar : public QTabBar
{
  Q_OBJECT

public:
  /// Constructor.
  CTabBar(QWidget *parent=0);
  /// Destructor.
  ~CTabBar();

protected:
  /*!
    Reimplements QTabBar::mouseReleaseEvent to close
    tabs when they are middle-clicked.
  */
  virtual void mouseReleaseEvent(QMouseEvent *event);
};
