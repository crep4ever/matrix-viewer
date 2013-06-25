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
#include "matrix-model.hh"

#include <QDebug>

CMatrixModel::CMatrixModel()
  : QAbstractTableModel()
  , m_data()
{
}

CMatrixModel::~CMatrixModel()
{
}

cv::Mat CMatrixModel::data() const
{
  return m_data;
}

void CMatrixModel::setData(const cv::Mat & matrix)
{
  m_data = matrix;
}

int CMatrixModel::rowCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return data().rows;
}

int CMatrixModel::columnCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return data().cols;
}

QVariant CMatrixModel::data (const QModelIndex & index, int role) const
{
  switch (role)
    {
    case Qt::DisplayRole:
      return data().at< double >(index.row(), index.column());
    
    default:
      data(index, Qt::DisplayRole);
    }
  
  return QVariant();
}

bool CMatrixModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  switch (role)
    {
    case Qt::EditRole:
	data().at< double >(index.row(), index.column()) = value.toDouble();
	return true;

    default:
      qDebug() << "setdata role not supported yet";
    }

 return false;
}

Qt::ItemFlags CMatrixModel::flags(const QModelIndex & index) const
{
  (void) index;
  return Qt::ItemIsSelectable | Qt::ItemIsEditable  | Qt::ItemIsEnabled;
}
