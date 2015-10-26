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

#ifndef __ELAPSED_TIMER_HH__
#define __ELAPSED_TIMER_HH__

#include <QtGlobal>
#include <QObject>
#include <QElapsedTimer>
#include <QDebug>

class CElapsedTimer : public QElapsedTimer
{
public:

  qint64 nsecsElapsed() const
  {

    qint64 ns;

#if QT_VERSION < QT_VERSION_CHECK(4, 8, 0)
    qWarning() << QObject::tr("Current Qt version does not support sub-millisecond timer precision");
    ns = elapsed() * 1000 * 1000;
#else
    ns = nsecsElapsed();
#endif
    
    return ns;
  }
};

#endif // __ELAPSED_TIMER_HH__
