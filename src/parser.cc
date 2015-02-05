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

#include "parser.hh"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QDebug>
#include <QString>

#include "matrix-converter.hh"

CParser::CParser() :
m_command()
{
}

CParser::CParser(const QStringList & p_cliArguments) :
m_command(p_cliArguments)
{
}

CParser::~CParser()
{
}

const QStringList & CParser::command() const
{
    return m_command;
}

void CParser::setCommand(const QStringList & p_command)
{
    m_command = p_command;
}


int CParser::execute()
{

  QStringList extensions;
  QString outputPath;
  bool outputDir = false;


  if (m_command.contains("--txt"))
    {
      extensions << ".txt";
    }

  if (m_command.contains("--xml"))
    {
      extensions << ".xml";
    }

  if (m_command.contains("--jpg"))
    {
      extensions << ".jpg";
    }

  if (m_command.contains("--bmp"))
    {
      extensions << ".bmp";
    }

  if (m_command.contains("--png"))
    {
      extensions << ".png";
    }

  if (m_command.contains("--bin"))
    {
      extensions << ".bin";
    }

  if (m_command.contains("--raw"))
    {
      extensions << ".raw";
    }

  if (m_command.contains("--mfe"))
    {
      extensions << ".mfe";
    }

  CMatrixConverter converter;

  for (int i = 1; i < m_command.size(); ++i)  // skip first command line argument (executable name)
    {
      QString arg(m_command[i]);

      if (arg == "--output-directory" || arg == "-o")
        {
          arg = QString(m_command[++i]); //option value

          if (QDir(arg).exists())
            {
              outputPath = arg;
              outputDir = true;
            }
          else
            {
              qWarning() << QObject::tr("Invalid output directory [%1]. Run [%2 -h] for usage information.")
                .arg(outputPath)
                .arg(QCoreApplication::applicationName());
            }
        }
      else if (arg == "--raw-width")
        {
          converter.setRawWidth(QString(m_command[++i]).toInt()); //option value
        }
      else if (arg == "--raw-height")
        {
          converter.setRawHeight(QString(m_command[++i]).toInt()); //option value
        }
      else if (QFile(arg).exists())
        {
          CMatrixConverter converter;

          // -------------------------------------------
          // Load input file
          // -------------------------------------------

          QString input = arg;
          if (!converter.load(input))
            {
              qWarning() << QObject::tr("Fail to load input file [%1]").arg(input);
              return -1;
            }

          // -------------------------------------------
          // Save output file in all specified formats
          // -------------------------------------------

          QFileInfo fi(input);
          foreach (QString extension, extensions)
            {
              QString path = outputDir ? outputPath : fi.absolutePath();
              QString output = path + QDir::separator() + fi.baseName() + extension;

              if (!converter.save(output))
                {
                  qWarning() << QObject::tr("Fail to save output file [%1]").arg(output);
                  return -1;
                }

              qDebug() << QObject::tr("Successful conversion from [%1] to [%2]")
                .arg(input)
                .arg(output);
            }
        }
      else if (!arg.endsWith(QCoreApplication::applicationName()) && !arg.startsWith("--"))
        {
          qWarning() << QObject::tr("Ignoring [%1]. Run [%2 -h] for usage information.")
            .arg(arg)
            .arg(QCoreApplication::applicationName());
        }
    }

  return 0;
}


