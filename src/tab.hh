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

#pragma once

#include <QSplitter>
#include <QModelIndex>
#include <QString>

/*!
  \file tab.hh
  \class CTab
  \brief CTab is the main widget of a tab: a splitter that may display different data views
*/

class CTab : public QSplitter
{
  Q_OBJECT

public:
  /// Constructor.
  CTab();

  /// Destructor.
  ~CTab() override;

  virtual void addWidget(QWidget *p_widget);

  bool isModified() const;
  void setModified(const bool p_modified);

  QString filePath() const;
  void setFilePath(const QString & p_path);

signals:
  void labelChanged(const QString &);
  void wasModified();

public slots:
  void modelDataChanged(const QModelIndex &, const QModelIndex &);
  void selectItem(int, int);

private:
  bool m_isModified;
  QString m_filePath;
};
