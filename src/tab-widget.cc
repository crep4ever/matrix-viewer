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
#include "tab-widget.hh"

#include "main-window.hh"
#include "matrix-converter.hh"
#include "tab.hh"

#include <QDebug>
#include <QFileInfo>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>

CTabWidget::CTabWidget(QWidget *p_parent) : QTabWidget(p_parent), m_dragActiveState(false)
{
    setStyleSheet(" QTabWidget::tab-bar {}");
    setTabBar(new CTabBar(this));
    setMinimumSize(QSize(450, 380));
    setAcceptDrops(true);

    connect(this, SIGNAL(tabCloseRequested(int)), SLOT(updateAfterClosedTab(int)));
}

CTabWidget::~CTabWidget() { }

void CTabWidget::changeTabText(const QString &p_str)
{
    setTabText(currentIndex(), p_str);
}

void CTabWidget::addTab(QWidget *p_page, const QString &p_label)
{
    QFileInfo fi(p_label);
    CTab *tab = qobject_cast<CTab *>(p_page);
    if (tab != nullptr)
    {
        tab->setFilePath(fi.absoluteFilePath());
        tab->setWindowTitle(fi.fileName());
        setToolTip(fi.absoluteFilePath());
        QTabWidget::addTab(p_page, fi.fileName());
    }
    else
    {
        QTabWidget::addTab(p_page, p_label);
    }
}

void CTabWidget::paintEvent(QPaintEvent *p_event)
{
    QTabWidget::paintEvent(p_event);

    if (count() <= 0)
    {
        QPainter painter(this);
        QFont customFont = painter.font();
        customFont.setPointSizeF(customFont.pointSizeF() * 2);
        painter.setFont(customFont);

        const int padding  = 8;
        QRect externalRect = rect().adjusted(padding, padding, -padding, -padding);

        const QColor borderColor = m_dragActiveState ? QColor(138, 226, 52) : Qt::gray;
        painter.setPen(QPen(borderColor, 2, Qt::DotLine));
        painter.setBrush(Qt::white);
        painter.drawRoundedRect(externalRect, 2, 2);

        QString message = tr("Open or drag and drop image files here\n");

        QRect topRect = QRect(QPoint(externalRect.left(), 40), QPoint(externalRect.right(), 100));
        painter.setPen(Qt::gray);
        painter.drawText(topRect, Qt::AlignCenter | Qt::TextWordWrap, message);

        QVector<Format> formats;
        formats.append(Format("Standard images", "*.tif, *.png, *.jpg, *.webp, *.bmp ..."));
        formats.append(Format("OpenCV file storage", "*.xml, *.yaml, *.json"));
        formats.append(Format("Matrix Format Exchange", "*.mfe"));
        formats.append(Format("European Data Format", "*.edf"));

        QString availableFormats = tr("Supported file formats:\n");
        for (auto format : formats)
        {
            availableFormats.append(QString("\n ● %1 (%2)").arg(format.name, format.extensions));
        }

        const QPoint detailsPadding(40, 40);
        const QRect detailsRect = QRect(topRect.bottomLeft() + detailsPadding, externalRect.bottomRight() - detailsPadding);
        painter.setPen(Qt::gray);

        customFont.setPointSizeF(customFont.pointSizeF() * 0.7);
        painter.setFont(customFont);
        painter.drawText(detailsRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap, availableFormats);
    }
}

void CTabWidget::dragEnterEvent(QDragEnterEvent *p_event)
{
    const auto &urls = p_event->mimeData()->urls();

    bool accepted = false;
    for (const QUrl &url : urls)
    {
        if (CMatrixConverter::isFilenameSupported(url.toLocalFile()))
        {
            accepted = true;
            break;
        }
    }

    m_dragActiveState = accepted;

    if (accepted)
    {
        p_event->acceptProposedAction();
    }
    else
    {
        p_event->setDropAction(Qt::IgnoreAction);
        p_event->ignore();
    }
    update();
}

void CTabWidget::dragLeaveEvent(QDragLeaveEvent *p_event)
{
    QTabWidget::dragLeaveEvent(p_event);
    m_dragActiveState = false;
    update();
}

void CTabWidget::dropEvent(QDropEvent *p_event)
{
    CMainWindow *mainWindow = qobject_cast<CMainWindow *>(parentWidget());
    if (mainWindow)
    {
        mainWindow->dropEvent(p_event);
    }
}

void CTabWidget::updateAfterClosedTab(int p_tabIndex)
{
    Q_UNUSED(p_tabIndex);
    m_dragActiveState = false;
    update();
}

//----------------------------------------------------------------------------

CTabBar::CTabBar(QWidget *p_parent) : QTabBar(p_parent) { }

CTabBar::~CTabBar() { }

void CTabBar::mouseReleaseEvent(QMouseEvent *p_event)
{
    if (p_event->button() == Qt::MiddleButton)
    {
        emit(tabCloseRequested(tabAt(p_event->pos())));
    }
    QTabBar::mouseReleaseEvent(p_event);
}
