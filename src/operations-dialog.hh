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

#include <QDialog>
#include <QList>

class CMainWindow;
class CMatrixModel;
class COperationWidget;
class QStackedWidget;
class QListWidget;
class QListWidgetItem;

/*!
  \file operations-dialog.hh
  \class COperationsDialog
  \brief COperationsDialog exposes common OpenCV operations through the CMatrixModel wrapper

  This dialog relies on the CMatrixModel to apply OpenCV
  transformations. For example, changing the internal type, applying
  transformations or image filters.

  The dialog groups operations into categories.
  Each category corresponds to a COperationWidget.
*/
class COperationsDialog : public QDialog
{
  Q_OBJECT

public:
  /// Constructor.
  COperationsDialog(QWidget *p_parent = 0);

  /// Destructor.
  ~COperationsDialog() override;

  CMainWindow* parent() const;
  CMatrixModel* model() const;

public slots:

  /*!
    Reset operation parameters and restore original data
  */
  void reset();

  /*!
    Changes the configuration page from \a previous to \a current.
  */
  void changePage(QListWidgetItem *p_current, QListWidgetItem *p_previous);

private:

  void createActions();
  void createIcons();

  CMainWindow *m_parent;
  CMatrixModel *m_backup;

  // Operations widgets
  QListWidget *m_categoriesWidget;
  QStackedWidget *m_operationsWidget;
};
