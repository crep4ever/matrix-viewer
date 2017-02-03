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

#pragma once

#include <QMainWindow>
#include <QDir>

class QTabWidget;
class QToolBar;
class QDropEvent;
class CProgressBar;
class CPosition;
class CTabWidget;
class CTab;
class CMatrixModel;
class CMatrixView;

/*!
\file main-window.hh
\class CMainWindow
\brief CMainWindow is the base class of the application.

The main window of the application features
a tabbed area where each tab corresponds to an opened file.

A matrix can be viewed as data and / or as an image
by enabling the corresponding actions in the View menu
or from the shortcuts in the main tool bar.

\image html main-window.png

The CMainWindow handles files as illustrated below :

\image html process.png

\li The class CMatrixConverter handles IO operations and
converts a file into OpenCV cv::Mat object

\li The class CMatrixModel is built on the cv::Mat data

\li CMatrixView and CImageView offer visualization of the CMatrixModel
*/
class CMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  /*!
  Return \a true if \a p_filename ends with a supported extensions,
  \a false otherwise.
  */
  static bool isFilenameSupported(const QString & p_filename);

public slots:
  /*!
  Open file from \a p_filePath
  */
  void open(const QString & p_filePath);

public:
  /// Constructor
  CMainWindow(QWidget *p_parent = 0);

  /// Destructor
  ~CMainWindow();

  /*!
  Returns the progress bar that is embedded in the status bar
  */
  CProgressBar * progressBar() const;

  /*!
  Getter on the main widget (multi-tabs container)
  */
  CTabWidget* mainWidget() const;

  /*!
  Getter on the current widget (single tab)
  */
  CTab* currentWidget() const;

  /*!
  Getter on the model of the current tab
  */
  CMatrixView* currentView() const;

  /*!
  Getter on the model of the current tab
  */
  CMatrixModel* currentModel() const;

  /*!
  Getter on the position widget widget (shared between all tabs)
  */
  CPosition* positionWidget() const;

  /*!
  Display \a p_message in the status bar
  */
  void showMessage(const QString & p_message) const;

protected:
  /*!
  Saves settings before closing the application
  */
  void closeEvent(QCloseEvent *p_event);

  void dropEvent(QDropEvent *p_event);

  void dragEnterEvent(QDragEnterEvent *p_event);

private slots:
  void newMatrix();
  void open();
  void save();
  void saveAs();
  void closeTab(int index);
  void changeTab(int index);
  void operations();
  void benchmark();

  //application
  void preferences();
  void setToolBarDisplayed(bool);
  void setStatusBarDisplayed(bool);
  void documentation();
  void reportBug();
  void about();

  void nextFile();
  void previousFile();


  // views
  void toggleDataView(bool);
  void toggleImageView(bool);

  void loadProfile();

private:
  void readSettings(bool firstLaunch = false);
  void writeSettings();

  void createActions();
  void createMenus();
  void createToolBar();

  bool isToolBarDisplayed();
  bool isStatusBarDisplayed();

  void save(const QString & filename);
  QString findProfile(const QString & filename) const;

  // Widgets
  CTabWidget *m_mainWidget;
  QToolBar *m_mainToolBar;
  CProgressBar *m_progressBar;
  CPosition *m_position;

  // Settings
  bool m_isToolBarDisplayed;
  bool m_isStatusBarDisplayed;

  // Application actions
  QAction *m_loadProfileAct;
  QAction *m_preferencesAct;
  QAction *m_documentationAct;
  QAction *m_bugsAct;
  QAction *m_aboutAct;
  QAction *m_exitAct;
  QAction *m_nextFileAct;
  QAction *m_previousFileAct;

  QAction *m_newAct;
  QAction *m_openAct;
  QAction *m_saveAct;
  QAction *m_saveAsAct;
  QAction *m_operationsAct;
  QAction *m_benchmarkAct;

  QAction *m_dataViewAct;
  QAction *m_imageViewAct;

  // Settings
  QString m_openPath;
  QString m_savePath;

public:
  static const QStringList _fileExtensions;
  static const QStringList _fileTypeFilters;
};
