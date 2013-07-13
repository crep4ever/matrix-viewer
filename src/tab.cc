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

#include <QFileInfo>
#include <QDebug>

#include "matrix-view.hh"
#include "image-view.hh"

CTab::CTab()
  : QSplitter(Qt::Horizontal)
  , m_isModified(false)
  , m_filePath()
{
}

CTab::~CTab()
{
}


void CTab::addWidget(QWidget* w)
{
  QSplitter::addWidget(w);
  if (CMatrixView *view = qobject_cast<CMatrixView*>(widget(0)))
    {
      connect(view->model(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
	      this, SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));
    }
}

void CTab::modelDataChanged(const QModelIndex & index,
			    const QModelIndex & previous)
{
  Q_UNUSED(index);
  Q_UNUSED(previous);

  setModified(true);
  emit(wasModified());
}

void CTab::selectItem(int row, int col)
{
  for (int i = 0; i < count(); ++i)
    {
      CMatrixView *dataView = qobject_cast<CMatrixView*>(widget(i));
      if (dataView)
	dataView->selectItem(row, col);

      CImageView *imgView = qobject_cast<CImageView*>(widget(i));
      if (imgView)
	imgView->selectItem(row, col);
    }

}

bool CTab::isModified() const
{
  return m_isModified;
}

void CTab::setModified(const bool modified)
{
  m_isModified = modified;

  // update the window title
  if (modified && !windowTitle().contains(" *"))
    {
      setWindowTitle(windowTitle() + " *");
      emit(labelChanged(windowTitle()));
    }
  else if (!modified && windowTitle().contains(" *"))
    {
      setWindowTitle(windowTitle().remove(" *"));
      emit(labelChanged(windowTitle()));
    }
}

QString CTab::filePath() const
{
  return m_filePath;
}

void CTab::setFilePath(const QString & path)
{
  m_filePath = path;
  QFileInfo fi (path);
  setWindowTitle(fi.fileName());
  emit(labelChanged(windowTitle()));
}
