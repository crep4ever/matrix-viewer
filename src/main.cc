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
 \file main.cc
 \mainpage Matrix-Viewer Documentation

 This application is a graphical frontend to OpenCV matrices
 that allows to visualize and edit them as tabular or image data.

 \image html main.png
*/
#include <QApplication>

#include <QTranslator>
#include <QTextCodec>
#include <QDate>
#include <QLocale>
#include <QDir>
#include <QTextStream>
#include <QMetaType>
#include <QDebug>

#include "config.hh"
#include "main-window.hh"
#include "parser.hh"
#include "benchmark-result.hh"

void printUsage()
{
  QTextStream out(stdout);
  out << endl;

  out << "********************************************************" << endl;
  out << "Description: Display and edit matrix data such as images" << endl;
  out << endl;

  out << "--------------------------------------------------------" << endl;
  out << "GUI mode" << endl;
  out << "Usage: " << QCoreApplication::applicationName() << " FILES " << endl;
  out << endl;

  out << "FILES: list of compatible matrix data files" << endl;
  out << endl;

  out << "--------------------------------------------------------" << endl;
  out << "CLI mode" << endl;
  out << "Usage: " << QCoreApplication::applicationName() << " --convert [OPTIONS] FILES FORMATS" << endl;
  out << endl;

  out << "FILES: list of 2D images" << endl;
  out << endl;

  out << "FORMATS: output formats from the following list:" << endl;
  out << "\t--txt \t" << "Text format: first row = [nbcolumns nbrows]; second row = [data]." << endl;
  out << "\t--xml \t" << "OpenCV serialization in xml format." << endl;
  out << "\t--jpg \t" << "JPEG format." << endl;
  out << "\t--bmp \t" << "BMP format." << endl;
  out << "\t--png \t" << "PNG format." << endl;
  out << "\t--webp\t" << "WEBP format." << endl;
  out << "\t--raw \t" << "Raw images (16bits unsigned, little-endian byte order)." << endl;
  out << "\t--mfe \t" << "Matrix Format Exchange." << endl;
  out << endl;

  out << "OPTIONS:" << endl;
  out << "\t-o, --output-directory <DIRECTORY>\t" << "Specify an existing DIRECTORY where converted images should be saved." << endl;
  out << "\t--raw-width  <VALUE>\t\t\t" << "Specify an integer VALUE for width of raw images (default is 2160)." << endl;
  out << "\t--raw-height <VALUE>\t\t\t" << "Specify an integer VALUE for height of raw images (default is 1944)." << endl;
  out << endl;
  out << "********************************************************" << endl;
  out << endl;
}

void printVersion()
{
  QTextStream out(stdout);
  out << QCoreApplication::applicationName()
  << " " << QCoreApplication::applicationVersion()
  << endl;
}


/// Main routine of the application
int main(int argc, char *argv[])
{
  QApplication application(argc, argv);

  QApplication::setOrganizationName("ViTechnology");
  QApplication::setOrganizationDomain("vitechnology.com");
  QApplication::setApplicationName(MATRIX_VIEWER_APPLICATION_NAME);
  QApplication::setApplicationVersion(MATRIX_VIEWER_VERSION);
  QApplication::setGraphicsSystem("raster");

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
  bool cliMode = false;

  if (arguments.contains("-h") || arguments.contains("--help"))
  {
    helpFlag = true;
  }
  else if (arguments.contains("--version") || arguments.contains("-v"))
  {
    versionFlag = true;
  }
  else if (arguments.contains("--convert")) // CLI option
  {
    cliMode = true;
  }

  // Localization
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8")) ;
  QDir translationDirectory;
  QString translationFilename = QString("matrix-viewer_%1.qm").arg(QLocale::system().name().split('_').first());
  QString directory;

  translationDirectory = QDir(MATRIX_VIEWER_DATA_PATH);
  if (translationDirectory.exists())
  {
    directory = translationDirectory.absoluteFilePath("lang");
  }
  else
  {
    directory = QDir::current().absoluteFilePath("lang");
  }

  QTranslator translator;
  translator.load(translationFilename, directory);
  application.installTranslator(&translator);

  if (helpFlag)
  {
    printUsage();
    return 0;
  }

  if (versionFlag)
  {
    printVersion();
    return 0;
  }

  if (cliMode)
  {
    CParser parser(QCoreApplication::arguments());
    return parser.execute();
  }

  qRegisterMetaType<BenchmarkResult>("BenchmarkResult");

  CMainWindow mainWindow;
  mainWindow.show();

  foreach (const QString & arg, arguments)
  {
    if (QFile(arg).exists() && CMainWindow::isFilenameSupported(arg))
    {
      mainWindow.open(arg);
    }
  }

  return application.exec();
}
