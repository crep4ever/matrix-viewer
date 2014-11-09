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

#ifndef __MAIN_WINDOW_HH__
#define __MAIN_WINDOW_HH__

#include <QMainWindow>
#include <QDir>

class QTabWidget;
class QToolBar;
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

  Class for the main window of the application.
*/
class CMainWindow : public QMainWindow
{
  Q_OBJECT

public slots:
  void open(const QString &filename);

public:
  /// Constructor.
  CMainWindow(QWidget *parent=0);

  /// Destructor.
  ~CMainWindow();

  /*!
    Returns the progress bar that is embedded in the status bar.
  */
  CProgressBar * progressBar() const;

  CTabWidget* mainWidget() const;
  CTab* currentWidget() const;
  CMatrixView* currentView() const;
  CMatrixModel* currentModel() const;

  CPosition* positionWidget() const;

protected:
  /*!
    Saves settings before closing the application.
  */
  void closeEvent(QCloseEvent *event);

private slots:
  void open();
  void save();
  void saveAs();
  void closeTab(int index);
  void changeTab(int index);
  void operations();

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

  QAction *m_openAct;
  QAction *m_saveAct;
  QAction *m_saveAsAct;
  QAction *m_operationsAct;

  QAction *m_dataViewAct;
  QAction *m_imageViewAct;

  // Settings
  QString m_openPath;
  QString m_savePath;

public:
  static const QStringList _filters;
};

#endif  // __MAIN_WINDOW_HH__
