// Copyright (C) 2022, Romain Goffe <romain.goffe@gmail.com>
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

#include <QString>
#include <QVector>
#include <QFileInfo>

class CProperty
{
public:

    CProperty(const QString & p_key = QString(),
              const QString & p_value = QString())
        : m_key(p_key), m_value(p_value)
    {}

    const QString& key() const
    {
        return m_key;
    }

    void setKey(const QString& p_key)
    {
        m_key = p_key;
    }

    const QString& value() const
    {
        return m_value;
    }

    void setValue(const QString& p_value)
    {
        m_value = p_value;
    }

private:
    QString m_key;
    QString m_value;
};

class CMetadata : public QFileInfo
{
public:
    const QVector<CProperty>& properties() const
    {
        return m_properties;
    }

    void addProperty(const CProperty& p_property)
    {
        m_properties << p_property;
    }

    void updateProperty(const QString& p_key, const QString& p_value)
    {
        for (CProperty& property : m_properties)
        {
            if (p_key == property.key())
            {
                property.setValue(p_value);
                break;
            }
        }
    }

    QString value(const QString& p_key) const
    {
        for (const CProperty& property : properties())
        {
            if (p_key == property.key())
            {
                return property.value();
            }
        }
        return QString();
    }

    void clear()
    {
        m_properties.clear();
    }

private:
    QVector<CProperty> m_properties;
};
