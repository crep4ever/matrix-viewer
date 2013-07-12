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
#include "matrix-view.hh"

#include <QSettings>
#include <QDebug>

#include "main-window.hh"
#include "position.hh"

CMatrixView::CMatrixView(QWidget *p)
  : QTableView(p)
  , m_parent(qobject_cast<CMainWindow*>(p))
{
  setAlternatingRowColors(true);
  setShowGrid(true);
  setSortingEnabled(true);
  setEditTriggers(QAbstractItemView::DoubleClicked);
  setSelectionMode(QAbstractItemView::SingleSelection);
}

CMatrixView::~CMatrixView()
{
}

void CMatrixView::currentChanged(const QModelIndex & index, const QModelIndex & previous)
{
  Q_UNUSED(previous);
  parent()->positionWidget()->setRow(index.row());
  parent()->positionWidget()->setCol(index.column());
  parent()->positionWidget()->setValue(index.data().toDouble());
}

void CMatrixView::setModel(QAbstractItemModel * model)
{
  QItemSelectionModel *sm = selectionModel();
  QTableView::setModel(model);
  delete sm;

  connect(selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
          this, SLOT(currentChanged(const QModelIndex &, const QModelIndex &)));
}

void CMatrixView::selectItem(int row, int col)
{
  setCurrentIndex(model()->index(row, col));
  scrollTo(model()->index(row, col));
}

CMainWindow* CMatrixView::parent() const
{
  if (!m_parent)
    qWarning() << "CMatrixView::parent invalid parent";

  return m_parent;
}

