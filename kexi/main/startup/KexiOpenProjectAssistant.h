/* This file is part of the KDE project
   Copyright (C) 2011-2013 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2012 Dimitrios T. Tanis <dimitrios.tanis@kdemail.net>

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

#include "KexiAssistantMessageHandler.h"
#include <core/kexidbconnectionset.h>
#include <kexiutils/KexiContextMessage.h>
#include <kexiutils/KexiAssistantPage.h>
#include <kexiutils/KexiAssistantWidget.h>
#include <kexiutils/utils.h>
#include <widget/KexiServerDriverNotFoundMessage.h>

#include <KDbConnectionData>

class QTabWidget;
class KexiProjectData;
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

    QTabWidget* tabWidget;
    KexiConnectionSelectorWidget* fileSelector;
    KexiConnectionSelectorWidget* connSelector;
private Q_SLOTS:
    void init();
    void tabChanged(int index);
private:
    QWidget* m_fileSelectorWidget;
    QWidget* m_connSelectorWidget;
    QPointer<KexiServerDriverNotFoundMessage> m_errorMessagePopup;
};

//! A page for selecting existing server database project
class KexiProjectDatabaseSelectionPage : public KexiAssistantPage
{
    Q_OBJECT
public:
    explicit KexiProjectDatabaseSelectionPage(KexiOpenProjectAssistant* parent);
    ~KexiProjectDatabaseSelectionPage();

    bool setConnection(KDbConnectionData* data);

    KexiProjectSelectorWidget* projectSelector;
    //! @todo KEXI3 use equivalent of QPointer<KDbConnectionData>
    KDbConnectionData* conndataToShow;

private:
    KexiOpenProjectAssistant* m_assistant;
};

class KexiOpenProjectAssistant : public KexiAssistantWidget,
                                 public KexiAssistantMessageHandler
{
    Q_OBJECT
public:
    explicit KexiOpenProjectAssistant(QWidget* parent = 0);
    ~KexiOpenProjectAssistant();

public Q_SLOTS:
    virtual void nextPageRequested(KexiAssistantPage* page);
    virtual void cancelRequested(KexiAssistantPage* page);
    void tryAgainActionTriggered();
    void cancelActionTriggered();

Q_SIGNALS:
    void openProject(const KexiProjectData& data);
    void openProject(const QString& fileName);

private Q_SLOTS:
    void slotOpenProject(KexiProjectData* data);

protected:
    virtual QWidget* calloutWidget() const;

private:
    class Private;
    Private* const d;
};

#endif
