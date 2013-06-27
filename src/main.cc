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

/*!
 * \file main.cc
 * \mainpage Matrix-Viewer Documentation
 *
 */

#include <QApplication>

#include <QTranslator>
#include <QTextCodec>
#include <QDate>
#include <QLocale>
#include <QDir>
#include <QTextStream>
#include <QDebug>

#include "config.hh"
#include "main-window.hh"

/// Main routine of the application
int main(int argc, char *argv[])
{
  QApplication application(argc, argv);

  QApplication::setOrganizationName("ViTechnology");
  QApplication::setOrganizationDomain("vitechnology.com");
  QApplication::setApplicationName(MATRIX_VIEWER_APPLICATION_NAME);
  QApplication::setApplicationVersion(MATRIX_VIEWER_VERSION);

  // Load the application ressources (icons, ...)
  Q_INIT_RESOURCE(matrix);

  // Check for a standard theme icon. If it does not exist, for
  // instance on MacOSX or Windows, fallback to one of the theme
  // provided in the ressource file.
  if (!QIcon::hasThemeIcon("document-open"))
    {
      QIcon::setThemeName("tango");
    }

  // Parse command line arguments
  QStringList arguments = QApplication::arguments();
  bool helpFlag = false;;
  bool versionFlag = false;
  if (arguments.contains("-h") || arguments.contains("--help"))
    helpFlag = true;
  else if (arguments.contains("--version"))
    versionFlag = true;

  // Localization
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8")) ;
  QDir translationDirectory;
  QString translationFilename = QString("matrix-viewer_%1.qm").arg(QLocale::system().name().split('_').first());
  QString directory;

  translationDirectory = QDir(MATRIX_VIEWER_DATA_PATH);
  if (translationDirectory.exists())
    directory = translationDirectory.absoluteFilePath("lang");
  else
    directory = QDir::current().absoluteFilePath("lang");

  QTranslator translator;
  translator.load(translationFilename, directory);
  application.installTranslator(&translator);

  if (helpFlag)
    {
      QTextStream out(stdout);
      out << "Usage: " << QApplication::applicationName() << "[OPTION]" << endl
	  << "Options:" << endl
	  << "    " << "-h, --help"
	  << "    " << "--version"
	  << " " << QApplication::applicationVersion()
	  << endl;
      return 0;
    }
  else if (versionFlag)
    {
      QTextStream out(stdout);
      out << QApplication::applicationName()
	  << " " << QApplication::applicationVersion()
	  << endl;
      return 0;
    }

  CMainWindow mainWindow;
  mainWindow.show();

  foreach (const QString & arg, arguments)
    {
      if ( QFile(arg).exists() &&
           (arg.endsWith(".txt") || arg.endsWith(".xml")) )
        {
          mainWindow.open(arg);
        }
    }

  return application.exec();
}
