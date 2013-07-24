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

#include <QContextMenuEvent>
#include <QSettings>
#include <QAction>
#include <QMenu>
#include <QDebug>

#include "main-window.hh"
#include "position.hh"
#include "properties-dialog.hh"

CMatrixView::CMatrixView(QWidget *p)
  : QTableView(p)
  , m_parent(qobject_cast<CMainWindow*>(p))
{
  setAlternatingRowColors(true);
  setShowGrid(true);
  setSortingEnabled(true);
  setEditTriggers(QAbstractItemView::DoubleClicked);
  setSelectionMode(QAbstractItemView::SingleSelection);

  m_adjustColumnsAct = new QAction(tr("&Adjust columns"), this);
  m_adjustColumnsAct->setStatusTip(tr("Adjust columns of the table view to contents"));
  connect(m_adjustColumnsAct, SIGNAL(triggered()), SLOT(adjustColumnsToContents()));

  m_propertiesAct = new QAction(tr("&Properties"), this);
  m_propertiesAct->setIcon(QIcon::fromTheme("document-properties"));
  m_propertiesAct->setStatusTip(tr("Display properties of the matrix"));
  connect(m_propertiesAct, SIGNAL(triggered()), SLOT(properties()));
}

CMatrixView::~CMatrixView()
{
  delete itemDelegate();
  delete model();
}

void CMatrixView::currentChanged(const QModelIndex & index, const QModelIndex & previous)
{
  Q_UNUSED(previous);
  parent()->positionWidget()->setRow(index.row());
  parent()->positionWidget()->setCol(index.column());
  parent()->positionWidget()->setValue(index.data().toString());
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

void CMatrixView::keyPressEvent(QKeyEvent *event)
{
  QTableView::keyPressEvent(event);

  switch (event->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Down:
    case Qt::Key_Up:
      scrollTo(selectionModel()->currentIndex());
      break;

    default:
      break;
    }
}

void CMatrixView::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu *menu = new QMenu;

  menu->addAction(m_adjustColumnsAct);
  menu->addAction(m_propertiesAct);

  menu->exec(event->globalPos());
  delete menu;
}

void CMatrixView::properties()
{
  CPropertiesDialog dialog(parent());
  dialog.exec();
}

void CMatrixView::adjustColumnsToContents()
{
  resizeColumnsToContents();
}

CMainWindow* CMatrixView::parent() const
{
  if (!m_parent)
    qWarning() << "CMatrixView::parent invalid parent";

  return m_parent;
}

