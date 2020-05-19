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

#include <QStringList>

/*!
  \file parser.hh
  \class CParser
  \brief CParser analyze command-line options when running the application from a terminal
*/
class CParser
{
public:
    CParser();
    CParser(QStringList  p_cliArguments);

    virtual ~CParser();

    const QStringList & command() const;
    void setCommand(const QStringList & p_command);

    /**
     * @brief Execute command line
     * @return Error code, 0 if success, -1 otherwise
     */
    int execute();

private:

    QStringList m_command;
};
