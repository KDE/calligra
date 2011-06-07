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

#ifndef KEXIRECENTPROJECTSASSISTANT_H
#define KEXIRECENTPROJECTSASSISTANT_H

#include "kexidbconnectionset.h"
#include "ui_KexiProjectStorageTypeSelectionPage.h"
#include <kexidb/connectiondata.h>
#include <kexidb/msghandler.h>
#include <kexiutils/KexiContextMessage.h>
#include <kexiutils/KexiAssistantPage.h>
#include <kexiutils/KexiAssistantWidget.h>
#include <kexiutils/KexiCategorizedView.h>

#include <QPointer>

class KexiConnSelectorWidget;
class KexiProjectSelectorWidget;
class KCategorizedView;
class KexiRecentProjectsAssistant;

class KexiMainRecentProjectsPage : public KexiAssistantPage
{
    Q_OBJECT
public:
    explicit KexiMainRecentProjectsPage(KexiRecentProjectsAssistant* assistant,
                                        QWidget* parent = 0);

    QString selectedTemplate;
    QString selectedCategory;
    
protected slots:
    void slotItemClicked(const QModelIndex& index);
private:
    KexiCategorizedView* m_recentProjects;
    KexiRecentProjectsAssistant* m_assistant;
};

class KexiProjectData;
class KexiProjectSet;

class KexiRecentProjectsAssistant : public KexiAssistantWidget,
                                    public KexiDB::MessageHandler
{
    Q_OBJECT
public:
    explicit KexiRecentProjectsAssistant(KexiProjectSet* projects, QWidget* parent = 0);
    ~KexiRecentProjectsAssistant();

    //! Implementation for KexiDB::MessageHandler.
    virtual void showErrorMessage(const QString &title,
                                  const QString &details = QString());

    //! Implementation for KexiDB::MessageHandler.
    virtual void showErrorMessage(KexiDB::Object *obj, const QString& msg = QString());

    KexiProjectSet* projects();
    
public slots:
    virtual void previousPageRequested(KexiAssistantPage* page);
    virtual void nextPageRequested(KexiAssistantPage* page);
    virtual void cancelRequested(KexiAssistantPage* page);
    void tryAgainActionTriggered();
signals:
    void openProject(KexiProjectData* data);
    
private:
    void createProject(
        const KexiDB::ConnectionData& cdata, const QString& databaseName,
        const QString& caption);

    class Private;
    Private* const d;
};

#endif
