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
#include "tab-widget.hh"

#include "tab.hh"

#include <QDebug>
#include <QFileInfo>
#include <QMouseEvent>

CTabWidget::CTabWidget(QWidget *p_parent) : QTabWidget(p_parent)
{
    setStyleSheet(" QTabWidget::tab-bar {}");
    setTabBar(new CTabBar(this));
}

CTabWidget::~CTabWidget() { }

void CTabWidget::changeTabText(const QString &p_str)
{
    setTabText(currentIndex(), p_str);
}

void CTabWidget::addTab(QWidget *p_page, const QString &p_label)
{
    QFileInfo fi(p_label);
    CTab *tab = qobject_cast<CTab *>(p_page);
    if (tab != nullptr)
    {
        tab->setFilePath(fi.absoluteFilePath());
        tab->setWindowTitle(fi.fileName());
        setToolTip(fi.absoluteFilePath());
        QTabWidget::addTab(p_page, fi.fileName());
    }
    else
    {
        QTabWidget::addTab(p_page, p_label);
    }
}

//----------------------------------------------------------------------------

CTabBar::CTabBar(QWidget *p_parent) : QTabBar(p_parent) { }

CTabBar::~CTabBar() { }

void CTabBar::mouseReleaseEvent(QMouseEvent *p_event)
{
    if (p_event->button() == Qt::MiddleButton)
    {
        emit(tabCloseRequested(tabAt(p_event->pos())));
    }
    QTabBar::mouseReleaseEvent(p_event);
}
