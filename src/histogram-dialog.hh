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

#ifndef __HISTOGRAM_DIALOG_HH__
#define __HISTOGRAM_DIALOG_HH__

#include <QDialog>
#include <QColor>

class CMainWindow;
class CHistogram;
class QBoxLayout;

/*!
  \file histogram-dialog.hh
  \class CHistogramDialog
  \brief CHistogramDialog is a dialog that wraps up the histogram of an image
*/

class CHistogramDialog : public QDialog
{
  Q_OBJECT

public:
  /// Constructor.
  CHistogramDialog(QWidget *parent = 0);

  /// Destructor.
  virtual ~CHistogramDialog();

  CMainWindow* parent() const;

  void setImage(QImage *image);

private:
  QBoxLayout * makeAxisLayout(const QColor & color);


  CMainWindow *m_parent;
  CHistogram *m_redHistogram;
  CHistogram *m_greenHistogram;
  CHistogram *m_blueHistogram;

  static const QColor _red;
  static const QColor _green;
  static const QColor _blue;
};

#endif  // __HISTOGRAM_DIALOG_HH__
