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
#include "histogram-dialog.hh"

#include <QDialogButtonBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QBoxLayout>
#include <QDebug>

#include "main-window.hh"
#include "histogram.hh"

CHistogramDialog::CHistogramDialog(QWidget *p)
  : QDialog(p)
  , m_parent(qobject_cast<CMainWindow*>(p))
  , m_redHistogram(new CHistogram)
  , m_greenHistogram(new CHistogram)
  , m_blueHistogram(new CHistogram)
{
  setWindowTitle(tr("Histogram"));

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
  connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));

  QBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(m_redHistogram);
  layout->addWidget(m_greenHistogram);
  layout->addWidget(m_blueHistogram);
  layout->addStretch();
  layout->addWidget(buttons);
  setLayout(layout);

  resize(QSize(400, 100));
}

CHistogramDialog::~CHistogramDialog()
{
}

void CHistogramDialog::setImage(QImage *image)
{
  QImage copy = image->convertToFormat(QImage::Format_ARGB32);
  QVector<int> redValues(256, 0), greenValues(256, 0), blueValues(256, 0);
  for (int j = 0; j < image->height(); ++j)
    {
      QRgb *row = (QRgb *)copy.scanLine(j);
      for (int i = 0; i < image->width(); ++i)
	{
	  const QColor color = QColor(row[i]);
	  ++redValues[color.red()];
	  ++greenValues[color.green()];
	  ++blueValues[color.blue()];
	}
    }

  m_redHistogram->setValues(redValues, Qt::NoPen, QBrush(QColor(239, 41, 41)));
  m_greenHistogram->setValues(greenValues, Qt::NoPen, QBrush(QColor(138, 226, 52)));
  m_blueHistogram->setValues(blueValues, Qt::NoPen, QBrush(QColor(114, 159, 207)));
}

CMainWindow* CHistogramDialog::parent() const
{
  if (!m_parent)
    qWarning() << "CHistogramDialog::parent invalid parent";
  return m_parent;
}
