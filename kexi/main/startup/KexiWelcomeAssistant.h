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

#ifndef KEXIWELCOMEASSISTANT_H
#define KEXIWELCOMEASSISTANT_H

#include <kexidb/connectiondata.h>
#include <kexidb/msghandler.h>
#include <kexiutils/KexiContextMessage.h>
#include <kexiutils/KexiAssistantPage.h>
#include <kexiutils/KexiAssistantWidget.h>
#include <kexiutils/KexiCategorizedView.h>

#include <QPointer>

class KexiProjectData;
class KexiProjectSelectorWidget;
class KCategorizedView;
class KexiWelcomeAssistant;
class KexiRecentProjectsProxyModel;
class KexiWelcomeStatusBar;

class KexiMainWelcomePage : public KexiAssistantPage
{
    Q_OBJECT
public:
    explicit KexiMainWelcomePage(KexiWelcomeAssistant* assistant,
                                        QWidget* parent = 0);

    QString selectedTemplate;
    QString selectedCategory;
    
signals:
    void openProject(const KexiProjectData& data, const QString& shortcutPath, bool *opened);

private slots:
    void slotItemClicked(const QModelIndex& index);
    void loadProjects();
private:
    KexiCategorizedView* m_recentProjects;
    KexiRecentProjectsProxyModel* m_recentProjectsProxyModel;
    KexiWelcomeAssistant* m_assistant;
    KexiWelcomeStatusBar* m_statusBar;
};

class KexiProjectData;
class KexiRecentProjects;

class KexiWelcomeAssistant : public KexiAssistantWidget,
                             public KexiDB::MessageHandler
{
    Q_OBJECT
public:
    explicit KexiWelcomeAssistant(KexiRecentProjects* projects, QWidget* parent = 0);
    ~KexiWelcomeAssistant();

    //! Implementation for KexiDB::MessageHandler.
    virtual void showErrorMessage(const QString &title,
                                  const QString &details = QString());

    //! Implementation for KexiDB::MessageHandler.
    virtual void showErrorMessage(KexiDB::Object *obj, const QString& msg = QString());

    KexiRecentProjects* projects();
    
public slots:
    virtual void previousPageRequested(KexiAssistantPage* page);
    virtual void nextPageRequested(KexiAssistantPage* page);
    virtual void cancelRequested(KexiAssistantPage* page);
    void tryAgainActionTriggered();

signals:
    /*! Emitted if project @a data was selected to open.
     @a shortcutPath can be non empty to indicate .kexis filename useful for opening new
     instance of Kexi. Receiver should set value pointed by @a opened to true if the
     database has been opened successfully. */
    void openProject(const KexiProjectData& data, const QString& shortcutPath, bool *opened);

private:
    void createProject(
        const KexiDB::ConnectionData& cdata, const QString& databaseName,
        const QString& caption);

    class Private;
    Private* const d;
};

#endif
