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
#include <QHeaderView>
#include <QDebug>

#include "main-window.hh"
#include "position.hh"
#include "properties-dialog.hh"

CMatrixView::CMatrixView(QWidget *p_parent) : QTableView(p_parent)
, m_parent(qobject_cast<CMainWindow*>(p_parent))
, m_adjustColumnsAct(0)
, m_propertiesAct(0)
{
  setAlternatingRowColors(true);
  setShowGrid(true);
  setSortingEnabled(true);
  setEditTriggers(QAbstractItemView::DoubleClicked);
  setSelectionMode(QAbstractItemView::SingleSelection);

  horizontalHeader()->setDefaultSectionSize(120);
  horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(horizontalHeader(), SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(horizontalHeaderContextMenu(const QPoint &)));

  verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(verticalHeader(), SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(verticalHeaderContextMenu(const QPoint &)));

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

void CMatrixView::currentChanged(const QModelIndex & p_index, const QModelIndex & p_previous)
{
  Q_UNUSED(p_previous);
  if (parent() != nullptr && parent()->positionWidget() != nullptr && p_index.isValid())
  {
    parent()->positionWidget()->setRow(p_index.row());
    parent()->positionWidget()->setCol(p_index.column());
    parent()->positionWidget()->setValue(p_index.data().toString());
  }
}

void CMatrixView::setModel(QAbstractItemModel *p_model)
{
  QItemSelectionModel *sm = selectionModel();
  QTableView::setModel(p_model);
  delete sm;

  connect(selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
  this, SLOT(currentChanged(const QModelIndex &, const QModelIndex &)));
}

void CMatrixView::selectItem(int p_row, int p_col)
{
  int r = qMax(0, p_row);
  r = qMin(r, model()->rowCount() - 1);

  int c = qMax(0, p_col);
  c = qMin(c, model()->columnCount() - 1);

  setCurrentIndex(model()->index(r, c));
  scrollTo(model()->index(r, c));
}

void CMatrixView::keyPressEvent(QKeyEvent *p_event)
{
  QTableView::keyPressEvent(p_event);

  switch (p_event->key())
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

void CMatrixView::contextMenuEvent(QContextMenuEvent *p_event)
{
  QMenu *menu = new QMenu;

  menu->addAction(m_adjustColumnsAct);
  menu->addAction(m_propertiesAct);

  menu->exec(p_event->globalPos());
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
  if (m_parent == nullptr)
  {
    qWarning() << "CMatrixView::parent invalid parent";
  }

  return m_parent;
}

void CMatrixView::verticalHeaderContextMenu(const QPoint & p_pos)
{
  m_currentSelection = indexAt(p_pos);

  setSelectionBehavior(QAbstractItemView::SelectRows);
  selectRow(m_currentSelection.row());

  QMenu *menu = new QMenu;

  QAction * action = new QAction(tr("Insert row before current"), this);
  connect(action, SIGNAL(triggered()), this, SLOT(insertRowBeforeCurrent()));
  menu->addAction(action);

  action = new QAction(tr("Remove current row"), this);
  connect(action, SIGNAL(triggered()), this, SLOT(removeCurrentRow()));
  menu->addAction(action);

  action = new QAction(tr("Remove other rows"), this);
  connect(action, SIGNAL(triggered()), this, SLOT(removeOtherRows()));
  menu->addAction(action);

  menu->exec(mapToGlobal(p_pos));
  delete menu;

  setSelectionBehavior(QAbstractItemView::SelectItems);
}

void CMatrixView::horizontalHeaderContextMenu(const QPoint & p_pos)
{
  m_currentSelection = indexAt(p_pos);

  setSelectionBehavior(QAbstractItemView::SelectColumns);
  selectColumn(m_currentSelection.column());

  QMenu *menu = new QMenu;

  QAction * action = new QAction(tr("Insert column before current"), this);
  connect(action, SIGNAL(triggered()), this, SLOT(insertColumnBeforeCurrent()));
  menu->addAction(action);

  action = new QAction(tr("Remove current column"), this);
  connect(action, SIGNAL(triggered()), this, SLOT(removeCurrentColumn()));
  menu->addAction(action);

  action = new QAction(tr("Remove other columns"), this);
  connect(action, SIGNAL(triggered()), this, SLOT(removeOtherColumns()));
  menu->addAction(action);

  menu->exec(mapToGlobal(p_pos));
  delete menu;

  setSelectionBehavior(QAbstractItemView::SelectItems);
}

void CMatrixView::removeCurrentRow()
{
  const int idx = m_currentSelection.row();
  if (!model()->removeRow(idx))
  {
    qWarning() << "Can't remove row " << idx;
  }
}

void CMatrixView::removeCurrentColumn()
{
  const int idx = m_currentSelection.column();
  if (!model()->removeColumn(idx))
  {
    qWarning() << "Can't remove column " << idx;
  }
}

void CMatrixView::removeOtherRows()
{
  bool success = true;
  const int idx = m_currentSelection.row();

  // remove top part
  if (idx > 0)
  {
    success = success && model()->removeRows(0, idx);
  }

  // remove bottom part
  success = success && model()->removeRows(1, model()->rowCount() - 1);

  if (!success)
  {
    qWarning() << "Can't remove all other rows than " << idx;
  }
}

void CMatrixView::removeOtherColumns()
{
  bool success = true;
  const int idx = m_currentSelection.column();

  // remove left part
  if (idx > 0)
  {
    success = success && model()->removeColumns(0, idx);
  }

  // remove right part
  success = success && model()->removeColumns(1, model()->columnCount() - 1);

  if (!success)
  {
    qWarning() << "Can't remove all other columns than " << idx;
  }
}

void CMatrixView::insertRowBeforeCurrent()
{
  const int idx = m_currentSelection.row();
  if (!model()->insertRow(idx))
  {
    qWarning() << "Can't insert row before index " << idx;
  }
}

void CMatrixView::insertColumnBeforeCurrent()
{
  const int idx = m_currentSelection.column();
  if (!model()->insertColumn(idx))
  {
    qWarning() << "Can't insert column before index " << idx;
  }
}
