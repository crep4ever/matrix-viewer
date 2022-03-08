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

#include "image-view.hh"
#include "main-window.hh"
#include "matrix-model.hh"
#include "matrix-view.hh"

#include <QBoxLayout>
#include <QDebug>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>

CPropertiesDialog::CPropertiesDialog(QWidget *p_parent) : QDialog(p_parent), m_parent(qobject_cast<CMainWindow *>(p_parent))
{
    setWindowTitle(tr("Matrix properties"));

    CMatrixModel *model = parent()->currentModel();

    if (model == nullptr)
    {
        parent()->showMessage(tr("Can't display properties of invalid models"));
        close();
    }
    else
    {
        QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
        connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));

        QTableWidgetItem *item;

        // model info
        const int nbProperties = (model->channels() == 1) ? 10 : 5;

        QTableWidget *matrixInfo = createPropertyTable(nbProperties, 2);

        int row = 0;
        item    = new QTableWidgetItem(tr("File"));
        matrixInfo->setItem(row, 0, item);

        item = new QTableWidgetItem(model->filePath());
        matrixInfo->setItem(row, 1, item);

        ++row;
        item = new QTableWidgetItem(tr("Rows"));
        matrixInfo->setItem(row, 0, item);

        item = new QTableWidgetItem(QString::number(model->rowCount()));
        matrixInfo->setItem(row, 1, item);

        ++row;
        item = new QTableWidgetItem(tr("Columns"));
        matrixInfo->setItem(row, 0, item);

        item = new QTableWidgetItem(QString::number(model->columnCount()));
        matrixInfo->setItem(row, 1, item);

        ++row;
        item = new QTableWidgetItem(tr("Type"));
        matrixInfo->setItem(row, 0, item);

        item = new QTableWidgetItem(model->typeString());
        matrixInfo->setItem(row, 1, item);

        ++row;
        item = new QTableWidgetItem(tr("Channels"));
        matrixInfo->setItem(row, 0, item);

        item = new QTableWidgetItem(QString::number(model->channels()));
        matrixInfo->setItem(row, 1, item);

        ++row;
        item = new QTableWidgetItem(tr("Elements"));
        matrixInfo->setItem(row, 0, item);

        item = new QTableWidgetItem(QString::number(model->total()));
        matrixInfo->setItem(row, 1, item);

        // Properties only available for single-channel matrices
        if (model->channels() == 1)
        {
            ++row;
            item = new QTableWidgetItem(tr("Non-zeros"));
            matrixInfo->setItem(row, 0, item);

            item = new QTableWidgetItem(QString::number(model->countNonZeros()));
            matrixInfo->setItem(row, 1, item);

            double min, max;
            QPoint minLoc, maxLoc;
            model->minMaxLoc(&min, &max, &minLoc, &maxLoc);

            ++row;
            item = new QTableWidgetItem(tr("Min"));
            matrixInfo->setItem(row, 0, item);

            item = new QTableWidgetItem(QString("%1 @ [row: %2, col: %3]").arg(min).arg(minLoc.y()).arg(minLoc.x()));
            matrixInfo->setItem(row, 1, item);

            ++row;
            item = new QTableWidgetItem(tr("Max"));
            matrixInfo->setItem(row, 0, item);

            item = new QTableWidgetItem(QString("%1 @ [row: %2, col: %3]").arg(max).arg(maxLoc.y()).arg(maxLoc.x()));
            matrixInfo->setItem(row, 1, item);

            double mean, stddev;
            model->meanStdDev(&mean, &stddev);

            ++row;
            item = new QTableWidgetItem(tr("Mean"));
            matrixInfo->setItem(row, 0, item);

            item = new QTableWidgetItem(QString::number(mean));
            matrixInfo->setItem(row, 1, item);

            ++row;
            item = new QTableWidgetItem(tr("StdDev"));
            matrixInfo->setItem(row, 0, item);

            item = new QTableWidgetItem(QString::number(stddev));
            matrixInfo->setItem(row, 1, item);
        }

        const QVector<CProperty> &properties = model->metadata().properties();

        QTableWidget *headerInfo = createPropertyTable(properties.count(), 2);

        row = 0;
        for (const CProperty &property : properties)
        {
            ++row;
            item = new QTableWidgetItem(property.key());
            headerInfo->setItem(row, 0, item);

            item = new QTableWidgetItem(property.value());
            headerInfo->setItem(row, 1, item);
        }

        QBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addWidget(new QLabel(tr("Statistics")));
        mainLayout->addWidget(matrixInfo, 1);

        if (!properties.isEmpty())
        {
            mainLayout->addWidget(new QLabel(tr("Header")));
            mainLayout->addWidget(headerInfo, 1);
        }

        mainLayout->addWidget(buttons);
        setLayout(mainLayout);
        resize(400, 400);
    }
}

CPropertiesDialog::~CPropertiesDialog() { }

CMainWindow *CPropertiesDialog::parent() const
{
    if (m_parent == nullptr)
    {
        qWarning() << "CPropertiesDialog::parent invalid parent";
    }

    return m_parent;
}

QTableWidget *CPropertiesDialog::createPropertyTable(const int p_rows, const int p_columns)
{
    QTableWidget *table = new QTableWidget(p_rows, p_columns, this);
    table->setAlternatingRowColors(true);
    table->setHorizontalHeaderLabels(QStringList() << tr("Property") << tr("Value"));
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::AllEditTriggers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setColumnWidth(0, 150);
    return table;
}
