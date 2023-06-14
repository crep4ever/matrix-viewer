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

#include <QMimeData>
#include <QString>
#include <QTabBar>
#include <QTabWidget>

/*!
  \file tab-widget.hh
  \class CTabWidget
  \brief CTabWidget is the widget that manages tabs
*/

class QPaintEvent;
class QDragEnterEvent;
class QDragLeaveEvent;
class QDropEvent;

class CTabWidget : public QTabWidget
{
    Q_OBJECT

    struct Format
    {
        Format(const QString &p_name, const QString &p_extension) : name(p_name), extensions(p_extension) { }
        QString name;
        QString extensions;
    };

public:
    /// Constructor.
    CTabWidget(QWidget *p_parent = nullptr);

    /// Destructor.
    ~CTabWidget() override;

    void addTab(QWidget *p_page, const QString &p_label);

protected:
    void paintEvent(QPaintEvent *p_event);
    void dragEnterEvent(QDragEnterEvent *p_event);
    void dragLeaveEvent(QDragLeaveEvent *p_event);
    void dropEvent(QDropEvent *p_event);

public slots:
    void changeTabText(const QString &p_str);

private slots:
    void updateAfterClosedTab(int p_tabIndex);

private:
    bool m_dragActiveState;
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
    CTabBar(QWidget *p_parent = nullptr);
    /// Destructor.
    ~CTabBar() override;

protected:
    /*!
    Reimplements QTabBar::mouseReleaseEvent to close
    tabs when they are middle-clicked.
  */
    void mouseReleaseEvent(QMouseEvent *p_event) override;
};
