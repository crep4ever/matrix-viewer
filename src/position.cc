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
#include "position.hh"

#include <QSpinBox>
#include <QBoxLayout>
#include <QLabel>
#include <QString>

#include <limits>

CPosition::CPosition(QWidget *parent)
  : QWidget(parent)
  , m_rowSpinBox(new QSpinBox)
  , m_colSpinBox(new QSpinBox)
  , m_valueLabel(new QLabel)
{
  m_rowSpinBox->setMaximum(std::numeric_limits<int>::max());
  m_colSpinBox->setMaximum(std::numeric_limits<int>::max());

  connect(m_rowSpinBox, SIGNAL(valueChanged(int)),
	  this, SLOT(changeRow(int)));

  connect(m_colSpinBox, SIGNAL(valueChanged(int)),
	  this, SLOT(changeCol(int)));

  QLabel *rowLabel = new QLabel(tr("Row:"));
  QLabel *colLabel = new QLabel(tr("Col:"));

  QBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(rowLabel);
  layout->addWidget(m_rowSpinBox);
  layout->addWidget(colLabel);
  layout->addWidget(m_colSpinBox);
  layout->addWidget(m_valueLabel);

  setLayout(layout);
}

CPosition::~CPosition()
{
}

uint CPosition::row() const
{
  return m_rowSpinBox->value();
}

void CPosition::setRow(const uint row)
{
  blockSignals(true);
  m_rowSpinBox->setValue(row);
  blockSignals(false);
}

uint CPosition::col() const
{
  return m_colSpinBox->value();
}

void CPosition::setCol(const uint col)
{
  blockSignals(true);
  m_colSpinBox->setValue(col);
  blockSignals(false);
}

void CPosition::changeRow(int row)
{
  emit(positionChanged(row, m_colSpinBox->value()));
}

void CPosition::changeCol(int col)
{
  emit(positionChanged(m_rowSpinBox->value(), col));
}

void CPosition::setValue(const QString & value)
{
  m_valueLabel->setText(tr("Value: %1").arg(value));
}

