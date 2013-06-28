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
#include "delegate.hh"

#include <QPainter>
#include <QSettings>
#include <QDebug>

QColor CDelegate::_redColor = QColor(239, 41, 41);
QColor CDelegate::_greenColor = QColor(128, 226, 52);

CDelegate::CDelegate(QWidget *parent) : QItemDelegate(parent)
{
  readSettings();
}

CDelegate::~CDelegate()
{
  writeSettings();
}



void CDelegate::readSettings()
{
  QSettings settings;
  settings.beginGroup("compare");
  m_absoluteThreshold = settings.value("absoluteThreshold", true).toDouble();
  m_percentageThreshold = settings.value("percentageThreshold", true).toDouble();
  settings.endGroup();
}

void CDelegate::writeSettings()
{
  qDebug() << "not implemented yet";
}

void CDelegate::paint(QPainter *painter,
		      const QStyleOptionViewItem &option,
		      const QModelIndex &index) const
{
  QStyleOptionViewItemV4 opt(option);

  const double value = index.data().toDouble();
  if (value >= m_absoluteThreshold)
    {
      painter->fillRect(opt.rect, _redColor);
    }
  else
    {
      painter->fillRect(opt.rect, _greenColor);
    }

  return QItemDelegate::paint(painter, option, index);
}
