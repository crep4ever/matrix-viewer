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
#include "tab.hh"

#include "image-view.hh"
#include "matrix-view.hh"

#include <QDebug>
#include <QFileInfo>

CTab::CTab() : QSplitter(Qt::Horizontal), m_isModified(false), m_filePath() { }

CTab::~CTab() { }

void CTab::addWidget(QWidget *p_widget)
{
    QSplitter::addWidget(p_widget);
    if (CMatrixView *view = qobject_cast<CMatrixView *>(widget(0)))
    {
        connect(view->model(),
                SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
                this,
                SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));
    }
}

void CTab::modelDataChanged(const QModelIndex &p_index, const QModelIndex &p_previous)
{
    Q_UNUSED(p_index);
    Q_UNUSED(p_previous);

    setModified(true);
    emit(wasModified());
}

void CTab::selectItem(int p_row, int p_col)
{
    for (int i = 0; i < count(); ++i)
    {
        CMatrixView *dataView = qobject_cast<CMatrixView *>(widget(i));
        if (dataView != nullptr)
        {
            dataView->selectItem(p_row, p_col);
        }

        CImageView *imgView = qobject_cast<CImageView *>(widget(i));
        if (imgView != nullptr)
        {
            imgView->selectItem(p_row, p_col);
        }
    }
}

bool CTab::isModified() const
{
    return m_isModified;
}

void CTab::setModified(const bool p_modified)
{
    m_isModified = p_modified;

    // update the window title
    if (p_modified && !windowTitle().contains(" *"))
    {
        setWindowTitle(windowTitle() + " *");
        emit(labelChanged(windowTitle()));
    }
    else if (!p_modified && windowTitle().contains(" *"))
    {
        setWindowTitle(windowTitle().remove(" *"));
        emit(labelChanged(windowTitle()));
    }
}

QString CTab::filePath() const
{
    return m_filePath;
}

void CTab::setFilePath(const QString &p_path)
{
    m_filePath = p_path;
    QFileInfo fi(p_path);
    setWindowTitle(fi.fileName());
    emit(labelChanged(windowTitle()));
}
