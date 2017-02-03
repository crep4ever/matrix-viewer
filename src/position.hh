// Copyright (C) 2009-2011, Romain Goffe <romain.goffe@gmail.com>
// Copyright (C) 2009-2011, Alexandre Dupas <alexandre.dupas@gmail.com>
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

#include <QWidget>

class QSpinBox;
class QLabel;

/*!
  \file position.hh
  \class CPosition
  \brief CPosition is a small widget that displays the value matrix at an editable row/column position
*/
class CPosition : public QWidget
{
  Q_OBJECT

public:
  /// Constructor.
  CPosition(QWidget *parent=0);

  /// Destructor.
  ~CPosition();

  uint row() const;
  uint col() const;

public slots:
  void setRow(const uint row);
  void setCol(const uint col);
  void setValue(const QString & value);
  void setValueDescription(const QString & desc);
  void changeRow(int row);
  void changeCol(int col);

signals:
  void positionChanged(int row, int col);

private:
  QSpinBox* m_rowSpinBox;
  QSpinBox* m_colSpinBox;
  QLabel*   m_valueLabel;
  QString   m_valueDescription;
};
