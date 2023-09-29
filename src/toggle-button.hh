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

#pragma once

#include <QAbstractButton>
#include <QBrush>

class QEvent;
class QPaintEvent;
class QPropertyAnimation;

/** @class CToggleButton
 *  @brief On/Off button with 'mobile' design
 *
 *  A CToggleButton implements a design
 *  that is often found in mobile (android, ios) applications
 *  to enable/disable an option.
 *
 *  Implementation adapted from
 *  [stackoverflow](https://stackoverflow.com/questions/14780517/toggle-switch-in-qt)
 */
class CToggleButton : public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(int offset READ offset WRITE setOffset)

public:
    CToggleButton(const bool p_animated = true, QWidget *p_parent = nullptr);

    QSize sizeHint() const override;

    QBrush brush() const;
    void setBrush(const QBrush &p_brush);

    int offset() const;
    void setOffset(int p_newOffset);

protected:
    void paintEvent(QPaintEvent *) override;
    void enterEvent(QEnterEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void resizeEvent(QResizeEvent *) override;

private:
    qreal m_opacity;
    int m_x, m_y, m_height, m_margin, m_offset;
    QBrush m_thumb, m_track, m_brush;
    bool m_switch;
    QPropertyAnimation *m_animation;
    bool m_isAnimated;
};
