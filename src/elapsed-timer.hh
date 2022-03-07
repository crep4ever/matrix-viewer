// Copyright (C) 2015, Romain Goffe <romain.goffe@gmail.com>
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

#include <QDebug>
#include <QElapsedTimer>
#include <QObject>
#include <QtGlobal>

/*!
  \file elapsed-timer.hh
  \class CElapsedTimer
  \brief Extension of QElapsedTimer that fallbacks in millisecond precision for Qt < 4.8

  The class QElapsedTimer provides nanosecond accuracy since Qt 4.8
  through the QElapsedTimer::nsecsElapsed() method.

  This class fakes a nsecsElapsed() for Qt < 4.8
  that actually have millisecond precision through QElapsedTimer::elapsed().
*/
class CElapsedTimer : public QElapsedTimer
{
public:
    qint64 nsecsElapsed() const
    {
        qint64 ns;

#if QT_VERSION < QT_VERSION_CHECK(4, 8, 0)
        qWarning() << QObject::tr("Current Qt version does not support sub-millisecond timer precision");
        ns = QElapsedTimer::elapsed() * 1000 * 1000;
#else
        ns = QElapsedTimer::nsecsElapsed();
#endif

        return ns;
    }
};
