// Copyright (C) 2017, Romain Goffe <romain.goffe@gmail.com>
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

class QSpinBox;
class QComboBox;
class QDoubleSpinBox;

class NewMatrixDialog : public QDialog
{
  Q_OBJECT

public:
  /// Constructor.
  NewMatrixDialog(QWidget* p_parent = 0);

private:
  void readSettings();
  void writeSettings();

protected:
  void accept() override;

private slots:
  void channelsChanged(int p_nbChannels);

private:
  QSpinBox *m_rows;
  QSpinBox *m_cols;
  QSpinBox *m_channels;
  QComboBox *m_type;
  QDoubleSpinBox *m_value1;
  QDoubleSpinBox *m_value2;
  QDoubleSpinBox *m_value3;
};
