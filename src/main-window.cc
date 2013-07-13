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
#include <QXmlStreamReader>
#include <QDirIterator>

#include <opencv2/opencv.hpp>

#include "progress-bar.hh"
#include "preferences.hh"
#include "matrix-model.hh"
#include "matrix-view.hh"
#include "image-view.hh"
#include "matrix-converter.hh"
#include "compare-dialog.hh"
#include "properties-dialog.hh"
#include "tab-widget.hh"
#include "tab.hh"
#include "position.hh"

#include "config.hh"

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

  m_dataViewAct = new QAction(tr("&Data"), this);
  m_dataViewAct->setIcon(QIcon(":/icons/matrix-viewer/48x48/matrix-viewer.png"));
  m_dataViewAct->setStatusTip(tr("Display the matrix in a table layout"));
  m_dataViewAct->setCheckable(true);
  connect(m_dataViewAct, SIGNAL(toggled(bool)), SLOT(toggleDataView(bool)));

  m_imageViewAct = new QAction(tr("&Image"), this);
  m_imageViewAct->setIcon(QIcon::fromTheme("image-x-generic"));
  m_imageViewAct->setStatusTip(tr("Display the matrix as an image"));
  m_imageViewAct->setCheckable(true);
  connect(m_imageViewAct, SIGNAL(toggled(bool)), SLOT(toggleImageView(bool)));

  m_adjustColumnsAct = new QAction(tr("&Adjust columns"), this);
  m_adjustColumnsAct->setStatusTip(tr("Adjust columns of the table view to contents"));
  connect(m_adjustColumnsAct, SIGNAL(triggered()), SLOT(adjustColumnsToContents()));

  m_propertiesAct = new QAction(tr("&Properties"), this);
  m_propertiesAct->setIcon(QIcon::fromTheme("document-properties"));
  m_propertiesAct->setStatusTip(tr("Display properties of the matrix"));
  connect(m_propertiesAct, SIGNAL(triggered()), SLOT(properties()));

  m_loadProfileAct = new QAction(tr("&Load profile"), this);
  m_loadProfileAct->setIcon(QIcon::fromTheme("document-x-generic"));
  m_loadProfileAct->setStatusTip(tr("Load a specific profile for the matrix"));
  connect(m_loadProfileAct, SIGNAL(triggered()), SLOT(loadProfile()));
}

void CMainWindow::toggleDataView(bool value)
{
  if (!currentWidget())
    return;

  int nbChildren = currentWidget()->count();
  for (int i = 0; i < nbChildren; ++i)
    {
      CMatrixView *view = qobject_cast<CMatrixView*>(currentWidget()->widget(i));
      if (view)
	view->setVisible(value);
    }
}

void CMainWindow::toggleImageView(bool value)
{
  if (!currentWidget())
    return;

  int nbChildren = currentWidget()->count();
  for (int i = 0; i < nbChildren; ++i)
    {
      CImageView *view = qobject_cast<CImageView*>(currentWidget()->widget(i));
      if (view)
	view->setVisible(value);
    }
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
  fileMenu->addAction(m_loadProfileAct);
  fileMenu->addSeparator();
  fileMenu->addAction(m_preferencesAct);
  fileMenu->addSeparator();
  fileMenu->addAction(m_exitAct);

  QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(m_dataViewAct);
  viewMenu->addAction(m_imageViewAct);
  viewMenu->addAction(m_adjustColumnsAct);
  viewMenu->addAction(m_propertiesAct);

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
  m_mainToolBar->addSeparator();
  m_mainToolBar->addAction(m_dataViewAct);
  m_mainToolBar->addAction(m_imageViewAct);
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
  toggleDataView(m_dataViewAct->isChecked());
  toggleImageView(m_imageViewAct->isChecked());
}

void CMainWindow::properties()
{
  CPropertiesDialog dialog(this);
  dialog.exec();
}

void CMainWindow::adjustColumnsToContents()
{
  if (!currentWidget())
    return;

  int nbChildren = currentWidget()->count();
  for (int i = 0; i < nbChildren; ++i)
    {
      CMatrixView *view = qobject_cast<CMatrixView*>(currentWidget()->widget(i));
      if (view)
	view->resizeColumnsToContents();
    }
}

void CMainWindow::loadProfile()
{
  if (!currentModel())
    {
      statusBar()->showMessage(tr("Can't load profiles for empty matrix"));
      return;
    }

  QString filename = QFileDialog::getOpenFileName(this,
						  tr("Open profile file"),
						  QString(MATRIX_VIEWER_DATA_PATH) + "/profiles",
						  tr("Profile files (*.xml)"));
  if (!filename.isEmpty())
    {
      currentModel()->setProfile(filename);
    }
}

QString CMainWindow::findProfile(const QString & filename) const
{
  QFileInfo fi(filename);
  QString profile;
  bool foundProfile = false;
  QDirIterator it(QString(MATRIX_VIEWER_DATA_PATH) + "/profiles", QDir::Files, QDirIterator::NoIteratorFlags);
  while (it.hasNext() && !foundProfile)
    {
      profile = it.next();

      QFile file(profile);
      file.open(QIODevice::ReadOnly);
      QXmlStreamReader xml(&file);
      while (!xml.atEnd())
	{
	  xml.readNext();
	  if (xml.name() == "regexp")
	    {
	      QRegExp re(xml.readElementText().simplified());
	      re.setPatternSyntax(QRegExp::Wildcard);
	      if (re.exactMatch(fi.fileName()))
		{
		  foundProfile = true;
		  break;
		}
	    }
	}
      if (xml.hasError())
	{
	  statusBar()->showMessage(tr("Badly formed xml document: %1").arg(profile));
	}
      file.close();
    }
  return profile;
}

void CMainWindow::open(const QString & filename)
{
  QFileInfo fi(filename);
  m_openPath = fi.absolutePath();
  CMatrixConverter converter(filename);

  // Try to find a suitable profile for this file
  QString profile = findProfile(filename);

  // Build model from data file
  CMatrixModel *model = new CMatrixModel();
  model->setData(converter.data());
  model->setProfile(profile);

  // New tab
  CTab *tab = new CTab();

  // Set up the views
  CMatrixView *matrixView = new CMatrixView(this);
  matrixView->setModel(model);
  tab->addWidget(matrixView);

  CImageView *imgView = new CImageView(this);
  imgView->setModel(model);
  tab->addWidget(imgView);

  if (converter.isFormatData())
    {
      m_dataViewAct->setChecked(true);
      toggleDataView(m_dataViewAct->isChecked());
    }

  if (converter.isFormatImage())
    {
      m_imageViewAct->setChecked(true);
      toggleImageView(m_imageViewAct->isChecked());
    }

  m_mainWidget->addTab(tab, filename);
  m_mainWidget->setCurrentWidget(tab);

  connect(tab, SIGNAL(labelChanged(const QString&)),
	  m_mainWidget, SLOT(changeTabText(const QString&)));

  statusBar()->showMessage(filename);
}

void CMainWindow::open()
{
  QStringList filenames = QFileDialog::getOpenFileNames(this,
                                                        tr("Open data file"),
                                                        m_openPath,
                                                        tr("Data files (*.xml *.txt *.bmp *.png *.jpg)"));
  foreach (const QString & filename, filenames)
    if (!filename.isEmpty())
      open(filename);
}

void CMainWindow::save(const QString & filename)
{
  if (!currentModel() || filename.isEmpty())
    {
      statusBar()->showMessage(tr("Can't save empty matrix"));
      return;
    }

  if (!currentWidget() || !currentWidget()->isModified())
    return;

  currentWidget()->setModified(false);
  currentWidget()->setFilePath(filename);

  CMatrixConverter converter;
  converter.setData(currentModel()->data());
  converter.save(filename);

  statusBar()->showMessage(tr("Save: %1").arg(filename));
}

void CMainWindow::save()
{
  save(currentWidget()->filePath());
}


void CMainWindow::saveAs()
{
  if (!currentModel())
    {
      statusBar()->showMessage(tr("Can't save empty matrix"));
      return;
    }

  QString filename = QFileDialog::getSaveFileName(this,
                                                  tr("Save data file"),
                                                  m_savePath,
                                                  tr("Data files (*.xml *.txt)"));
  QFileInfo fi(filename);
  m_savePath = fi.absolutePath();
  save(filename);
}


CProgressBar* CMainWindow::progressBar() const
{
  return m_progressBar;
}

void CMainWindow::closeTab(int index)
{
  int nbChildren = currentWidget()->count();
  for (int i = 0; i < nbChildren; ++i)
    delete currentWidget()->widget(i);

  delete m_mainWidget->widget(index);
}

void CMainWindow::changeTab(int index)
{
  Q_UNUSED(index);
  toggleDataView(m_dataViewAct->isChecked());
  toggleImageView(m_imageViewAct->isChecked());
  if (currentWidget())
    {
      disconnect(positionWidget());
      connect(positionWidget(), SIGNAL(positionChanged(int, int)),
	      currentWidget(), SLOT(selectItem(int, int)));
    }
}

