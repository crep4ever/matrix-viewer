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
#include "file-chooser.hh"

#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QLineEdit>
#include <QPushButton>
#include <QBoxLayout>
#include <QCompleter>
#include <QDirModel>
#include <QSettings>

CFileChooser::CFileChooser(QWidget *p_parent) :
QWidget(p_parent)
, m_lineEdit(0)
, m_button(0)
, m_caption(QCoreApplication::applicationName())
, m_directory(QDir::homePath())
, m_path("")
, m_filter("")
, m_options(0)
{
  m_lineEdit = new QLineEdit();
  QCompleter *completer = new QCompleter(this);
  completer->setModel(new QDirModel(completer));
  completer->setCompletionMode(QCompleter::InlineCompletion);
  m_lineEdit->setCompleter(completer);

  connect(m_lineEdit, SIGNAL(textChanged(const QString &)),
  this, SLOT(setPath(const QString &)));

  m_button = new QPushButton(tr("Browse"));
  connect(m_button, SIGNAL(clicked()), SLOT(browse()));

  QLayout *mainLayout = new QHBoxLayout;
  mainLayout->addWidget(m_lineEdit);
  mainLayout->addWidget(m_button);
  // disable layout's margin to have a proper "one widget" appearance
  mainLayout->setContentsMargins(0, 0, 0, 0);
  setLayout(mainLayout);
}

CFileChooser::~CFileChooser()
{
}

void CFileChooser::browse()
{
  QString selection;
  if (options() & QFileDialog::ShowDirsOnly)
  selection = QFileDialog::getExistingDirectory(this, caption(), directory());
  else
  selection = QFileDialog::getOpenFileName(this, caption(), directory(), filter(), 0, options());

  if (!selection.isEmpty())
  setPath(selection);
}

QFileDialog::Options CFileChooser::options() const
{
  return m_options;
}

void CFileChooser::setOptions(const QFileDialog::Options &p_opts)
{
  m_options = p_opts;
}

QString CFileChooser::filter() const
{
  return m_filter;
}

void CFileChooser::setFilter(const QString &p_filter)
{
  m_filter = p_filter;
}

QString CFileChooser::caption() const
{
  return m_caption;
}

void CFileChooser::setCaption(const QString &p_caption)
{
  m_caption = p_caption;
}

QString CFileChooser::directory() const
{
  return m_directory;
}

void CFileChooser::setDirectory(const QString &p_directory)
{
  m_directory = p_directory;
}

void CFileChooser::setDirectory(const QDir &p_directory)
{
  m_directory = p_directory.absolutePath();
}


QString CFileChooser::path() const
{
  return m_path;
}

void CFileChooser::setPath(const QString &p_path)
{
  if (QString::compare(m_path, p_path, Qt::CaseSensitive) == 0)
  return;

  if (m_lineEdit->text() != p_path)
  m_lineEdit->setText(p_path);

  m_path = p_path;

  QFileInfo fileInfo(m_path);
  if (fileInfo.isDir())
  {
    setDirectory(m_path);
  }
  else
  {
    setDirectory(fileInfo.dir());
  }

  emit(pathChanged(m_path));
}
