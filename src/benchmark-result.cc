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

#include "benchmark-result.hh"

#include <QDebug>
#include <utility>

BenchmarkResult::BenchmarkResult() : QObject(), m_title("invalid"), m_nsMin(0), m_nsMax(0), m_nsAvg(0), m_status(Ignored) { }

BenchmarkResult::BenchmarkResult(QString p_name) : QObject(), m_title(std::move(p_name)), m_nsMin(0), m_nsMax(0), m_nsAvg(0), m_status(Ignored) { }

BenchmarkResult::BenchmarkResult(const BenchmarkResult& p_other)
    : QObject()
    , m_title(p_other.title())
    , m_nsMin(p_other.nsMin())
    , m_nsMax(p_other.nsMax())
    , m_nsAvg(p_other.nsAvg())
    , m_status(p_other.status())
{
}

BenchmarkResult::~BenchmarkResult() { }

const QString& BenchmarkResult::title() const
{
    return m_title;
}

void BenchmarkResult::setTitle(const QString& p_str)
{
    m_title = p_str;
}

double BenchmarkResult::nsMin() const
{
    return m_nsMin;
}

void BenchmarkResult::setNsMin(const double p_value)
{
    m_nsMin = p_value;
}

double BenchmarkResult::nsMax() const
{
    return m_nsMax;
}

void BenchmarkResult::setNsMax(const double p_value)
{
    m_nsMax = p_value;
}

double BenchmarkResult::nsAvg() const
{
    return m_nsAvg;
}

void BenchmarkResult::setNsAvg(const double p_value)
{
    m_nsAvg = p_value;
}

BenchmarkResult::Status BenchmarkResult::status() const
{
    return m_status;
}

void BenchmarkResult::setStatus(const Status p_status)
{
    m_status = p_status;
}

QString BenchmarkResult::statusStr() const
{
    switch (m_status)
    {
        case Success:
            return "success";

        case Error:
            return "error";

        case Canceled:
            return "canceled";

        case Ignored:
            return "ignored";
    }

    return QString();
}

QString BenchmarkResult::timeStr() const
{
    QString unit = "ns";
    double min   = m_nsMin;
    double max   = m_nsMax;
    double avg   = m_nsAvg;

    const double us = 1000;
    const double ms = us * 1000;
    const double s  = ms * 1000;
    if (min > s)
    {
        min /= s;
        max /= s;
        avg /= s;
        unit = "s";
    }
    else if (min > ms)
    {
        min /= ms;
        max /= ms;
        avg /= ms;
        unit = "ms";
    }
    else if (min > us)
    {
        min /= us;
        max /= us;
        avg /= us;
        unit = "µs";
    }

    return QString("avg: %1 %4\nmin: %2 %4\nmax: %3 %4\n").arg(avg).arg(min).arg(max).arg(unit);
}
