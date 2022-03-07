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
#include "matrix-model.hh"

#include "logger.hh"

#include <QDebug>
#include <QFile>
#include <QImage>
#include <QSettings>
#include <QXmlStreamReader>

CMatrixModel::CMatrixModel()
    : QAbstractTableModel()
    , m_filePath()
    , m_data()
    , m_format(CMatrixConverter::Format_Unknown)
    , m_horizontalHeaderLabels()
    , m_verticalHeaderLabels()
{
}

CMatrixModel::CMatrixModel(const CMatrixModel &p_other)
    : QAbstractTableModel()
    , m_filePath(p_other.filePath())
    , m_data(p_other.data().clone())
    , m_format(p_other.m_format)
    , m_horizontalHeaderLabels(p_other.m_horizontalHeaderLabels)
    , m_verticalHeaderLabels(p_other.m_verticalHeaderLabels)
{
}

CMatrixModel::CMatrixModel(const QString &p_filePath)
    : QAbstractTableModel()
    , m_filePath(p_filePath)
    , m_data()
    , m_format(CMatrixConverter::Format_Unknown)
    , m_horizontalHeaderLabels()
    , m_verticalHeaderLabels()
{
    CMatrixConverter converter(p_filePath);
    setData(converter.data());
    m_format = converter.format();
}

CMatrixModel::CMatrixModel(const int p_rows, const int p_cols, const int p_type, const double p_value1, const double p_value2, const double p_value3)
    : QAbstractTableModel()
    , m_filePath()
    , m_data()
    , m_format(CMatrixConverter::Format_Mfe)
    , m_horizontalHeaderLabels()
    , m_verticalHeaderLabels()
{
    try
    {
        const int nbChannels = p_type / 8 + 1;

        cv::Scalar value;
        switch (nbChannels)
        {
            case 1:
                value = cv::Scalar(p_value1);
                break;

            case 2:
                value = cv::Scalar(p_value1, p_value2);
                break;

            case 3:
                value = cv::Scalar(p_value1, p_value2, p_value3);
                break;

            default:
                qWarning() << tr("Invalid number of channels");
                break;
        }

        m_data = cv::Mat(p_rows, p_cols, p_type, value);
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }
}

CMatrixModel::~CMatrixModel() { }

const QString &CMatrixModel::filePath() const
{
    return m_filePath;
}

bool CMatrixModel::isFormatData() const
{
    return (m_format == CMatrixConverter::Format_Xml || m_format == CMatrixConverter::Format_Txt || m_format == CMatrixConverter::Format_Mfe);
}

bool CMatrixModel::isFormatImage() const
{
    return (m_format == CMatrixConverter::Format_Bmp || m_format == CMatrixConverter::Format_Jpg || m_format == CMatrixConverter::Format_Png ||
            m_format == CMatrixConverter::Format_Webp || m_format == CMatrixConverter::Format_Raw);
}

cv::Mat CMatrixModel::data() const
{
    return m_data;
}

void CMatrixModel::setData(const cv::Mat &p_matrix)
{
    m_data = p_matrix;
    emit(dataChanged(QModelIndex(), QModelIndex()));
}

int CMatrixModel::rowCount(const QModelIndex &p_parent) const
{
    Q_UNUSED(p_parent);
    return data().rows;
}

int CMatrixModel::columnCount(const QModelIndex &p_parent) const
{
    Q_UNUSED(p_parent);
    return data().cols;
}

QVariant CMatrixModel::data(const QModelIndex &p_index, int p_role) const
{
    switch (p_role)
    {
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;

        case Qt::EditRole:
        case Qt::DisplayRole:
        {
            const int r = p_index.row();
            const int c = p_index.column();

            cv::Vec2b point2b;
            cv::Vec3b point3b;
            cv::Vec2s point2s;
            cv::Vec3s point3s;
            cv::Vec2s point2i;
            cv::Vec3s point3i;
            cv::Vec2f point2f;
            cv::Vec3f point3f;
            cv::Vec2d point2d;
            cv::Vec3d point3d;

            switch (type())
            {
                case CV_8UC1:
                    return m_data.at<uchar>(r, c);

                case CV_8UC2:
                    point2b = m_data.at<cv::Vec2b>(r, c);
                    return QString("%1 | %2").arg(point2b[0]).arg(point2b[1]);

                case CV_8UC3:
                    point3b = m_data.at<cv::Vec3b>(r, c);
                    return QString("%1 | %2 | %3").arg(point3b[0]).arg(point3b[1]).arg(point3b[2]);

                case CV_16UC1:
                    return m_data.at<unsigned short>(r, c);

                case CV_16UC2:
                    point2s = m_data.at<cv::Vec2s>(r, c);
                    return QString("%1 | %2").arg(point2s[0]).arg(point2s[1]);

                case CV_16UC3:
                    point3s = m_data.at<cv::Vec3s>(r, c);
                    return QString("%1 | %2 | %3").arg(point3s[0]).arg(point3s[1]).arg(point3s[2]);

                case CV_32SC1:
                    return m_data.at<int>(r, c);

                case CV_32SC2:
                    point2i = m_data.at<cv::Vec2i>(r, c);
                    return QString("%1 | %2").arg(point2i[0]).arg(point2i[1]);

                case CV_32SC3:
                    point3i = m_data.at<cv::Vec3i>(r, c);
                    return QString("%1 | %2 | %3").arg(point3i[0]).arg(point3i[1]).arg(point3i[2]);

                case CV_32FC1:
                    return m_data.at<float>(r, c);

                case CV_32FC2:
                    point2f = m_data.at<cv::Vec2f>(r, c);
                    return QString("%1 | %2").arg(point2f[0]).arg(point2f[1]);

                case CV_32FC3:
                    point3f = m_data.at<cv::Vec3f>(r, c);
                    return QString("%1 | %2 | %3").arg(point3f[0]).arg(point3f[1]).arg(point3f[2]);

                case CV_64FC1:
                    return m_data.at<double>(r, c);

                case CV_64FC2:
                    point2d = m_data.at<cv::Vec2d>(r, c);
                    return QString("%1 | %2").arg(point2d[0]).arg(point2d[1]);

                case CV_64FC3:
                    point3d = m_data.at<cv::Vec3d>(r, c);
                    return QString("%1 | %2 | %3").arg(point3d[0]).arg(point3d[1]).arg(point3d[2]);

                default:
                    return QVariant();
            }
            break;
        }

        default:
            data(p_index, Qt::DisplayRole);
    }

    return QVariant();
}

bool CMatrixModel::setData(const QModelIndex &p_index, const QVariant &p_value, int p_role)
{
    if (p_role != Qt::EditRole)
    {
        qWarning() << tr("Unsupported role: ") << p_role;
        return false;
    }

    QStringList tokens;
    if (channels() > 1)
    {
        tokens = p_value.toString().split(" | ");
        if (tokens.count() != channels())
        {
            qWarning() << tr("Invalid value %1 for matrix of type %2").arg(p_value.toString()).arg(typeString(true));
            return false;
        }
    }

    const int r = p_index.row();
    const int c = p_index.column();

    switch (type())
    {
        case CV_8UC1:
            m_data.at<uchar>(r, c) = (uchar) p_value.toInt();
            break;

        case CV_8UC2:
            m_data.at<cv::Vec2b>(r, c) = cv::Vec2b((uchar) tokens[0].toInt(), (uchar) tokens[1].toInt());
            break;

        case CV_8UC3:
            m_data.at<cv::Vec3b>(r, c) = cv::Vec3b((uchar) tokens[0].toInt(), (uchar) tokens[1].toInt(), (uchar) tokens[2].toInt());
            break;

        case CV_16UC1:
            m_data.at<unsigned short>(r, c) = (unsigned short) p_value.toInt();
            break;

        case CV_16UC2:
            m_data.at<cv::Vec2s>(r, c) = cv::Vec2s((unsigned short) tokens[0].toInt(), (unsigned short) tokens[1].toInt());
            break;

        case CV_16UC3:
            m_data.at<cv::Vec3s>(r, c) =
                cv::Vec3s((unsigned short) tokens[0].toInt(), (unsigned short) tokens[1].toInt(), (unsigned short) tokens[2].toInt());
            break;

        case CV_32SC1:
            m_data.at<int>(r, c) = p_value.toInt();
            break;

        case CV_32SC2:
            m_data.at<cv::Vec2i>(r, c) = cv::Vec2i(tokens[0].toInt(), tokens[1].toInt());
            break;

        case CV_32SC3:
            m_data.at<cv::Vec3i>(r, c) = cv::Vec3i(tokens[0].toInt(), tokens[1].toInt(), tokens[2].toInt());
            break;

        case CV_32FC1:
            m_data.at<float>(r, c) = p_value.toFloat();
            break;

        case CV_32FC2:
            m_data.at<cv::Vec2f>(r, c) = cv::Vec2f(tokens[0].toFloat(), tokens[1].toFloat());
            break;

        case CV_32FC3:
            m_data.at<cv::Vec3f>(r, c) = cv::Vec3f(tokens[0].toFloat(), tokens[1].toFloat(), tokens[2].toFloat());
            break;

        case CV_64FC1:
            m_data.at<double>(r, c) = p_value.toDouble();
            break;

        case CV_64FC2:
            m_data.at<cv::Vec2d>(r, c) = cv::Vec2d(tokens[0].toDouble(), tokens[1].toDouble());
            break;

        case CV_64FC3:
            m_data.at<cv::Vec3d>(r, c) = cv::Vec3d(tokens[0].toDouble(), tokens[1].toDouble(), tokens[2].toDouble());
            break;

        default:
            qWarning() << tr("Unsupported edition of matrix of type ") << typeString(true);
            return false;
    }

    emit(dataChanged(QModelIndex(), QModelIndex()));
    return true;
}

QVariant CMatrixModel::headerData(int p_section, Qt::Orientation p_orientation, int p_role) const
{
    if (p_role == Qt::DisplayRole)
    {
        if (p_orientation == Qt::Horizontal)
        {
            return p_section < m_horizontalHeaderLabels.size() ? m_horizontalHeaderLabels.at(p_section) : QString::number(p_section);
        }

        if (p_orientation == Qt::Vertical)
        {
            return p_section < m_verticalHeaderLabels.size() ? m_verticalHeaderLabels.at(p_section) : QString::number(p_section);
        }
    }
    return QVariant();
}

Qt::ItemFlags CMatrixModel::flags(const QModelIndex &p_index) const
{
    Q_UNUSED(p_index);
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool CMatrixModel::removeRows(int p_row, int p_count, const QModelIndex &p_parent)
{
    QAbstractTableModel::beginRemoveRows(p_parent, p_row, p_row + p_count - 1);

    try
    {
        // allocate output
        cv::Mat dst(m_data.rows - p_count, m_data.cols, m_data.type());

        // copy top part of the matrix
        if (p_row > 0)
        {
            cv::Mat top = dst.rowRange(0, p_row);
            m_data.rowRange(0, p_row).copyTo(top);
        }

        // copy bottom part of the matrix
        if (p_row + p_count < m_data.rows)
        {
            cv::Mat bottom = dst.rowRange(p_row, dst.rows);
            m_data.rowRange(p_row + p_count, m_data.rows).copyTo(bottom);
        }

        m_data = dst;
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
        return false;
    }

    QAbstractTableModel::endRemoveRows();

    return true;
}

bool CMatrixModel::removeColumns(int p_column, int p_count, const QModelIndex &p_parent)
{
    QAbstractTableModel::beginRemoveColumns(p_parent, p_column, p_column + p_count - 1);

    try
    {
        // allocate output
        cv::Mat dst(m_data.rows, m_data.cols - p_count, m_data.type());

        // copy left part of the matrix
        if (p_column > 0)
        {
            cv::Mat left = dst.colRange(0, p_column);
            m_data.colRange(0, p_column).copyTo(left);
        }

        // copy right part of the matrix
        if (p_column + p_count < m_data.cols)
        {
            cv::Mat right = dst.colRange(p_column, dst.cols);
            m_data.colRange(p_column + p_count, m_data.cols).copyTo(right);
        }

        m_data = dst;
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
        return false;
    }

    QAbstractTableModel::endRemoveColumns();

    return true;
}

bool CMatrixModel::insertRows(int p_row, int p_count, const QModelIndex &p_parent)
{
    QAbstractTableModel::beginInsertRows(p_parent, p_row, p_row + p_count - 1);

    try
    {
        // allocate output
        cv::Mat dst = cv::Mat::zeros(m_data.rows + p_count, m_data.cols, m_data.type());

        // copy top part of the matrix
        if (p_row > 0)
        {
            cv::Mat top = dst.rowRange(0, p_row);
            m_data.rowRange(0, p_row).copyTo(top);
        }

        // copy bottom part of the matrix
        if (p_row < m_data.rows)
        {
            cv::Mat bottom = dst.rowRange(p_row + p_count, dst.rows);
            m_data.rowRange(p_row, m_data.rows).copyTo(bottom);
        }

        m_data = dst;
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
        return false;
    }

    QAbstractTableModel::endInsertRows();

    return true;
}

bool CMatrixModel::insertColumns(int p_column, int p_count, const QModelIndex &p_parent)
{
    QAbstractTableModel::beginInsertColumns(p_parent, p_column, p_column + p_count - 1);

    try
    {
        // allocate output
        cv::Mat dst = cv::Mat::zeros(m_data.rows, m_data.cols + p_count, m_data.type());

        // copy left part of the matrix
        if (p_column > 0)
        {
            cv::Mat left = dst.colRange(0, p_column);
            m_data.colRange(0, p_column).copyTo(left);
        }

        // copy right part of the matrix
        if (p_column < m_data.cols)
        {
            cv::Mat right = dst.colRange(p_column + p_count, dst.cols);
            m_data.colRange(p_column, m_data.cols).copyTo(right);
        }

        m_data = dst;
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
        return false;
    }

    QAbstractTableModel::endInsertColumns();

    return true;
}

void CMatrixModel::sort(int p_column, Qt::SortOrder p_order)
{
    if (channels() != 1)
    {
        qWarning() << tr("Sorting matrix with multiple channels is not implemented yet");
        return;
    }

    const int cvOrder = (p_order == Qt::AscendingOrder) ? cv::SORT_ASCENDING : cv::SORT_DESCENDING;

    cv::Mat sortedData(m_data.size(), m_data.type());
    cv::Mat sortedColumnIndex;
    cv::sortIdx(m_data.col(p_column), sortedColumnIndex, cv::SORT_EVERY_COLUMN | cvOrder);
    for (int i = 0; i < sortedColumnIndex.rows; ++i)
    {
        cv::Mat tmp = sortedData.row(i);
        m_data.row(sortedColumnIndex.at<int>(i, 0)).copyTo(tmp);
    }
    sortedData.assignTo(m_data);

    emit(dataChanged(QModelIndex(), QModelIndex()));
}

int CMatrixModel::channels() const
{
    return m_data.channels();
}

int CMatrixModel::type() const
{
    return m_data.type();
}

QString CMatrixModel::typeString(const bool p_full) const
{
    QString res;
    switch (type() % 8)
    {
        case 0:
            res = "8U";
            break;

        case 1:
            res = "8S";
            break;

        case 2:
            res = "16U";
            break;

        case 3:
            res = "16S";
            break;

        case 4:
            res = "32S";
            break;

        case 5:
            res = "32F";
            break;

        case 6:
            res = "64F";
            break;

        default:
            res = "??";
            break;
    }

    if (p_full)
    {
        res += "C" + QString::number(channels());
    }

    return res;
}

void CMatrixModel::setProfile(const QString &p_profile)
{
    if (p_profile.isEmpty())
    {
        return;
    }

    // Parse profile for rows/columns labels
    m_horizontalHeaderLabels.clear();
    m_verticalHeaderLabels.clear();

    QFile file(p_profile);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << tr("Can't open profile in read mode: %1").arg(p_profile);
        return;
    }

    QXmlStreamReader xml(&file);
    while (!xml.atEnd())
    {
        if (xml.readNext())
        {
            if (xml.name() == "column")
            {
                QString label = xml.attributes().value("label").toString().simplified();
                if (!label.isEmpty())
                {
                    m_horizontalHeaderLabels << label;
                }
            }
            else if (xml.name() == "row")
            {
                QString label = xml.attributes().value("label").toString().simplified();
                if (!label.isEmpty())
                {
                    m_verticalHeaderLabels << label;
                }
            }
        }
    }

    if (xml.hasError())
    {
        qWarning() << tr("Badly formed xml document: %1").arg(p_profile);
        qWarning() << tr("Error: %1").arg(xml.errorString());
    }

    file.close();

    emit(headerDataChanged(Qt::Horizontal, 0, m_horizontalHeaderLabels.size()));
    emit(headerDataChanged(Qt::Vertical, 0, m_verticalHeaderLabels.size()));
}

/*
Operations
*/

size_t CMatrixModel::total() const
{
    size_t res = 0;

    try
    {
        res = m_data.total();
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }

    return res;
}

int CMatrixModel::countNonZeros() const
{
    int res = 0;

    try
    {
        res = cv::countNonZero(m_data);
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }

    return res;
}

void CMatrixModel::minMaxLoc(double *p_minVal, double *p_maxVal, QPoint *p_minLoc, QPoint *p_maxLoc)
{
    try
    {
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(m_data, p_minVal, p_maxVal, &minLoc, &maxLoc);
        if (p_minLoc != nullptr)
        {
            p_minLoc->setX(minLoc.x);
            p_minLoc->setY(minLoc.y);
        }

        if (p_maxLoc != nullptr)
        {
            p_maxLoc->setX(maxLoc.x);
            p_maxLoc->setY(maxLoc.y);
        }
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }
}

void CMatrixModel::meanStdDev(double *p_mean, double *p_stddev)
{
    try
    {
        cv::Scalar mean, stddev;
        cv::meanStdDev(m_data, mean, stddev);
        *p_mean   = mean[0];
        *p_stddev = stddev[0];
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }
}

void CMatrixModel::convertTo(const int p_type, const double p_alpha, const double p_beta)
{
    if (p_type != type() || p_alpha != 1 || p_beta != 0)
    {
        try
        {
            m_data.convertTo(m_data, p_type, p_alpha, p_beta);
            emit(dataChanged(QModelIndex(), QModelIndex()));
        }
        catch (cv::Exception &e)
        {
            qWarning() << e;
        }
    }
}

void CMatrixModel::add(const double p_value)
{
    if (p_value != 0)
    {
        try
        {
            setData(m_data + p_value);
        }
        catch (cv::Exception &e)
        {
            qWarning() << e;
        }
    }
}

void CMatrixModel::multiply(const double p_value)
{
    if (p_value != 1)
    {
        try
        {
            setData(m_data * p_value);
        }
        catch (cv::Exception &e)
        {
            qWarning() << e;
        }
    }
}

void CMatrixModel::transpose()
{
    try
    {
        m_data = m_data.t();
        emit(dataChanged(QModelIndex(), QModelIndex()));
        emit(layoutChanged());
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }
}

void CMatrixModel::mulTranspose()
{
    try
    {
        cv::mulTransposed(m_data, m_data, false);
        emit(dataChanged(QModelIndex(), QModelIndex()));
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }
}

void CMatrixModel::verticalFlip()
{
    try
    {
        cv::flip(m_data, m_data, 0);
        emit(dataChanged(QModelIndex(), QModelIndex()));
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }
}

void CMatrixModel::horizontalFlip()
{
    try
    {
        cv::flip(m_data, m_data, 1);
        emit(dataChanged(QModelIndex(), QModelIndex()));
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }
}

void CMatrixModel::rotate(const QPointF &p_center, const double p_angle_dg, const double p_scaleFactor)
{
    try
    {
        const cv::Point2f rotationCenter(p_center.x(), p_center.y());
        cv::Mat rotation = getRotationMatrix2D(rotationCenter, p_angle_dg, p_scaleFactor);

        cv::Mat dst;
        cv::warpAffine(m_data, dst, rotation, m_data.size());
        m_data = dst;

        emit(dataChanged(QModelIndex(), QModelIndex()));
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }
}

void CMatrixModel::normalize(const double p_alpha, const double p_beta, const int p_norm)
{
    try
    {
        cv::normalize(m_data, m_data, p_alpha, p_beta, p_norm);
        emit(dataChanged(QModelIndex(), QModelIndex()));
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }
}

void CMatrixModel::absdiff(const cv::Mat &p_other)
{
    try
    {
        cv::absdiff(m_data, p_other, m_data);
        emit(dataChanged(QModelIndex(), QModelIndex()));
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }
}

void CMatrixModel::multiplyElements(const cv::Mat &p_other)
{
    try
    {
        m_data = m_data.mul(p_other);
        emit(dataChanged(QModelIndex(), QModelIndex()));
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }
}

void CMatrixModel::multiplyMatrix(const cv::Mat &p_other)
{
    try
    {
        m_data = m_data * p_other;
        emit(dataChanged(QModelIndex(), QModelIndex()));
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }
}

void CMatrixModel::applyColorMap(const int p_colorMap)
{
    try
    {
        cv::applyColorMap(m_data, m_data, p_colorMap);
        emit(dataChanged(QModelIndex(), QModelIndex()));
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }
}

void CMatrixModel::threshold(const double p_threshold, const double p_maxValue, const int p_type)
{
    try
    {
        cv::threshold(m_data, m_data, p_threshold, p_maxValue, p_type);
        emit(dataChanged(QModelIndex(), QModelIndex()));
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }
}

void CMatrixModel::merge(const QStringList &p_channels)
{
    std::vector<cv::Mat> layers;
    foreach (const QString &channel, p_channels)
    {
        layers.push_back(CMatrixConverter(channel).data());
    }

    try
    {
        cv::merge(layers, m_data);
        emit(dataChanged(QModelIndex(), QModelIndex()));
    }
    catch (cv::Exception &e)
    {
        qWarning() << e;
    }
}

QImage *CMatrixModel::toQImage() const
{
    QSettings settings;
    settings.beginGroup("image");
    bool stretch = settings.value("stretch-dynamic", true).toBool();
    settings.endGroup();

    cv::Mat imgData;
    if (stretch)
    {
        double min = 0, max = 0;
        cv::minMaxLoc(m_data, &min, &max);
        cv::Mat tmp = m_data - min;
        imgData     = tmp * 255 / (max - min);
    }
    else
    {
        imgData = m_data.clone();
    }

    // Convert matrix data to RGB
    try
    {
        imgData.convertTo(imgData, CV_8U);
        cv::cvtColor(imgData, imgData, channels() < 3 ? cv::COLOR_GRAY2RGB : cv::COLOR_BGR2RGB);
    }
    catch (cv::Exception &e)
    {
        qWarning() << tr("Can't convert color space");
        qWarning() << e;
        return new QImage;
    }

    QImage *image = new QImage(imgData.cols, imgData.rows, QImage::Format_RGB888);
    for (int i = 0; i < imgData.rows; ++i)
    {
        memcpy(image->scanLine(i), imgData.ptr(i), image->bytesPerLine());
    }

    return image;
}

QString CMatrixModel::valueDescription() const
{
    // update position widget in main window
    if (type() == CV_8UC3)
    {
        return tr("BGR");
    }

    return tr("Value");
}

CMatrixModel *CMatrixModel::clone() const
{
    return new CMatrixModel(*this);
}

bool CMatrixModel::compare(CMatrixModel *p_model, CMatrixModel *p_other)
{
    cv::Mat m1 = p_model->data();
    cv::Mat m2 = p_other->data();

    if (m1.empty() && m2.empty())
    {
        return true;
    }

    if (m1.channels() != m2.channels() || m1.type() != m2.type() || m1.rows != m2.rows || m1.cols != m2.cols)
    {
        return false;
    }

    m1.convertTo(m1, CV_64F);
    m2.convertTo(m2, CV_64F);

    std::vector<cv::Mat> channels1, channels2;
    cv::split(m1, channels1);
    cv::split(m2, channels2);

    for (int channel = 0; channel < m1.channels(); ++channel)
    {
        for (int r = 0; r < m1.rows; ++r)
        {
            for (int c = 0; c < m1.cols; ++c)
            {
                const double v1 = channels1[channel].at<double>(r, c);
                const double v2 = channels2[channel].at<double>(r, c);
                if (v1 != v2)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

QPointF CMatrixModel::center() const
{
    const float x = (float) (columnCount() / 2.0);
    const float y = (float) (rowCount() / 2.0);
    return QPointF(x, y);
}
