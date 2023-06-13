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
#include "benchmark-result.hh"
#include "config.hh"
#include "main-window.hh"
#include "parser.hh"

#include <QApplication>
#include <QDate>
#include <QDebug>
#include <QDir>
#include <QLocale>
#include <QMetaType>
#include <QTextStream>
#include <QTranslator>

void printUsage()
{
    QTextStream out(stdout);
    out << Qt::endl;

    out << "********************************************************" << Qt::endl;
    out << "Description: Display and edit matrix data such as images" << Qt::endl;
    out << Qt::endl;

    out << "--------------------------------------------------------" << Qt::endl;
    out << "GUI mode" << Qt::endl;
    out << "Usage: " << QCoreApplication::applicationName() << " FILES " << Qt::endl;
    out << Qt::endl;

    out << "FILES: list of compatible matrix data files" << Qt::endl;
    out << Qt::endl;

    out << "--------------------------------------------------------" << Qt::endl;
    out << "CLI mode" << Qt::endl;
    out << "Usage: " << QCoreApplication::applicationName() << " --convert [OPTIONS] FILES FORMATS" << Qt::endl;
    out << Qt::endl;

    out << "FILES: list of 2D images" << Qt::endl;
    out << Qt::endl;

    out << "FORMATS: output formats from the following list:" << Qt::endl;
    out << "\t--txt \t"
        << "Text format: first row = [nbcolumns nbrows]; second row = [data]." << Qt::endl;
    out << "\t--xml \t"
        << "OpenCV serialization in xml format." << Qt::endl;
    out << "\t--jpg \t"
        << "JPEG format." << Qt::endl;
    out << "\t--bmp \t"
        << "BMP format." << Qt::endl;
    out << "\t--png \t"
        << "PNG format." << Qt::endl;
    out << "\t--webp\t"
        << "WEBP format (requires OpenCV 3.X)." << Qt::endl;
    out << "\t--raw \t"
        << "Raw images (16bits unsigned, little-endian byte order)." << Qt::endl;
    out << "\t--mfe \t"
        << "Matrix Format Exchange." << Qt::endl;
    out << "\t--edf \t"
        << "European Data Format." << Qt::endl;
    out << Qt::endl;

    out << "OPTIONS:" << Qt::endl;
    out << "\t-o, --output-directory <DIRECTORY>\t"
        << "Specify an existing DIRECTORY where converted images should be saved." << Qt::endl;
    out << "\t--raw-width  <VALUE>\t\t\t"
        << "Specify an integer VALUE for width of raw images (default is 2160)." << Qt::endl;
    out << "\t--raw-height <VALUE>\t\t\t"
        << "Specify an integer VALUE for height of raw images (default is 1944)." << Qt::endl;
    out << Qt::endl;
    out << "********************************************************" << Qt::endl;
    out << Qt::endl;
}

void printVersion()
{
    QTextStream out(stdout);
    out << QCoreApplication::applicationName() << " " << QCoreApplication::applicationVersion() << Qt::endl;
}

/// Main routine of the application
int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    QApplication::setOrganizationDomain("rgoffe.org");
    QApplication::setApplicationName(PROJECT_APPLICATION_NAME);
    QApplication::setApplicationVersion(PROJECT_VERSION);

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
    bool helpFlag         = false;
    ;
    bool versionFlag = false;
    bool cliMode     = false;

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
    QDir translationDirectory;
    QString translationFilename = QString("matrix-viewer_%1.qm").arg(QLocale::system().name().split('_').first());
    QString directory;

    translationDirectory = QDir(PROJECT_DATA_PATH);
    if (translationDirectory.exists())
    {
        directory = translationDirectory.absoluteFilePath("lang");
    }
    else
    {
        directory = QDir::current().absoluteFilePath("lang");
    }

    QTranslator translator;
    if (!translator.load(translationFilename, directory))
    {
        qDebug() << "Can't load translation" << translationFilename << "from directory" << directory;
    }
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

    foreach (const QString &arg, arguments)
    {
        if (QFile(arg).exists() && CMainWindow::isFilenameSupported(arg))
        {
            mainWindow.open(arg);
        }
    }

    return application.exec();
}
