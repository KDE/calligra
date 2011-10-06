/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIOPENPROJECTASSISTANT_H
#define KEXIOPENPROJECTASSISTANT_H

#include "kexidbconnectionset.h"
#include "ui_KexiProjectStorageTypeSelectionPage.h"
#include <kexidb/connectiondata.h>
#include <kexidb/msghandler.h>
#include <kexiutils/KexiContextMessage.h>
#include <kexiutils/KexiAssistantPage.h>
#include <kexiutils/KexiAssistantWidget.h>

class KTabWidget;
class KexiProjectData;
class KexiProjectSet;
class KexiConnectionSelectorWidget;
class KexiProjectSelectorWidget;
class KexiOpenProjectAssistant;

//! A page displaying file projects and connections for server projects
class KexiMainOpenProjectPage : public KexiAssistantPage
{
    Q_OBJECT
public:
    explicit KexiMainOpenProjectPage(QWidget* parent = 0);
    ~KexiMainOpenProjectPage();

    KTabWidget* tabWidget;
    KexiConnectionSelectorWidget* fileSelector;
    KexiConnectionSelectorWidget* connSelector;
private slots:
    void init();
private:
    QWidget* m_fileSelectorWidget;
    QWidget* m_connSelectorWidget;
};

//! A page for selecting existing server database project
class KexiProjectDatabaseSelectionPage : public KexiAssistantPage
{
    Q_OBJECT
public:
    explicit KexiProjectDatabaseSelectionPage(KexiOpenProjectAssistant* parent);
    ~KexiProjectDatabaseSelectionPage();
    
    bool setConnection(KexiDB::ConnectionData* data);

    KexiProjectSelectorWidget* projectSelector;
    QPointer<KexiDB::ConnectionData> conndataToShow;

private:
    KexiProjectSet *m_projectSetToShow;
    KexiOpenProjectAssistant* m_assistant;
};

class KexiOpenProjectAssistant : public KexiAssistantWidget,
                                 public KexiDB::MessageHandler
{
    Q_OBJECT
public:
    explicit KexiOpenProjectAssistant(QWidget* parent = 0);
    ~KexiOpenProjectAssistant();

    //! Implementation for KexiDB::MessageHandler.
     virtual void showErrorMessage(const QString &title,
                                   const QString &details = QString());

    //! Implementation for KexiDB::MessageHandler.
    virtual void showErrorMessage(KexiDB::Object *obj, const QString& msg = QString());

public slots:
    virtual void previousPageRequested(KexiAssistantPage* page);
    virtual void nextPageRequested(KexiAssistantPage* page);
    virtual void cancelRequested(KexiAssistantPage* page);
    void slotOpenProject(KexiProjectData* data);

signals:
    void openProject(const KexiProjectData& data);
    void openProject(const QString& fileName);

private:
    class Private;
    Private* const d;
};

#endif
