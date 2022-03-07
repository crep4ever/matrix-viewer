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
#include "new-matrix-dialog.hh"

#include <QBoxLayout>
#include <QCloseEvent>
#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QSettings>
#include <QSpinBox>
#include <cfloat>

NewMatrixDialog::NewMatrixDialog(QWidget *p_parent)
    : QDialog(p_parent)
    , m_rows(nullptr)
    , m_cols(nullptr)
    , m_channels(nullptr)
    , m_type(nullptr)
    , m_value1(nullptr)
    , m_value2(nullptr)
    , m_value3(nullptr)
{
    m_rows = new QSpinBox;
    m_rows->setMaximum(INT_MAX);
    m_rows->setToolTip(tr("Number of rows (matrix height)"));

    m_cols = new QSpinBox;
    m_cols->setMaximum(INT_MAX);
    m_cols->setToolTip(tr("Number of cols (matrix width)"));

    m_channels = new QSpinBox;
    m_channels->setRange(1, 3);
    m_channels->setToolTip(tr("Number of channels (matrix depth)"));

    m_type = new QComboBox;
    m_type->addItem("8U");
    m_type->addItem("8S");
    m_type->addItem("16U");
    m_type->addItem("16S");
    m_type->addItem("32S");
    m_type->addItem("32F");
    m_type->addItem("64F");
    m_type->setToolTip(tr("OpenCV type (matrix format)"));

    m_value1 = new QDoubleSpinBox;
    m_value1->setRange(-DBL_MAX, DBL_MAX);
    m_value1->setToolTip(tr("Default initial value."));

    m_value2 = new QDoubleSpinBox;
    m_value2->setRange(-DBL_MAX, DBL_MAX);
    m_value2->setToolTip(tr("Default initial value."));

    m_value3 = new QDoubleSpinBox;
    m_value3->setRange(-DBL_MAX, DBL_MAX);
    m_value3->setToolTip(tr("Default initial value."));

    QBoxLayout *valueLayout = new QHBoxLayout;
    valueLayout->addWidget(m_value1);
    valueLayout->addWidget(m_value2);
    valueLayout->addWidget(m_value3);

    connect(m_channels, SIGNAL(valueChanged(int)), this, SLOT(channelsChanged(int)));

    readSettings();
    channelsChanged(m_channels->value()); // update m_value<i> visibility

    QFormLayout *userLayout = new QFormLayout;
    userLayout->addRow(tr("Rows"), m_rows);
    userLayout->addRow(tr("Columns"), m_cols);
    userLayout->addRow(tr("Channels"), m_channels);
    userLayout->addRow(tr("Type"), m_type);
    userLayout->addRow(tr("Initial value"), valueLayout);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

    QBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(userLayout);
    mainLayout->addWidget(buttons);

    setLayout(mainLayout);

    setWindowTitle(tr("Create new matrix"));
}

void NewMatrixDialog::readSettings()
{
    QSettings settings;
    settings.beginGroup("new-matrix");
    m_rows->setValue(settings.value("rows", 3).toInt());
    m_cols->setValue(settings.value("cols", 3).toInt());
    m_channels->setValue(settings.value("channels", 1).toInt());
    m_type->setCurrentIndex(settings.value("type", 0).toInt());
    m_value1->setValue(settings.value("value1", 0.0).toDouble());
    m_value2->setValue(settings.value("value2", 0.0).toDouble());
    m_value3->setValue(settings.value("value3", 0.0).toDouble());
    settings.endGroup();
}

void NewMatrixDialog::writeSettings()
{
    QSettings settings;
    settings.beginGroup("new-matrix");
    settings.setValue("rows", m_rows->value());
    settings.setValue("cols", m_cols->value());
    settings.setValue("channels", m_channels->value());
    settings.setValue("type", m_type->currentIndex());
    settings.setValue("value1", m_value1->value());
    settings.setValue("value2", m_value2->value());
    settings.setValue("value3", m_value3->value());
    settings.endGroup();
}

void NewMatrixDialog::accept()
{
    writeSettings();
    QDialog::accept();
}

void NewMatrixDialog::channelsChanged(int p_nbChannels)
{
    switch (p_nbChannels)
    {
        case 1:
            m_value1->setVisible(true);
            m_value2->setVisible(false);
            m_value3->setVisible(false);
            break;

        case 2:
            m_value1->setVisible(true);
            m_value2->setVisible(true);
            m_value3->setVisible(false);
            break;

        case 3:
            m_value1->setVisible(true);
            m_value2->setVisible(true);
            m_value3->setVisible(true);
            break;

        default:
            qWarning() << tr("Invalid number of channels");
            break;
    }
}
