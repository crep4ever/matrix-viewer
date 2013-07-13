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

#ifndef __PROPERTIES_DIALOG_HH__
#define __PROPERTIES_DIALOG_HH__

#include <QDialog>

class CMainWindow;

/*!
  \file properties-dialog.hh
  \class CPropertiesDialog
  \brief CPropertiesDialog is a read-only dialog that displays global information about the matrix
*/

class CPropertiesDialog : public QDialog
{
  Q_OBJECT

public:
  /// Constructor.
  CPropertiesDialog(QWidget *parent = 0);

  /// Destructor.
  virtual ~CPropertiesDialog();

  CMainWindow* parent() const;

private:

  CMainWindow *m_parent;
};

#endif  // __PROPERTIES_DIALOG_HH__
