/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KexiNewProjectWizard_H
#define KexiNewProjectWizard_H

#include "kexidbconnectionset.h"
#include "ui_KexiNewPrjTypeSelector.h"
#include "ui_KexiOpenExistingFile.h"
#include "ui_KexiServerDBNamePage.h"
#include <kexidb/connectiondata.h>

#include <k3wizard.h>

class Q3ListViewItem;

class KexiNewPrjTypeSelector : public QWidget, public Ui::KexiNewPrjTypeSelector
{
  public:
    KexiNewPrjTypeSelector(QWidget* parent);
};

class KexiOpenExistingFile : public QWidget, public Ui::KexiOpenExistingFile
{
  public:
    KexiOpenExistingFile(QWidget* parent);
};

class KexiServerDBNamePage : public QWidget, public Ui::KexiServerDBNamePage
{
  public:
    KexiServerDBNamePage(QWidget* parent);
};

class KexiConnSelectorWidget;
class KexiDBTitlePage;
class KexiProjectSelectorWidget;

class KEXIMAIN_EXPORT KexiNewProjectWizard : public K3Wizard
{
  Q_OBJECT
  public:
    KexiNewProjectWizard(KexiDBConnectionSet& conn_set, QWidget *parent=0);
    virtual ~KexiNewProjectWizard();
  
    /*! \return name for a new project's database if server-based project 
     type was selected. Returns file name if file-based project was selected. */
    QString projectDBName() const;

    /*! \return name for a new project. Used for both file- and serever- based projects. */
    QString projectCaption() const;

    /*! \return data of selected connection for new project, 
     if server-based project type was selected.
     Returns NULL if no selection has been made or file-based project
     has been selected. */
    KexiDB::ConnectionData* projectConnectionData() const;
    
    /*! Reimplemented for internal reasons */	
    virtual void show();

    /*! If true, user will be asked to accept overwriting existing project. 
     This is true by default. */
    void setConfirmOverwrites(bool set);

  protected slots:
    void slotLvTypesSelected(Q3ListViewItem *);
    void slotLvTypesExecuted(Q3ListViewItem *);
    void slotServerDBCaptionTxtChanged(const QString &capt);
    void slotServerDBNameTxtChanged(const QString &n);
    
    virtual void done(int r);
    virtual void next();
    virtual void accept();
    
  protected:
    virtual void showPage(QWidget *page);
    
    KexiNewPrjTypeSelector *m_prjtype_sel;
    KexiDBTitlePage *m_db_title;
    KexiServerDBNamePage *m_server_db_name;
    KexiProjectSelectorWidget* m_project_selector;
    
    KexiConnSelectorWidget *m_conn_sel;
    QWidget *m_conn_sel_widget;
    
    class Private;
    Private * const d;
};

#endif

