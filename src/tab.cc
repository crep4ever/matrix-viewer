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

#include <QDebug>

#include "matrix-view.hh"

CTab::CTab()
  : QSplitter(Qt::Horizontal)
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
      disconnect(view->model());
      connect(view->model(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
	      this, SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));
    }
}

void CTab::modelDataChanged(const QModelIndex & index,
				  const QModelIndex & previous)
{
  Q_UNUSED(index);
  Q_UNUSED(previous);

  // update the window title
  if (!windowTitle().contains(" *"))
    {
      setWindowTitle(windowTitle() + " *");
      emit(labelChanged(windowTitle()));
    }
}
