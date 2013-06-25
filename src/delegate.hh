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

#ifndef __DELEGATE_HH__
#define __DELEGATE_HH__

#include <QColor>
#include <QItemDelegate>

/*!
  \file delegate.hh
  \class CDelegate
  \brief CDelegate is a delegate that can highlight cells from data matrix views
*/

class CDelegate : public QItemDelegate
{
  Q_OBJECT

  public:
  /// Constructor
  CDelegate(QWidget *parent = 0);

  /// Destructor
  virtual ~CDelegate();

  void paint(QPainter *painter,
	     const QStyleOptionViewItem &option,
	     const QModelIndex &index) const;

  void readSettings();
  void writeSettings();

private:
  double m_absoluteThreshold;
  double m_percentageThreshold;

  static QColor _redColor;
  static QColor _greenColor;

};

#endif  // __DELEGATE_HH__
