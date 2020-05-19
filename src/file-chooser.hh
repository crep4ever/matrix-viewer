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

#pragma once

#include <QWidget>
#include <QString>
#include <QDir>
#include <QFileDialog>

class QLineEdit;
class QPushButton;

/*!
  \file file-chooser.hh
  \class CFileChooser
  \brief CFileChooser is an inline widget to select a path.

  The CFileChooser is composed of two widgets:
    \li a QLineEdit which displays the currently selected path;
    \li a QPushButton which allow to select the path through a dialog.

  \image html file-chooser.png
*/
class CFileChooser : public QWidget
{
  Q_OBJECT

public:
  /// Constructor.
  CFileChooser(QWidget *p_parent = nullptr);

  /// Destructor.
  ~CFileChooser() override;

  /*!
    Returns the options of the QFileDialog .
    \sa setOptions
  */
  const QFileDialog::Options & options() const;

  /*!
    Sets the options of the QFileDialog .
    \sa options
  */
  void setOptions(const QFileDialog::Options & p_options);

  /*!
    Returns the filter of the QFileDialog.
    \sa setFilter
  */
  const QString  & filter() const;

  /*!
    Sets the filter of the QFileDialog.
    \sa filter
  */
  void setFilter(const QString & p_filter);

  /*!
    Returns the caption of the QFileDialog.
    \sa setCaption
  */
  const QString & caption() const;

  /*!
    Sets the caption of the QFileDialog.
    \sa caption
  */
  void setCaption(const QString & p_caption);

  /*!
    Returns the base directory from which the QFileDialog is opened.
    \sa setDirectory
  */
  const QString & directory() const;

  /*!
    Sets the base directory from which the QFileDialog is opened.
    \sa directory
  */
  void setDirectory(const QString & p_directory);

  /*!
    Sets the base directory from which the QFileDialog is opened.
    \sa directory
  */
  void setDirectory(const QDir & p_directory);

  /*!
    Returns the path from the QLineEdit.
    \sa setPath
  */
  const QString & path() const;

public slots:
  /*!
    Sets the path for the QLineEdit.
    \sa path
  */
  void setPath(const QString & p_path);

signals:
  /*!
    This signal is emitted when the path is changed in the QLineEdit.
    \sa path, setPath
  */
  void pathChanged(const QString & p_path);

private slots:
  void browse();


private:
  QLineEdit* m_lineEdit;
  QPushButton* m_button;
  QString m_caption;
  QString m_directory;
  QString m_path;
  QString m_filter;
  QFileDialog::Options m_options;
};
