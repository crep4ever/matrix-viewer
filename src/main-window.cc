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
#include "main-window.hh"

#include <QApplication>
#include <QAction>
#include <QBoxLayout>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QStatusBar>
#include <QToolBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QUrl>
#include <QDebug>
#include <QStringList>
#include <QTableView>
#include <QSplitter>
#include <QLabel>

#include <opencv2/opencv.hpp>

#include "progress-bar.hh"
#include "preferences.hh"
#include "matrix-model.hh"
#include "matrix-view.hh"
#include "image-view.hh"
#include "matrix-converter.hh"
#include "compare-dialog.hh"
#include "tab-widget.hh"
#include "tab.hh"
#include "position.hh"

CMainWindow::CMainWindow(QWidget *parent)
  : QMainWindow(parent)
  , m_mainWidget(new CTabWidget(this))
  , m_progressBar(new CProgressBar(this))
  , m_position(new CPosition(this))
  , m_isToolBarDisplayed(true)
  , m_isStatusBarDisplayed(true)
  , m_openPath(QDir::homePath())
  , m_savePath(QDir::homePath())
{
  setWindowTitle("Matrix viewer");
  setWindowIcon(QIcon(":/icons/matrix-viewer/256x256/matrix-viewer.png"));

  createActions();
  createMenus();
  createToolBar();

  // place elements into the main window
  m_mainWidget->setTabsClosable(true);
  m_mainWidget->setMovable(true);
  connect(m_mainWidget, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
  connect(m_mainWidget, SIGNAL(currentChanged(int)), SLOT(changeTab(int)));
  setCentralWidget(m_mainWidget);

  m_progressBar->setTextVisible(false);
  m_progressBar->setRange(0, 0);
  m_progressBar->hide();

  // status bar with an embedded label and progress bar
  statusBar()->addPermanentWidget(m_position);
  statusBar()->addPermanentWidget(m_progressBar);

  readSettings(true);
}

CMainWindow::~CMainWindow()
{
}

void CMainWindow::readSettings(bool firstLaunch)
{
  QSettings settings;
  settings.beginGroup("general");
  if (firstLaunch)
    {
      resize(settings.value("size", QSize(800,600)).toSize());
      move(settings.value("pos", QPoint(200, 200)).toPoint());
      if (settings.value("maximized", isMaximized()).toBool())
	showMaximized();
    }
  m_openPath = settings.value("openPath", QDir::homePath()).toString();
  m_savePath = settings.value("savePath", QDir::homePath()).toString();
  settings.endGroup();

  settings.beginGroup("display");
  setStatusBarDisplayed(settings.value("statusBar", true).toBool());
  setToolBarDisplayed(settings.value("toolBar", true).toBool());
  settings.endGroup();
}

void CMainWindow::writeSettings()
{
  QSettings settings;
  settings.beginGroup( "general" );
  settings.setValue( "maximized", isMaximized() );
  if (!isMaximized())
    {
      settings.setValue( "pos", pos() );
      settings.setValue( "size", size() );
    }
  settings.setValue( "openPath", m_openPath );
  settings.setValue( "savePath", m_savePath );
  settings.endGroup();
}

void CMainWindow::createActions()
{
  m_openAct = new QAction(tr("&Open..."), this);
  m_openAct->setIcon(QIcon::fromTheme("document-open", QIcon(":/icons/tango/32x32/actions/document-open.png")));
  m_openAct->setShortcut(QKeySequence::Open);
  m_openAct->setStatusTip(tr("Open a data file"));
  connect(m_openAct, SIGNAL(triggered()), this, SLOT(open()));

  m_saveAct = new QAction(tr("&Save"), this);
  m_saveAct->setShortcut(QKeySequence::Save);
  m_saveAct->setIcon(QIcon::fromTheme("document-save", QIcon(":/icons/tango/32x32/actions/document-save.png")));
  m_saveAct->setStatusTip(tr("Save the current data file"));
  connect(m_saveAct, SIGNAL(triggered()), this, SLOT(save()));

  m_saveAsAct = new QAction(tr("Save &As..."), this);
  m_saveAsAct->setShortcut(QKeySequence::SaveAs);
  m_saveAsAct->setIcon(QIcon::fromTheme("document-save-as", QIcon(":/icons/tango/32x32/actions/document-save-as.png")));
  m_saveAsAct->setStatusTip(tr("Save the current data file with a different name"));
  connect(m_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  m_compareAct = new QAction(tr("&Compare"), this);
  m_compareAct->setIcon(QIcon(":/icons/matrix-viewer/48x48/compare.png"));
  m_compareAct->setStatusTip(tr("Compare with another data file"));
  connect(m_compareAct, SIGNAL(triggered()), this, SLOT(compare()));

  m_documentationAct = new QAction(tr("Online &Documentation"), this);
  m_documentationAct->setShortcut(QKeySequence::HelpContents);
  m_documentationAct->setIcon(QIcon::fromTheme("help-contents", QIcon(":/icons/tango/32x32/actions/help-contents.png")));
  m_documentationAct->setStatusTip(tr("Download documentation pdf file "));
  connect(m_documentationAct, SIGNAL(triggered()), this, SLOT(documentation()));

  m_bugsAct = new QAction(tr("&Report a bug"), this);
  m_bugsAct->setStatusTip(tr("Report a bug about this application"));
  connect(m_bugsAct, SIGNAL(triggered()), this, SLOT(reportBug()));

  m_aboutAct = new QAction(tr("&About"), this);
  m_aboutAct->setIcon(QIcon::fromTheme("help-about", QIcon(":/icons/tango/32x32/actions/help-about.png")));
  m_aboutAct->setStatusTip(tr("About this application"));
  m_aboutAct->setMenuRole(QAction::AboutRole);
  connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  m_exitAct = new QAction(tr("&Quit"), this);
  m_exitAct->setIcon(QIcon::fromTheme("application-exit", QIcon(":/icons/tango/32x32/application-exit.png")));
  m_exitAct->setShortcut(QKeySequence::Quit);
  m_exitAct->setStatusTip(tr("Quit the program"));
  m_exitAct->setMenuRole(QAction::QuitRole);
  connect(m_exitAct, SIGNAL(triggered()), this, SLOT(close()));

  m_preferencesAct = new QAction(tr("&Preferences"), this);
  m_preferencesAct->setIcon(QIcon::fromTheme("document-properties", QIcon(":/icons/tango/32x32/document-properties.png")));
  m_preferencesAct->setStatusTip(tr("Configure the application"));
  m_preferencesAct->setMenuRole(QAction::PreferencesRole);
  connect(m_preferencesAct, SIGNAL(triggered()), SLOT(preferences()));
}

CTabWidget* CMainWindow::mainWidget() const
{
  return m_mainWidget;
}

CTab* CMainWindow::currentWidget() const
{
  if (!m_mainWidget)
    return 0;

  return qobject_cast<CTab*>(m_mainWidget->currentWidget());
}

CMatrixView* CMainWindow::currentView() const
{
  if (!currentWidget())
    return 0;

  return qobject_cast<CMatrixView*>(currentWidget()->widget(0));
}

CMatrixModel* CMainWindow::currentModel() const
{
  if (!currentView())
    return 0;

  return qobject_cast<CMatrixModel*>(currentView()->model());
}

cv::Mat CMainWindow::currentData() const
{
  if (!currentModel())
    return cv::Mat();

  return currentModel()->data();
}

CPosition* CMainWindow::positionWidget() const
{
  return m_position;
}

void CMainWindow::setToolBarDisplayed(bool value)
{
  m_mainToolBar->setVisible(value);
}

bool CMainWindow::isToolBarDisplayed()
{
  return m_isToolBarDisplayed;
}

void CMainWindow::setStatusBarDisplayed(bool value)
{
  m_isStatusBarDisplayed = value;
  statusBar()->setVisible(value);
}

bool CMainWindow::isStatusBarDisplayed()
{
  return m_isStatusBarDisplayed;
}

void CMainWindow::closeEvent(QCloseEvent *event)
{
  writeSettings();
  event->accept();
}

void CMainWindow::createMenus()
{
  menuBar()->setContextMenuPolicy(Qt::PreventContextMenu);

  QMenu *fileMenu = menuBar()->addMenu(tr("&Matrix"));
  fileMenu->addAction(m_openAct);
  fileMenu->addAction(m_saveAct);
  fileMenu->addAction(m_saveAsAct);
  fileMenu->addSeparator();
  fileMenu->addAction(m_compareAct);
  fileMenu->addSeparator();
  fileMenu->addAction(m_preferencesAct);
  fileMenu->addSeparator();
  fileMenu->addAction(m_exitAct);

  QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(m_documentationAct);
  helpMenu->addAction(m_bugsAct);
  helpMenu->addAction(m_aboutAct);
}

void CMainWindow::createToolBar()
{
  m_mainToolBar = new QToolBar(tr("Matrix"), this);
  m_mainToolBar->setMovable(false);
  m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  m_mainToolBar->addAction(m_openAct);
  m_mainToolBar->addAction(m_compareAct);
  addToolBar(m_mainToolBar);

  setUnifiedTitleAndToolBarOnMac(true);
}

void CMainWindow::preferences()
{
  ConfigDialog dialog(this);
  dialog.exec();
  readSettings();
}

void CMainWindow::documentation()
{
  QDesktopServices::openUrl(QUrl("http://www.github.com/crep4ever/matrix-viewer"));
}

void CMainWindow::reportBug()
{
  QDesktopServices::openUrl(QUrl("http://www.github.com/crep4ever/matrix-viewer/issues"));
}

void CMainWindow::about()
{
  QString title(tr("About Matrix Viewer"));
  QString version = QCoreApplication::applicationVersion();

  QString description(tr("This program allows to visualize and edit matrix data"));

  QStringList authorsList = QStringList() << "R. Goffe";
  QString authors = authorsList.join(", ");

  QMessageBox::about(this, title, tr("<p>%1</p>"
				     "<p><b>Version:</b> %2</p>"
				     "<p><b>Authors:</b> %3</p>")
		     .arg(description).arg(version).arg(authors));
}

void CMainWindow::compare()
{
  CCompareDialog dialog(this);
  dialog.exec();
}

void CMainWindow::open(const QString & filename)
{
  QFileInfo fi(filename);
  m_openPath = fi.absolutePath();
  CMatrixConverter converter(filename);

  // Build model from data file
  CMatrixModel *model = new CMatrixModel();
  model->setData(converter.data());

  // Set up the views
  CMatrixView *matrixView = new CMatrixView(this);
  matrixView->setModel(model);

  CImageView *imgView = new CImageView(this);
  imgView->setModel(model);

  // New tab
  CTab *tab = new CTab();
  tab->addWidget(matrixView);
  tab->addWidget(imgView);

  connect(tab, SIGNAL(labelChanged(const QString&)),
	  m_mainWidget, SLOT(changeTabText(const QString&)));

  m_mainWidget->addTab(tab, filename);
  m_mainWidget->setCurrentWidget(tab);
  statusBar()->showMessage(filename);
}

void CMainWindow::open()
{
  QStringList filenames = QFileDialog::getOpenFileNames(this,
                                                        tr("Open data file"),
                                                        m_openPath,
                                                        tr("Data files (*.xml *.txt);;Images (*.bmp)"));
  foreach (const QString & filename, filenames)
    if (!filename.isEmpty())
      open(filename);
}

void CMainWindow::save(const QString & filename)
{
  if (QTableView *view = qobject_cast< QTableView* >(currentWidget()->widget(0)))
    if (CMatrixModel* model = qobject_cast< CMatrixModel* >(view->model()))
      {
        CMatrixConverter converter;
        converter.setData(model->data());
        converter.save(filename);
      }
  statusBar()->showMessage(tr("Save: %1").arg(filename));
}

void CMainWindow::save()
{
  if (!currentWidget() || !currentWidget()->isModified())
    return;

  currentWidget()->setModified(false);
  save(currentWidget()->filePath());
}


void CMainWindow::saveAs()
{
  QString filename = QFileDialog::getSaveFileName(this,
                                                  tr("Save data file"),
                                                  m_savePath,
                                                  tr("Data files (*.xml *.txt)"));
  QFileInfo fi(filename);
  m_savePath = fi.absolutePath();

  if (QTableView *view = qobject_cast< QTableView* >(currentWidget()->widget(0)))
    if (CMatrixModel* model = qobject_cast< CMatrixModel* >(view->model()))
      {
        CMatrixConverter converter;
        converter.setData(model->data());
        converter.save(filename);
      }
}


CProgressBar* CMainWindow::progressBar() const
{
  return m_progressBar;
}


void CMainWindow::closeTab(int index)
{
  if (CMatrixView *view = qobject_cast< CMatrixView* >(m_mainWidget->widget(index)))
    {
      delete view->model();
      delete view;
    }

  if (QSplitter *comparisonView = qobject_cast< QSplitter* >(m_mainWidget->widget(index)))
    {
      for (int i = 0; i < comparisonView->count(); ++i)
      	if (CMatrixView *view = qobject_cast< CMatrixView* >(comparisonView->widget(i)))
          delete view->model();

      delete comparisonView;
    }
}

void CMainWindow::changeTab(int index)
{
  Q_UNUSED(index);
  if (currentWidget())
    {
      disconnect(positionWidget());
      connect(positionWidget(), SIGNAL(positionChanged(int, int)),
	      currentView(), SLOT(selectItem(int, int)));
    }
}

