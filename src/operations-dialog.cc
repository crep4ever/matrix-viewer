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
#include "operations-dialog.hh"

#include "main-window.hh"
#include "matrix-model.hh"
#include "operations-widget.hh"
#include "tab.hh"

#include <QBoxLayout>
#include <QDebug>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QSettings>
#include <QStackedWidget>

COperationsDialog::COperationsDialog(QWidget *p_parent)
    : QDialog(p_parent)
    , m_parent(qobject_cast<CMainWindow *>(p_parent))
    , m_backup(nullptr)
    , m_categoriesWidget(nullptr)
    , m_operationsWidget(nullptr)
{
    setWindowTitle(tr("Operations"));

    m_categoriesWidget = new QListWidget(this);
    m_categoriesWidget->setViewMode(QListView::IconMode);
    m_categoriesWidget->setIconSize(QSize(62, 62));
    m_categoriesWidget->setMovement(QListView::Static);
    m_categoriesWidget->setSpacing(12);
    m_categoriesWidget->setFixedWidth(125);
    m_categoriesWidget->setCurrentRow(0);

    m_operationsWidget = new QStackedWidget(this);
    m_operationsWidget->addWidget(new CFormatWidget(tr("Format"), model(), m_parent));
    m_operationsWidget->addWidget(new CScalarWidget(tr("Scalar"), model(), m_parent));
    m_operationsWidget->addWidget(new CMatrixWidget(tr("Matrix"), model(), m_parent));
    m_operationsWidget->addWidget(new CTransformationsWidget(tr("Transform"), model(), m_parent));
    m_operationsWidget->addWidget(new CRotationWidget(tr("Rotate"), model(), m_parent));
    m_operationsWidget->addWidget(new CNormalizeWidget(tr("Normalize"), model(), m_parent));
    m_operationsWidget->addWidget(new CThresholdWidget(tr("Threshold"), model(), m_parent));
    m_operationsWidget->addWidget(new CColorMapWidget(tr("ColorMap"), model(), m_parent));
    m_operationsWidget->addWidget(new CChannelsWidget(tr("Channels"), model(), m_parent));
    createIcons();

    QPushButton *okButton = new QPushButton(tr("&OK"));
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    QPushButton *resetButton = new QPushButton(tr("&Reset"));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(reset()));

    QDialogButtonBox *buttons = new QDialogButtonBox;
    buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
    buttons->addButton(resetButton, QDialogButtonBox::ResetRole);

    QBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(m_categoriesWidget);
    horizontalLayout->addWidget(m_operationsWidget, 1);

    QBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(buttons);

    setLayout(mainLayout);
    adjustSize();

    // save a backup of the model data
    // that can be restored by the reset button
    m_backup = model()->clone();
}

COperationsDialog::~COperationsDialog()
{
    delete m_backup;
}

QSize COperationsDialog::sizeHint() const
{
    return {600, 350};
}

void COperationsDialog::reset()
{
    model()->setData(m_backup->data().clone());

    for (int i = 0; i < m_operationsWidget->count(); ++i)
    {
        qobject_cast<COperationWidget *>(m_operationsWidget->widget(i))->reset();
    }
}

CMainWindow *COperationsDialog::parent() const
{
    if (m_parent == nullptr)
    {
        qWarning() << "COperationsDialog::parent invalid parent";
    }

    return m_parent;
}

CMatrixModel *COperationsDialog::model() const
{
    return parent()->currentModel();
}

void COperationsDialog::createIcons()
{
    for (int i = 0; i < m_operationsWidget->count(); ++i)
    {
        QListWidgetItem *button = new QListWidgetItem(m_categoriesWidget);
        //displayButton->setIcon(QIcon::fromTheme("preferences-desktop", QIcon(":/icons/tango/48x48/categories/preferences-desktop.png")));
        button->setText(qobject_cast<COperationWidget *>(m_operationsWidget->widget(i))->title());
        button->setTextAlignment(Qt::AlignHCenter);
        button->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    connect(m_categoriesWidget,
            SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this,
            SLOT(changePage(QListWidgetItem *, QListWidgetItem *)));
}

void COperationsDialog::changePage(QListWidgetItem *p_current, QListWidgetItem *p_previous)
{
    if (p_current == nullptr)
    {
        p_current = p_previous;
    }

    m_operationsWidget->setCurrentIndex(m_categoriesWidget->row(p_current));
    adjustSize();
}
