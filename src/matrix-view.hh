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

#ifndef __MATRIX_VIEW_HH__
#define __MATRIX_VIEW_HH__

#include <QTableView>
#include <QColor>
#include <QPoint>

class CMainWindow;
class QAction;
class QContextMenuEvent;

/*!
  \file matrix-view.hh
  \class CMatrixView
  \brief CMatrixView is a view that presents matrix data as a table of values
*/

class CMatrixView : public QTableView
{
  Q_OBJECT

public:
  /// Constructor.
  CMatrixView(QWidget *parent = 0);

  /// Destructor.
  virtual ~CMatrixView();

  CMainWindow* parent() const;

  virtual void setModel(QAbstractItemModel * model);

public slots:
  void currentChanged(const QModelIndex & index, const QModelIndex & previous);
  void selectItem(int row, int col);

  void properties();
  void adjustColumnsToContents();

protected:
  /*!
    Provides custom context menu with specific actions that are relevant to the matrix view.
    For example, properties, adjustColumns
  */
  virtual void contextMenuEvent(QContextMenuEvent *event);

  virtual void keyPressEvent(QKeyEvent *event);

private slots:
  void verticalHeaderContextMenu(const QPoint & p_pos);
  void horizontalHeaderContextMenu(const QPoint & p_pos);

  void removeCurrentRow();
  void removeCurrentColumn();

  void removeOtherRows();
  void removeOtherColumns();

  void insertRowBeforeCurrent();
  void insertColumnBeforeCurrent();

private:
  CMainWindow *m_parent;

  QAction *m_adjustColumnsAct;
  QAction *m_propertiesAct;

  QModelIndex m_currentSelection;
};

#endif  // __MATRIX_VIEW_HH__
