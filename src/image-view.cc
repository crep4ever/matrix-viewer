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
#include "image-view.hh"

#include "histogram-widget.hh"
#include "main-window.hh"
#include "matrix-model.hh"
#include "position.hh"

#include <QAction>
#include <QDebug>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QWheelEvent>

CImageView::CImageView(QWidget *p_parent)
    : QGraphicsView(p_parent)
    , m_parent(qobject_cast<CMainWindow *>(p_parent))
    , m_model(nullptr)
    , m_image(nullptr)
    , m_histogramWidget(nullptr)
    , m_histogramNeedsRedraw(true)
    , m_scene(new QGraphicsScene)
    , m_selectionBox(new QGraphicsRectItem(0, 0, 1, 1))
    , m_zoomInAct(nullptr)
    , m_zoomOutAct(nullptr)
    , m_normalSizeAct(nullptr)
    , m_fitToWindowAct(nullptr)
    , m_histogramAct(nullptr)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setBackgroundRole(QPalette::Dark);

    m_selectionBox->setBrush(QBrush(QColor(255, 0, 0, 100)));
    m_selectionBox->setPen(Qt::NoPen);

    setScene(m_scene);

    createActions();
}

CImageView::~CImageView()
{
    delete m_image;
    delete m_histogramWidget;
    delete m_selectionBox;
    delete m_scene;
}

void CImageView::createActions()
{
    m_zoomInAct = new QAction(tr("Zoom &In"), this);
    m_zoomInAct->setIcon(QIcon::fromTheme("zoom-in"));
    m_zoomInAct->setShortcut(QKeySequence::ZoomIn);
    m_zoomInAct->setStatusTip(tr("Zoom in"));
    connect(m_zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    m_zoomOutAct = new QAction(tr("Zoom &Out"), this);
    m_zoomOutAct->setIcon(QIcon::fromTheme("zoom-out"));
    m_zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    m_zoomOutAct->setStatusTip(tr("Zoom out"));
    connect(m_zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    m_fitToWindowAct = new QAction(tr("&Fit window"), this);
    m_fitToWindowAct->setIcon(QIcon::fromTheme("zoom-fit-best"));
    m_fitToWindowAct->setStatusTip(tr("Fit the image within the window"));
    connect(m_fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

    m_normalSizeAct = new QAction(tr("&Original size"), this);
    m_normalSizeAct->setIcon(QIcon::fromTheme("zoom-original"));
    m_normalSizeAct->setStatusTip(tr("Original size of the image"));
    connect(m_normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    m_histogramAct = new QAction(tr("&Histogram"), this);
    m_histogramAct->setStatusTip(tr("Histogram"));
    m_histogramAct->setCheckable(true);
    m_histogramAct->setChecked(false);
    connect(m_histogramAct, SIGNAL(toggled(bool)), this, SLOT(toggleHistogram(bool)));
}

CMatrixModel *CImageView::model() const
{
    return m_model;
}

void CImageView::setModel(CMatrixModel *p_model)
{
    if (p_model != m_model)
    {
        m_model = p_model;

        connect(m_model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(update(const QModelIndex &, const QModelIndex &)));

        update();
    }
}

CMainWindow *CImageView::parent() const
{
    if (m_parent == nullptr)
    {
        qWarning() << "CImageView::parent invalid parent";
    }

    return m_parent;
}

void CImageView::wheelEvent(QWheelEvent *p_event)
{
    if (p_event->delta() > 0)
    {
        zoomIn();
    }
    else
    {
        zoomOut();
    }

    centerOn(mapToScene(p_event->pos()));

    QGraphicsView::wheelEvent(p_event);
}

void CImageView::mousePressEvent(QMouseEvent *p_event)
{
    const QPointF scenePoint = mapToScene(p_event->pos());
    // ensure scenePoint is within the scene range
    if (0 < scenePoint.x() && scenePoint.x() < sceneRect().width() && 0 < scenePoint.y() && scenePoint.y() < sceneRect().height())
    {
        selectItem((int) scenePoint.y(), (int) scenePoint.x());
    }

    QGraphicsView::mousePressEvent(p_event);
}

void CImageView::keyPressEvent(QKeyEvent *p_event)
{
    if (parent() == nullptr)
    {
        return QGraphicsView::keyPressEvent(p_event);
    }

    const int row = parent()->positionWidget()->row();
    const int col = parent()->positionWidget()->col();

    switch (p_event->key())
    {
        case Qt::Key_Left:
            selectItem(row, qMax(0, col - 1));
            break;

        case Qt::Key_Right:
            selectItem(row, qMin((int) sceneRect().width() - 1, col + 1));
            break;

        case Qt::Key_Down:
            selectItem(qMin((int) sceneRect().height() - 1, row + 1), col);
            break;

        case Qt::Key_Up:
            selectItem(qMax(0, row - 1), col);
            break;

        default:
            QGraphicsView::keyPressEvent(p_event);
    }
}

void CImageView::selectItem(int p_row, int p_col)
{
    int r = qMax(0, p_row);
    r     = qMin(r, model()->rowCount() - 1);

    int c = qMax(0, p_col);
    c     = qMin(c, model()->columnCount() - 1);

    m_selectionBox->setPos(c, r);

    if (parent())
    {
        parent()->positionWidget()->setRow(r);
        parent()->positionWidget()->setCol(c);

        const QModelIndex index = m_model->index(r, c);
        const QString value     = m_model->data(index, Qt::DisplayRole).toString();
        parent()->positionWidget()->setValue(value);
    }
}

void CImageView::zoomIn()
{
    scale(1.2, 1.2);
}

void CImageView::zoomOut()
{
    scale(1 / 1.2, 1 / 1.2);
}

void CImageView::normalSize()
{
    resetTransform();
}

void CImageView::fitToWindow()
{
    fitInView(sceneRect(), Qt::KeepAspectRatio);
}

void CImageView::bestSize()
{
    if (m_scene->sceneRect().width() > width() || m_scene->sceneRect().height() > height())
    {
        fitToWindow();
    }
}

void CImageView::contextMenuEvent(QContextMenuEvent *p_event)
{
    QMenu *menu = new QMenu;

    menu->addAction(m_zoomInAct);
    menu->addAction(m_zoomOutAct);
    menu->addAction(m_fitToWindowAct);
    menu->addAction(m_normalSizeAct);
    menu->addSeparator();
    menu->addAction(m_histogramAct);

    menu->exec(p_event->globalPos());
    delete menu;
}

void CImageView::toggleHistogram(bool p_visible)
{
    if (p_visible && m_histogramNeedsRedraw)
    {
        if (!m_histogramWidget)
        {
            m_histogramWidget = new CHistogramWidget(this);
        }

        m_histogramWidget->setImage(m_image);
        m_histogramNeedsRedraw = false;
    }

    m_histogramWidget->setVisible(p_visible);
}

void CImageView::draw()
{
    // reset scene
    m_scene->clear();

    // delete previous image
    if (m_image != nullptr)
    {
        delete m_image;
    }

    // set p_image as current image
    m_image = model()->toQImage();

    // rebuild histogram
    if (m_histogramAct->isChecked())
    {
        if (m_histogramWidget == nullptr)
        {
            m_histogramWidget = new CHistogramWidget(this);
        }

        m_histogramWidget->setImage(m_image);
        m_histogramNeedsRedraw = false;
        m_histogramWidget->setVisible(true);
    }
    else
    {
        m_histogramNeedsRedraw = true;
    }

    // rebuild selectionbox
    m_selectionBox = new QGraphicsRectItem(0, 0, 1, 1);
    m_selectionBox->setBrush(QBrush(QColor(255, 0, 0, 100)));
    m_selectionBox->setPen(Qt::NoPen);

    // rebuild scene
    m_scene->setSceneRect(QRect(0, 0, m_image->width(), m_image->height()));
    m_scene->addPixmap(QPixmap::fromImage(*m_image));
    m_scene->addItem(m_selectionBox);
}

void CImageView::update(const QModelIndex &p_begin, const QModelIndex &p_end)
{
    Q_UNUSED(p_begin);
    Q_UNUSED(p_end);

    draw();
}
