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

  if (!model)
    {
      parent()->showMessage(tr("Can't display properties of invalid models"));
      close();
    }

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
  connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));


  QTableWidgetItem  *item;

  // model info
  const int nbProperties = (model->channels() == 1) ? 10 : 5;

  QTableWidget * matrixInfo = new QTableWidget(nbProperties, 2, this);
  matrixInfo->setAlternatingRowColors(true);
  matrixInfo->setHorizontalHeaderLabels(QStringList() << tr("Property") << tr("Value"));
  matrixInfo->verticalHeader()->setVisible(false);
  matrixInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
  matrixInfo->horizontalHeader()->setStretchLastSection(true);
  matrixInfo->setSelectionMode(QAbstractItemView::NoSelection);
  matrixInfo->setColumnWidth(0, 150);

  item = new QTableWidgetItem(tr("Rows"));
  matrixInfo->setItem(0, 0, item);

  item = new QTableWidgetItem(QString::number(model->rowCount()));
  matrixInfo->setItem(0, 1, item);

  item = new QTableWidgetItem(tr("Columns"));
  matrixInfo->setItem(1, 0, item);

  item = new QTableWidgetItem(QString::number(model->columnCount()));
  matrixInfo->setItem(1, 1, item);

  item = new QTableWidgetItem(tr("Type"));
  matrixInfo->setItem(2, 0, item);

  item = new QTableWidgetItem(model->typeString());
  matrixInfo->setItem(2, 1, item);

  item = new QTableWidgetItem(tr("Channels"));
  matrixInfo->setItem(3, 0, item);

  item = new QTableWidgetItem(QString::number(model->channels()));
  matrixInfo->setItem(3, 1, item);

  item = new QTableWidgetItem(tr("Elements"));
  matrixInfo->setItem(4, 0, item);

  item = new QTableWidgetItem(QString::number(model->total()));
  matrixInfo->setItem(4, 1, item);

  // Properties only available for single-channel matrices
  if (model->channels() == 1)
    {
      item = new QTableWidgetItem(tr("Non-zeros"));
      matrixInfo->setItem(5, 0, item);

      item = new QTableWidgetItem(QString::number(model->countNonZeros()));
      matrixInfo->setItem(5, 1, item);


      double min, max;
      cv::Point minLoc, maxLoc;
      model->minMaxLoc(&min, &max, &minLoc, &maxLoc);

      item = new QTableWidgetItem(tr("Min"));
      matrixInfo->setItem(6, 0, item);

      item = new QTableWidgetItem(QString("%1 @ [%2, %3]").arg(min).arg(minLoc.x).arg(minLoc.y));
      matrixInfo->setItem(6, 1, item);

      item = new QTableWidgetItem(tr("Max"));
      matrixInfo->setItem(7, 0, item);

      item = new QTableWidgetItem(QString("%1 @ [%2, %3]").arg(max).arg(maxLoc.x).arg(maxLoc.y));
      matrixInfo->setItem(7, 1, item);


      double mean, stddev;
      model->meanStdDev(&mean, &stddev);

      item = new QTableWidgetItem(tr("Mean"));
      matrixInfo->setItem(8, 0, item);

      item = new QTableWidgetItem(QString::number(mean));
      matrixInfo->setItem(8, 1, item);

      item = new QTableWidgetItem(tr("StdDev"));
      matrixInfo->setItem(9, 0, item);

      item = new QTableWidgetItem(QString::number(stddev));
      matrixInfo->setItem(9, 1, item);
    }

  QBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(matrixInfo, 1);
  layout->addWidget(buttons);
  setLayout(layout);
  resize(400, 400);
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
