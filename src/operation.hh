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

#include <QList>
#include <QString>
#include <QUrl>

class Operation
{
public:
    Operation();
    Operation(const Operation& p_other);
    ~Operation();

    const QString& name() const;
    void setName(const QString& p_name);

    const QString& description() const;
    void setDescription(const QString& p_description);

    const QUrl& url() const;
    void setUrl(const QUrl& p_url);
    void setUrl(const QString& p_url);

    static QList<Operation> list_benchmark();

private:
    QString m_name;
    QString m_description;
    QUrl m_url;
};
