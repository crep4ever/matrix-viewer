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

#include "config.hh"

#include <QDialog>
#include <QPushButton>
#include <QScrollArea>
#include <QWidget>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;

class QLabel;
class QLineEdit;
class QCheckBox;
class QSpinBox;
class QComboBox;
class CFileChooser;
class CMainWindow;

class QtGroupBoxPropertyBrowser;

/** \brief ConfigDialog handles the display of the config pages
 */
class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    /// Constructor.
    ConfigDialog(QWidget *p_parent = nullptr);

    /*!
    Returns the parent widget (which is the main window of the application).
  */
    CMainWindow *parent() const;

public slots:
    /*!
    Changes the configuration page from \a previous to \a current.
  */
    void changePage(QListWidgetItem *p_current, QListWidgetItem *p_previous);

protected:
    /*!
    Saves all pages settings before closing.
  */
    void closeEvent(QCloseEvent *p_event) override;

private:
    void createIcons();

    QListWidget *m_contentsWidget;
    QStackedWidget *m_pagesWidget;
};

/** \brief Page is the base class for config page
 */
class Page : public QScrollArea
{
    Q_OBJECT
public:
    /// Constructor.
    Page(QWidget *p_parent = nullptr);

    /*!
    Returns the parent widget (which is the ConfigDialog object).
  */
    ConfigDialog *parent() const;

    /*!
    Applies the layout \a layout to the current page.
  */
    void setLayout(QLayout *p_layout);

protected:
    /*!
    Saves settings before closing the page.
  */
    void closeEvent(QCloseEvent *p_event) override;

private:
    virtual void readSettings();
    virtual void writeSettings();

    QWidget *m_content;
};

/**
 * \class DisplayPage
 * \brief Config page used to specify which components should be displayed/hidden
 */
class DisplayPage : public Page
{
    Q_OBJECT

public:
    /// Constructor.
    DisplayPage(QWidget *p_parent = nullptr);

private:
    void readSettings() override;
    void writeSettings() override;

    QCheckBox *m_statusBarCheckBox;
    QCheckBox *m_toolBarCheckBox;
};

/**
 * \class ImportImagePage
 * \brief ImportImagePage defines parameters to import image files
 */
class ImagePage : public Page
{
    Q_OBJECT

public:
    /// Constructor.
    ImagePage(QWidget *p_parent = nullptr);

private:
    void readSettings() override;
    void writeSettings() override;

    QCheckBox *m_stretchDynamic;

    QComboBox *m_rawType;
    QSpinBox *m_rawWidth;
    QSpinBox *m_rawHeight;
    QCheckBox *m_rawLittleEndianByteOrder;
};
