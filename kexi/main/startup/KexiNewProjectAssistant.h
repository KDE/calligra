/* This file is part of the KDE project
   Copyright (C) 2003-2011 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXINEWPROJECTASSISTANT_H
#define KEXINEWPROJECTASSISTANT_H

#include "kexidbconnectionset.h"
#include "ui_KexiProjectStorageTypeSelectionPage.h"
#include <kexidb/connectiondata.h>
#include <kexiutils/KexiContextMessage.h>
#include <kexiutils/KexiAssistantPage.h>
#include <kexiutils/KexiAssistantWidget.h>

#include <QPointer>

class KexiConnSelectorWidget;
class KexiProjectSelectorWidget;
class KCategorizedView;

class KexiTemplateSelectionPage : public KexiAssistantPage
{
    Q_OBJECT
public:
    explicit KexiTemplateSelectionPage(QWidget* parent = 0);
    
protected slots:
    void slotItemClicked(const QModelIndex& index);
private:
    KCategorizedView* m_templatesList;
public:
    QString selectedTemplate;
    QString selectedCategory;
};

class KexiProjectStorageTypeSelectionPage : public KexiAssistantPage, public Ui::KexiProjectStorageTypeSelectionPage
{
    Q_OBJECT
public:
    explicit KexiProjectStorageTypeSelectionPage(QWidget* parent = 0);
    virtual ~KexiProjectStorageTypeSelectionPage();
    
    bool fileTypeSelected() const { return m_fileTypeSelected; }
private slots:
    void buttonClicked();

private:
    bool m_fileTypeSelected;
};

class KexiDBTitlePage;
class KexiStartupFileHandler;

class KexiProjectTitleSelectionPage : public KexiAssistantPage
{
    Q_OBJECT
public:
    explicit KexiProjectTitleSelectionPage(QWidget* parent = 0);
    virtual ~KexiProjectTitleSelectionPage();

    bool isAcceptable();

    KexiDBTitlePage* contents;
    KexiStartupFileHandler *fileHandler;
    QPointer<KexiContextMessageWidget> messageWidget;
private slots:    
    void titleTextChanged(const QString & text);
    void askForOverwriting(const KexiContextMessage& message);
private:
    void updateUrl();
};

class QProgressBar;

class KexiProjectCreationPage : public KexiAssistantPage
{
    Q_OBJECT
public:
    explicit KexiProjectCreationPage(QWidget* parent = 0);
    virtual ~KexiProjectCreationPage();
    
    QProgressBar* m_progressBar;
};

class KexiProjectConnectionSelectionPage : public KexiAssistantPage
{
    Q_OBJECT
public:
    explicit KexiProjectConnectionSelectionPage(QWidget* parent = 0);
    virtual ~KexiProjectConnectionSelectionPage();

    KexiConnSelectorWidget* connSelector;
};

class KexiServerDBNamePage;
class KexiGUIMessageHandler;
class KexiProjectData;
class KexiProjectSet;
class KexiProjectSelectorWidget;
class KexiProjectDatabaseNameSelectionPage : public KexiAssistantPage
{
    Q_OBJECT
public:
    explicit KexiProjectDatabaseNameSelectionPage(QWidget* parent = 0);
    virtual ~KexiProjectDatabaseNameSelectionPage();

    bool setConnection(KexiDB::ConnectionData* data);

    KexiServerDBNamePage* contents;
    QPointer<KexiDB::ConnectionData> conndataToShow;
    QPointer<KexiContextMessageWidget> messageWidget;
    bool isAcceptable();
private slots:
    void slotTitleChanged(const QString &capt);
    void slotNameChanged(const QString &);
    void overwriteActionTriggered();

private:
    QString enteredDbName() const;
    KexiGUIMessageHandler* m_msgHandler;
    KexiProjectSet *m_projectSetToShow;
    KexiProjectSelectorWidget* m_projectSelector;

    bool m_dbNameAutofill;
    bool m_le_dbname_txtchanged_enabled;
    KexiProjectData* m_projectDataToOverwrite;
    QAction* m_messageWidgetActionYes;
    QAction* m_messageWidgetActionNo;
};

class KexiProjectData;

class KexiNewProjectAssistant : public KexiAssistantWidget
{
    Q_OBJECT
public:
    explicit KexiNewProjectAssistant(QWidget* parent = 0);
    ~KexiNewProjectAssistant();
public slots:
    virtual void previousPageRequested(KexiAssistantPage* page);
    virtual void nextPageRequested(KexiAssistantPage* page);
    virtual void cancelRequested(KexiAssistantPage* page);
signals:
    void createProject(KexiProjectData* data);
private:
    void createProject(
        const KexiDB::ConnectionData& cdata, const QString& databaseName,
        const QString& caption);

    class Private;
    Private* const d;
};

#endif
