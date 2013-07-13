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
#include "properties-dialog.hh"

#include <QDialogButtonBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QBoxLayout>
#include <QDebug>

#include "main-window.hh"
#include "matrix-model.hh"
#include "matrix-view.hh"
#include "image-view.hh"

CPropertiesDialog::CPropertiesDialog(QWidget *p)
  : QDialog(p)
  , m_parent(qobject_cast<CMainWindow*>(p))
{
  setWindowTitle(tr("Matrix properties"));

  CMatrixModel *model = parent()->currentModel();

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
  connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));


  QTableWidgetItem  *item;

  // model info
  QTableWidget * matrixInfo = new QTableWidget(4, 2, this);
  matrixInfo->setAlternatingRowColors(true);
  matrixInfo->setHorizontalHeaderLabels(QStringList() << tr("Property") << tr("Value"));
  matrixInfo->verticalHeader()->setVisible(false);
  matrixInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
  matrixInfo->horizontalHeader()->setStretchLastSection(true);
  matrixInfo->setSelectionMode(QAbstractItemView::NoSelection);

  item = new QTableWidgetItem(tr("Rows:"));
  matrixInfo->setItem(0, 0, item);

  QString row = model ? QString::number(model->rowCount()) : QString::number(0);
  item = new QTableWidgetItem(row);
  matrixInfo->setItem(0, 1, item);

  item = new QTableWidgetItem(tr("Columns:"));
  matrixInfo->setItem(1, 0, item);

  QString col = model ? QString::number(model->columnCount()) : QString::number(0);
  item = new QTableWidgetItem(col);
  matrixInfo->setItem(1, 1, item);

  item = new QTableWidgetItem(tr("Type:"));
  matrixInfo->setItem(2, 0, item);

  QString type = model ? model->typeString() : tr("invalid");
  item = new QTableWidgetItem(type);
  matrixInfo->setItem(2, 1, item);

  item = new QTableWidgetItem(tr("Channels:"));
  matrixInfo->setItem(3, 0, item);

  QString channels = model ? QString::number(model->channels()) : tr("invalid");
  item = new QTableWidgetItem(channels);
  matrixInfo->setItem(3, 1, item);

  // image info

  QBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(matrixInfo);
  layout->addStretch();
  layout->addWidget(buttons);

  setLayout(layout);
}

CPropertiesDialog::~CPropertiesDialog()
{
}

CMainWindow* CPropertiesDialog::parent() const
{
  if (!m_parent)
    qWarning() << "CPropertiesDialog::parent invalid parent";
  return m_parent;
}
