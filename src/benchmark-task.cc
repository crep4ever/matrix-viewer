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

#include "benchmark-task.hh"

#include "elapsed-timer.hh"
#include "matrix-model.hh"

#include <QApplication>
#include <QDebug>
#include <QStringList>
#include <utility>

BenchmarkTask::BenchmarkTask(QString p_operationName, const int p_nbIterations, CMatrixModel* p_model)
    : QObject()
    , m_name(std::move(p_operationName))
    , m_iterations(p_nbIterations)
    , m_model(p_model)
    , m_cancelRequested(false)
{
}

BenchmarkTask::~BenchmarkTask() { }

void BenchmarkTask::execute()
{
    if (m_model == nullptr)
    {
        return;
    }

    m_cancelRequested = false;

    CMatrixModel* ref = m_model->clone();

    BenchmarkResult result(m_name);

    CElapsedTimer timer;

    qint64 ns  = 0;
    double min = DBL_MAX;
    double max = -DBL_MAX;
    double sum = 0;
    for (int i = 0; i < m_iterations; ++i)
    {
        // check for cancel signals from benchmark-dialog
        QApplication::processEvents();

        if (m_cancelRequested)
        {
            result.setStatus(BenchmarkResult::Canceled);
            emit resultReady(result);
            return;
        }

        CMatrixModel* backup = m_model->clone();

        if (m_name == "total")
        {
            timer.start();
            m_model->total();
            ns = timer.nsecsElapsed();
        }
        else if (m_name == "countNonZeros")
        {
            timer.start();
            Q_UNUSED(m_model->countNonZeros());
            ns = timer.nsecsElapsed();
        }
        else if (m_name == "minMaxLoc")
        {
            timer.start();
            double minVal, maxVal;
            QPoint minLoc, maxLoc;
            m_model->minMaxLoc(&minVal, &maxVal, &minLoc, &maxLoc);
            ns = timer.nsecsElapsed();
        }
        else if (m_name == "meanStdDev")
        {
            timer.start();
            double mean, stdDev;
            m_model->meanStdDev(&mean, &stdDev);
            ns = timer.nsecsElapsed();
        }
        else if (m_name == "add")
        {
            timer.start();
            m_model->add(1);
            ns = timer.nsecsElapsed();
        }
        else if (m_name == "multiply")
        {
            timer.start();
            m_model->multiply(2);
            ns = timer.nsecsElapsed();
        }
        else if (m_name == "transpose")
        {
            timer.start();
            m_model->transpose();
            ns = timer.nsecsElapsed();
        }
        else if (m_name == "verticalFlip")
        {
            timer.start();
            m_model->verticalFlip();
            ns = timer.nsecsElapsed();
        }
        else if (m_name == "horizontalFlip")
        {
            timer.start();
            m_model->horizontalFlip();
            ns = timer.nsecsElapsed();
        }
        else if (m_name == "rotate")
        {
            timer.start();
            m_model->rotate(m_model->center(), 60, 1);
            ns = timer.nsecsElapsed();
        }
        else if (m_name == "normalize")
        {
            timer.start();
            m_model->normalize(1, 0, cv::NORM_L2);
            ns = timer.nsecsElapsed();
        }
        else if (m_name == "mulTranspose")
        {
            timer.start();
            m_model->mulTranspose();
            ns = timer.nsecsElapsed();
        }
        else
        {
            qWarning() << "unsupported operation " << m_name;
            delete backup;
            result.setStatus(BenchmarkResult::Ignored);
            emit resultReady(result);
            return;
        }

        m_model->setData(backup->data());
        delete backup;

        if (ns < min)
        {
            min = ns;
        }

        if (ns > max)
        {
            max = ns;
        }
        sum += ns;
    }

    // Check data integrity
    if (!CMatrixModel::compare(m_model, ref))
    {
        qWarning() << "Benchmark of operation" << m_name << " has modified original model";
    }

    delete ref;

    result.setNsMin(min);
    result.setNsMax(max);
    result.setNsAvg(sum / (double) m_iterations);
    result.setStatus(BenchmarkResult::Success);

    emit resultReady(result);
}

void BenchmarkTask::cancel()
{
    m_cancelRequested = true;
}
