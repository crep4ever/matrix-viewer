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

#include "empty-icon-provider.hh"

#include <QApplication>
#include <QBoxLayout>
#include <QCompleter>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>

CFileChooser::CFileChooser(QWidget *p_parent)
    : QWidget(p_parent)
    , m_lineEdit(new QLineEdit)
    , m_button(new QPushButton)
    , m_caption(QCoreApplication::applicationName())
    , m_directory(QDir::homePath())
    , m_path("")
    , m_filter("")
    , m_options()
    , m_completerModel(new QFileSystemModel(this))
    , m_completer(new QCompleter(this))
{
    const int size = 22;
    m_button->setFixedSize(QSize(size, size)); // never allow a 'vertical' rectangle
    m_button->setText(tr("Browse"));
    m_button->setIcon(QIcon::fromTheme("folder-open"));
    m_button->setIconSize(QSize(22, 22));
    m_button->setStyleSheet(QString("QPushButton:hover {border:none; border-radius: 2px; background-color: lightgrey;} "));
    m_button->setFlat(true);
    connect(m_button, SIGNAL(clicked()), SLOT(browse()));

    m_completerModel->setRootPath(QString());
    m_completerModel->setIconProvider(new CEmptyIconProvider);
    m_completerModel->setNameFilterDisables(false);

    m_completer->setModel(m_completerModel);
    m_completer->setCompletionMode(QCompleter::InlineCompletion);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);

    m_lineEdit->setCompleter(m_completer);
    connect(m_lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setPath(const QString &)));

    QLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_lineEdit);
    mainLayout->addWidget(m_button);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);
}

CFileChooser::~CFileChooser() { }

void CFileChooser::browse()
{
    QString selection;
    if (options() & QFileDialog::ShowDirsOnly)
    {
        selection = QFileDialog::getExistingDirectory(nullptr, caption(), directory());
    }
    else
    {
        selection = QFileDialog::getOpenFileName(nullptr, caption(), directory(), filter(), nullptr, options());
    }

    if (!selection.isEmpty())
    {
        setPath(selection);
    }
}

const QFileDialog::Options &CFileChooser::options() const
{
    return m_options;
}

void CFileChooser::setOptions(const QFileDialog::Options &p_options)
{
    m_options = p_options;
}

const QString &CFileChooser::filter() const
{
    return m_filter;
}

void CFileChooser::setFilter(const QString &p_filter)
{
    m_filter = p_filter;
}

const QString &CFileChooser::caption() const
{
    return m_caption;
}

void CFileChooser::setCaption(const QString &p_caption)
{
    m_caption = p_caption;
}

const QString &CFileChooser::directory() const
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

const QString &CFileChooser::path() const
{
    return m_path;
}

void CFileChooser::setPath(const QString &p_path)
{
    if (QString::compare(m_path, p_path, Qt::CaseSensitive) == 0)
    {
        return;
    }

    if (m_lineEdit->text() != p_path)
    {
        m_lineEdit->setText(p_path);
    }

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
