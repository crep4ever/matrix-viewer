// Copyright (C) 2023, Romain Goffe <romain.goffe@gmail.com>
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

#include "toggle-button.hh"

#include <QDebug>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QTimer>

CToggleButton::CToggleButton(const bool p_animated, QWidget *p_parent)
    : QAbstractButton(p_parent)
    , m_opacity(0.0)
    , m_x(0)
    , m_y(0)
    , m_height(12)
    , m_margin(3)
    , m_offset(0)
    , m_thumb(Qt::gray)
    , m_track()
    , m_brush()
    , m_switch(false)
    , m_animation(new QPropertyAnimation(this, "offset", this))
    , m_isAnimated(p_animated)
{
    setCheckable(true);
    setChecked(false);

    // initial x position for 'false' state is on the left
    m_x = m_height / 2;
    m_y = m_height / 2;
    setOffset(m_x);
    setBrush(QColor(78, 154, 6)); // tango dark green
}

QBrush CToggleButton::brush() const
{
    return m_brush;
}

void CToggleButton::setBrush(const QBrush &p_brush)
{
    m_brush = p_brush;
}

int CToggleButton::offset() const
{
    return m_offset;
}

void CToggleButton::setOffset(int p_newOffset)
{
    m_offset = p_newOffset;
    update();
}

void CToggleButton::paintEvent(QPaintEvent *p_event)
{
    Q_UNUSED(p_event);

    QPainter painter(this);
    painter.setPen(Qt::NoPen);

    const int left  = m_height / 2;
    const int right = width() - m_height;

    if (isChecked())
    {
        m_thumb = m_brush;
        m_x     = right;
    }
    else
    {
        m_thumb = QBrush(Qt::gray);
        m_x     = left;
    }
    if (!m_isAnimated || m_animation->state() == QAbstractAnimation::Stopped)
    {
        m_offset = m_x;
    }

    if (isChecked())
    {
        painter.setBrush(isChecked() ? brush() : Qt::black);
        painter.setOpacity(isChecked() ? 0.5 : 0.38);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawRoundedRect(QRect(m_margin, m_margin, width() - 2 * m_margin, height() - 2 * m_margin), 5.0, 5.0);
        painter.setBrush(m_thumb);
        painter.setOpacity(1.0);
        painter.drawEllipse(QRectF(offset() - left, m_y - (m_height / 2.0), height(), height()));
    }
    else
    {
        painter.setBrush(Qt::gray);
        painter.setOpacity(0.3);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawRoundedRect(QRect(m_margin, m_margin, width() - 2 * m_margin, height() - 2 * m_margin), 5.0, 5.0);
        painter.setOpacity(1.0);
        painter.setBrush(Qt::lightGray);
        painter.drawEllipse(QRectF(offset() - left, m_y - left, height(), height()));
    }
}

void CToggleButton::enterEvent(QEnterEvent *p_event)
{
    setCursor(Qt::PointingHandCursor);
    QAbstractButton::enterEvent(p_event);
}

void CToggleButton::mouseReleaseEvent(QMouseEvent *p_e)
{
    if (p_e->button() & Qt::LeftButton & m_isAnimated)
    {
        //m_switch = !m_switch;

        if (!isChecked())
        {
            m_animation->setStartValue(m_height / 2);
            m_animation->setEndValue(width() - m_height);
            m_animation->setDuration(120);
            m_animation->start();
        }
        else
        {
            m_animation->setStartValue(offset());
            m_animation->setEndValue(m_height / 2);
            m_animation->setDuration(120);
            m_animation->start();
        }
    }

    QAbstractButton::mouseReleaseEvent(p_e);
}

void CToggleButton::resizeEvent(QResizeEvent *)
{
    if (isChecked())
    {
        m_offset = width() - m_height;
    }
}

QSize CToggleButton::sizeHint() const
{
    const int width  = 2 * (m_height + m_margin);
    const int height = m_height + 2 * m_margin;
    return {width, height};
}
