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
#include "preferences.hh"

#include <QBoxLayout>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFontDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSettings>
#include <QSpinBox>
#include <QStackedWidget>

#include "main-window.hh"
#include "file-chooser.hh"

#include <QDebug>

// Config Dialog

ConfigDialog::ConfigDialog(QWidget* parent) :
QDialog(parent)
, m_contentsWidget(0)
, m_pagesWidget(0)
{
  m_contentsWidget = new QListWidget(this);
  m_contentsWidget->setViewMode(QListView::IconMode);
  m_contentsWidget->setIconSize(QSize(62, 62));
  m_contentsWidget->setMovement(QListView::Static);
  m_contentsWidget->setSpacing(12);
  m_contentsWidget->setFixedWidth(110);

  m_pagesWidget = new QStackedWidget(this);
  m_pagesWidget->addWidget(new DisplayPage(this));
  m_pagesWidget->addWidget(new ImagePage(this));

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
  connect(buttons, SIGNAL(rejected()), this, SLOT(close()));

  createIcons();
  m_contentsWidget->setCurrentRow(0);

  QBoxLayout *horizontalLayout = new QHBoxLayout;
  horizontalLayout->addWidget(m_contentsWidget);
  horizontalLayout->addWidget(m_pagesWidget, 1);

  QBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(horizontalLayout);
  mainLayout->addSpacing(12);
  mainLayout->addWidget(buttons);

  setLayout(mainLayout);
  setWindowTitle(tr("Preferences"));
  resize(600,600);
}

CMainWindow* ConfigDialog::parent() const
{
  CMainWindow *p = qobject_cast<CMainWindow*>(QDialog::parent());
  if (!p)
  {
    qWarning() << tr("ConfigDialog::parent() invalid parent");
  }

  return p;
}

void ConfigDialog::createIcons()
{
  QListWidgetItem *displayButton = new QListWidgetItem(m_contentsWidget);
  displayButton->setIcon(QIcon::fromTheme("preferences-desktop", QIcon(":/icons/tango/48x48/categories/preferences-desktop.png")));
  displayButton->setText(tr("Display"));
  displayButton->setTextAlignment(Qt::AlignHCenter);
  displayButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

  QListWidgetItem *imageButton = new QListWidgetItem(m_contentsWidget);
  imageButton->setIcon(QIcon::fromTheme("image-x-generic"));
  imageButton->setText(tr("Image"));
  imageButton->setTextAlignment(Qt::AlignHCenter);
  imageButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

  connect(m_contentsWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
  this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
}

void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
  if (!current)
  {
    current = previous;
  }

  m_pagesWidget->setCurrentIndex(m_contentsWidget->row(current));
}

void ConfigDialog::closeEvent(QCloseEvent *event)
{
  Q_UNUSED(event);
  for ( int i = 0 ; i < m_pagesWidget->count() ; ++i )
  {
    m_pagesWidget->widget(i)->close();
  }
}


// Page

Page::Page(QWidget *parent) :
QScrollArea(parent)
, m_content(new QWidget)
{
}

ConfigDialog * Page::parent() const
{
  ConfigDialog *p = qobject_cast<ConfigDialog*>(QScrollArea::parent());
  if (!p) qWarning() << tr("Page::parent() invalid parent");
  return p;
}

void Page::closeEvent(QCloseEvent *event)
{
  writeSettings();
  event->accept();
}

void Page::readSettings()
{
}

void Page::writeSettings()
{
}

void Page::setLayout(QLayout *layout)
{
  m_content->setLayout(layout);
  setWidget(m_content);
}

// Display Page

DisplayPage::DisplayPage(QWidget *parent) :
Page(parent)
, m_statusBarCheckBox(0)
, m_toolBarCheckBox(0)
{
  QGroupBox *displayApplicationGroupBox = new QGroupBox(tr("Application"));
  m_statusBarCheckBox = new QCheckBox(tr("Status bar"));
  m_toolBarCheckBox = new QCheckBox(tr("Tool bar"));

  QVBoxLayout *displayApplicationLayout = new QVBoxLayout;
  displayApplicationLayout->addWidget(m_statusBarCheckBox);
  displayApplicationLayout->addWidget(m_toolBarCheckBox);
  displayApplicationGroupBox->setLayout(displayApplicationLayout);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(displayApplicationGroupBox);
  mainLayout->addStretch(1);
  setLayout(mainLayout);

  readSettings();
}

void DisplayPage::readSettings()
{
  QSettings settings;
  settings.beginGroup("display");
  m_statusBarCheckBox->setChecked(settings.value("statusBar", true).toBool());
  m_toolBarCheckBox->setChecked(settings.value("toolBar", true).toBool());
  settings.endGroup();
}

void DisplayPage::writeSettings()
{
  QSettings settings;
  settings.beginGroup("display");
  settings.setValue("statusBar", m_statusBarCheckBox->isChecked());
  settings.setValue("toolBar", m_toolBarCheckBox->isChecked());
  settings.endGroup();
}

// Image Page

ImagePage::ImagePage(QWidget *parent) :
Page(parent)
, m_stretchDynamic(0)
, m_rawType(0)
, m_rawWidth(0)
, m_rawHeight(0)
, m_rawLittleEndianByteOrder(0)
{
  QGroupBox *displayGroupBox = new QGroupBox(tr("Display options"));
  m_stretchDynamic = new QCheckBox;
  m_stretchDynamic->setEnabled(true);

  QFormLayout *displayLayout = new QFormLayout;
  displayLayout->addRow(tr("Stretch dynamic"), m_stretchDynamic);
  displayGroupBox->setLayout(displayLayout);

  QGroupBox *rawGroupBox = new QGroupBox(tr("Raw images"));
  m_rawType = new QComboBox;
  m_rawType->addItem(tr("16-bit unsigned"));

  m_rawWidth = new QSpinBox;
  m_rawWidth->setMaximum(20000);

  m_rawHeight = new QSpinBox;
  m_rawHeight->setMaximum(20000);

  m_rawLittleEndianByteOrder = new QCheckBox;
  m_rawLittleEndianByteOrder->setEnabled(false);

  QFormLayout *rawLayout = new QFormLayout;
  rawLayout->addRow(tr("Type"), m_rawType);
  rawLayout->addRow(tr("Width"), m_rawWidth);
  rawLayout->addRow(tr("Height"), m_rawHeight);
  rawLayout->addRow(tr("Little-endian byte order"), m_rawLittleEndianByteOrder);
  rawGroupBox->setLayout(rawLayout);

  QBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(rawGroupBox);
  mainLayout->addWidget(displayGroupBox);

  setLayout(mainLayout);

  readSettings();
}

void ImagePage::readSettings()
{
  QSettings settings;
  settings.beginGroup("image");
  m_stretchDynamic->setChecked(settings.value("stretch-dynamic", true).toBool());
  m_rawType->setCurrentIndex(settings.value("raw-type", 0).toInt());
  m_rawWidth->setValue(settings.value("raw-width", 2160).toInt());
  m_rawHeight->setValue(settings.value("raw-height", 1944).toInt());
  m_rawLittleEndianByteOrder->setChecked(settings.value("raw-little-endian", true).toBool());
  settings.endGroup();
}

void ImagePage::writeSettings()
{
  QSettings settings;
  settings.beginGroup("image");
  settings.setValue("stretch-dynamic", m_stretchDynamic->isChecked());
  settings.setValue("raw-type", m_rawType->currentIndex());
  settings.setValue("raw-width", m_rawWidth->value());
  settings.setValue("raw-height", m_rawHeight->value());
  settings.setValue("raw-little-endian", m_rawLittleEndianByteOrder->isChecked());
  settings.endGroup();
}
