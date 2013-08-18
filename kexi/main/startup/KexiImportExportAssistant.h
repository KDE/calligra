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

#ifndef KEXIIMPORTEXPORTASSISTANT_H
#define KEXIIMPORTEXPORTASSISTANT_H

#include <kexiutils/KexiAssistantPage.h>
#include <kexiutils/KexiAssistantWidget.h>

#include "ui_KexiMainImportExportPage.h"

class QAction;
class KexiImportExportAssistant;

class KexiMainImportExportPage : public KexiAssistantPage,
                                 public Ui::KexiMainImportExportPage
{
    Q_OBJECT
public:
    KexiMainImportExportPage(KexiImportExportAssistant *assistant,
                             QWidget* parent = 0);
    ~KexiMainImportExportPage();
};

class KexiImportExportAssistant : public KexiAssistantWidget
{
    Q_OBJECT
public:
    KexiImportExportAssistant(
        const QAction *action_project_import_export_send_,
        const QAction *action_import_project_,
        QWidget* parent = 0);
    ~KexiImportExportAssistant();

    const QAction *action_project_import_export_send;
    const QAction *action_import_project;

public slots:
    virtual void nextPageRequested(KexiAssistantPage* page);
    virtual void cancelRequested(KexiAssistantPage* page);

signals:
    void importProject();

private:

    class Private;
    Private* const d;
};

#endif
