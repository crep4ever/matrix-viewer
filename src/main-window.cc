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
#include "operations-dialog.hh"
#include "benchmark-dialog.hh"
#include "tab-widget.hh"
#include "tab.hh"
#include "position.hh"

#include "config.hh"

const QStringList CMainWindow::_fileExtensions = QStringList()
<< "*.mfe" << "*.raw"
<< "*.png" << "*.PNG"
<< "*.jpg" << "*.jpeg" << "*.JPG"
<< "*.bmp" << "*.BMP"
<< "*.tif" << "*.tiff" << "*.TIFF"
<< "*.txt" << "*.xml";

const QStringList CMainWindow::_fileTypeFilters = QStringList()
<< "All files (*.*);;"
<< "JPEG (*.jpg *.jpeg *.JPG);;"
<< "PNG (*.png *.PNG);;"
<< "BMP (*.bmp *.BMP);;"
<< "TIFF (*.tif *.tiff *.TIFF);;"
<< "RAW (*.raw *.RAW);;"
<< "MFE (*.mfe *.MFE);;"
<< "TXT (*.txt *.TXT);;"
<< "XML (*.xml *.XML);;";


CMainWindow::CMainWindow(QWidget *p_parent) :
QMainWindow(p_parent)
, m_mainWidget(new CTabWidget(this))
, m_mainToolBar(0)
, m_progressBar(new CProgressBar(this))
, m_position(new CPosition(this))
, m_isToolBarDisplayed(true)
, m_isStatusBarDisplayed(true)
, m_loadProfileAct(0)
, m_preferencesAct(0)
, m_documentationAct(0)
, m_bugsAct(0)
, m_aboutAct(0)
, m_exitAct(0)
, m_nextFileAct(0)
, m_previousFileAct(0)
, m_openAct(0)
, m_saveAct(0)
, m_saveAsAct(0)
, m_operationsAct(0)
, m_benchmarkAct(0)
, m_dataViewAct(0)
, m_imageViewAct(0)
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

  m_operationsAct = new QAction(tr("&Operations"), this);
  m_operationsAct->setIcon(QIcon(":/icons/matrix-viewer/48x48/operations.png"));
  m_operationsAct->setStatusTip(tr("Apply common operations"));
  connect(m_operationsAct, SIGNAL(triggered()), this, SLOT(operations()));

  m_benchmarkAct = new QAction(tr("&Benchmark"), this);
  m_benchmarkAct->setIcon(QIcon(":/icons/matrix-viewer/48x48/benchmarks.png"));
  m_benchmarkAct->setStatusTip(tr("Run OpenCV benchmark on current matrix"));
  connect(m_benchmarkAct, SIGNAL(triggered()), this, SLOT(benchmark()));

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
  m_imageViewAct->setIcon(QIcon::fromTheme("image-x-generic", QIcon(":/icons/tango/48x48/image-x-generic.png")));
  m_imageViewAct->setStatusTip(tr("Display the matrix as an image"));
  m_imageViewAct->setCheckable(true);
  connect(m_imageViewAct, SIGNAL(toggled(bool)), SLOT(toggleImageView(bool)));

  m_loadProfileAct = new QAction(tr("&Load profile"), this);
  m_loadProfileAct->setIcon(QIcon::fromTheme("document-import", QIcon(":/icons/tango/48x48/document-import.png")));
  m_loadProfileAct->setStatusTip(tr("Load a specific profile for the matrix"));
  connect(m_loadProfileAct, SIGNAL(triggered()), SLOT(loadProfile()));

  m_previousFileAct = new QAction(tr("&Previous"), this);
  m_previousFileAct->setShortcut(Qt::CTRL + QKeySequence::MoveToPreviousChar);
  m_previousFileAct->setIcon(QIcon::fromTheme("go-previous", QIcon(":/icons/tango/48x48/go-previous.png")));
  m_previousFileAct->setStatusTip(tr("Load previous data file in current folder"));
  connect(m_previousFileAct, SIGNAL(triggered()), SLOT(previousFile()));

  m_nextFileAct = new QAction(tr("&Next"), this);
  m_nextFileAct->setShortcut(Qt::CTRL + QKeySequence::MoveToNextChar);
  m_nextFileAct->setIcon(QIcon::fromTheme("go-next", QIcon(":/icons/tango/48x48/go-next.png")));
  m_nextFileAct->setStatusTip(tr("Load next data file in current folder"));
  connect(m_nextFileAct, SIGNAL(triggered()), SLOT(nextFile()));
}

void CMainWindow::nextFile()
{
  if (!currentWidget())
  return;

  QFileInfo current(currentWidget()->filePath());

  QDir dir(m_openPath);
  dir.setNameFilters(_fileExtensions);

  QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::NoSymLinks, QDir::Name);
  for (int i = 0; i < list.size(); ++i)
  {
    if (list.at(i).fileName() == current.fileName())
    {
      int idx = (i + 1 == list.size()) ? 0 : i + 1;
      closeTab(m_mainWidget->currentIndex());
      open(list.at(idx).absoluteFilePath());
      return;
    }
  }

  showMessage(tr("No next file found"));
}

void CMainWindow::previousFile()
{
  if (!currentWidget())
  return;

  QFileInfo current(currentWidget()->filePath());

  QDir dir(m_openPath);
  dir.setNameFilters(_fileExtensions);

  QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::NoSymLinks, QDir::Name);
  for (int i = 0; i < list.size(); ++i)
  {
    if (list.at(i).fileName() == current.fileName())
    {
      int idx = (i == 0) ? list.size() - 1 : i - 1;
      closeTab(m_mainWidget->currentIndex());
      open(list.at(idx).absoluteFilePath());
      return;
    }
  }

  showMessage(tr("No previous file found"));
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

void CMainWindow::closeEvent(QCloseEvent *p_event)
{
  writeSettings();
  p_event->accept();
}

void CMainWindow::createMenus()
{
  menuBar()->setContextMenuPolicy(Qt::PreventContextMenu);

  QMenu *fileMenu = menuBar()->addMenu(tr("&Matrix"));
  fileMenu->addAction(m_openAct);
  fileMenu->addAction(m_saveAct);
  fileMenu->addAction(m_saveAsAct);
  fileMenu->addSeparator();
  fileMenu->addAction(m_operationsAct);
  fileMenu->addAction(m_benchmarkAct);
  fileMenu->addAction(m_loadProfileAct);
  fileMenu->addSeparator();
  fileMenu->addAction(m_preferencesAct);
  fileMenu->addSeparator();
  fileMenu->addAction(m_exitAct);

  QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(m_dataViewAct);
  viewMenu->addAction(m_imageViewAct);

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
  m_mainToolBar->addAction(m_saveAct);
  m_mainToolBar->addAction(m_saveAsAct);
  m_mainToolBar->addSeparator();

  m_mainToolBar->addAction(m_previousFileAct);
  m_mainToolBar->addAction(m_nextFileAct);
  m_mainToolBar->addSeparator();

  m_mainToolBar->addAction(m_operationsAct);
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
  "<p><b>Authors:</b> %3</p>"
  "<p><b>Qt:</b> %4</p>")
  .arg(description)
  .arg(version)
  .arg(authors)
  .arg(QT_VERSION_STR));
}

void CMainWindow::operations()
{
  if (!currentModel())
  {
    showMessage(tr("Can't apply operations without matrix"));
    return;
  }

  COperationsDialog dialog(this);
  dialog.exec();
}

void CMainWindow::benchmark()
{
  if (!currentModel())
  {
    showMessage(tr("Can't apply benchmark without matrix"));
    return;
  }

  CBenchmarkDialog dialog(this);
  dialog.exec();
}

void CMainWindow::loadProfile()
{
  if (!currentModel())
  {
    showMessage(tr("Can't load profiles for empty matrix"));
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
    if (!file.open(QIODevice::ReadOnly))
    {
      qWarning() << "CMainWindow::loadProfile can't open file in read mode";
      return QString();  
    }

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
      showMessage(tr("Badly formed xml document: %1").arg(profile));
    }

    file.close();
  }

  return foundProfile ? profile : QString();
}

void CMainWindow::open(const QString & filename)
{
  QFileInfo fi(filename);
  m_openPath = fi.absolutePath();

  // Try to find a suitable profile for this file
  QString profile = findProfile(filename);

  // Build model from data file
  CMatrixModel *model = new CMatrixModel(filename);
  model->setProfile(profile);
  positionWidget()->setValueDescription(model->valueDescription());

  // New tab
  CTab *tab = new CTab();

  // Set up the views
  CMatrixView *matrixView = new CMatrixView(this);
  matrixView->setModel(model);
  tab->addWidget(matrixView);

  CImageView *imgView = new CImageView(this);
  imgView->setModel(model);
  tab->addWidget(imgView);

  m_mainWidget->addTab(tab, filename);
  m_mainWidget->setCurrentWidget(tab);

  imgView->bestSize();

  if (model->isFormatData())
  {
    m_dataViewAct->setChecked(true);
    toggleDataView(m_dataViewAct->isChecked());
  }

  if (model->isFormatImage())
  {
    m_imageViewAct->setChecked(true);
    toggleImageView(m_imageViewAct->isChecked());
  }

  connect(tab, SIGNAL(labelChanged(const QString&)),
  m_mainWidget, SLOT(changeTabText(const QString&)));

  showMessage(filename);
  writeSettings(); // updates openPath
}

void CMainWindow::open()
{
  QStringList filenames = QFileDialog::getOpenFileNames(this,
    tr("Open data file"),
    m_openPath,
    tr("%1").arg(_fileTypeFilters.join(" ")));

  foreach (const QString & filename, filenames)
  {
    if (!filename.isEmpty())
    {
      open(filename);
    }
  }
}

void CMainWindow::save(const QString & filename)
{
  if (!currentWidget() || filename.isEmpty())
  {
    showMessage(tr("Can't save empty matrix"));
    return;
  }

  currentWidget()->setModified(false);
  currentWidget()->setFilePath(filename);

  CMatrixConverter converter;
  converter.setData(currentModel()->data());
  converter.save(filename);

  showMessage(tr("Save: %1").arg(filename));
}

void CMainWindow::save()
{
  save(currentWidget()->filePath());
}


void CMainWindow::saveAs()
{
  if (!currentModel())
  {
    showMessage(tr("Can't save empty matrix"));
    return;
  }

  QString filename = QFileDialog::getSaveFileName(this,
    tr("Save data file"),
    m_savePath,
    tr("Matrices (%1)").arg(_fileTypeFilters.join(" ")));

  if (!filename.isEmpty())
  {
    QFileInfo fi(filename);
    m_savePath = fi.absolutePath();
    save(filename);
    writeSettings(); //update savePath
  }
}

CProgressBar* CMainWindow::progressBar() const
{
  return m_progressBar;
}

void CMainWindow::closeTab(int index)
{
  CTab *tab = qobject_cast<CTab*>(m_mainWidget->widget(index));
  if (tab)
  {
    const int nbChildren = tab->count();
    for (int i = 0; i < nbChildren; ++i)
    {
      delete currentWidget()->widget(i);
    }
  }

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

    if (positionWidget() && currentModel())
    {
      positionWidget()->setValueDescription(currentModel()->valueDescription());
    }
  }
}

void CMainWindow::showMessage(const QString & p_message) const
{
  statusBar()->showMessage(p_message);
}
