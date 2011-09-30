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

#include "KexiImportExportAssistant.h"

#include <KAction>
#include <QPointer>

static QString stripText(const QString &text)
{
    return QString(text).replace('&', "").replace("...", "");
}

KexiMainImportExportPage::KexiMainImportExportPage(
    KexiImportExportAssistant *assistant,
    QWidget* parent)
 : KexiAssistantPage(stripText(assistant->action_project_import_export_send->text()),
                     QString(),
                     parent)
{
    setNextButtonVisible(false);

    QWidget* contents = new QWidget;
    setupUi(contents);
    btn_import->setText(stripText(assistant->action_import_project->text()));
    btn_import->setDescription(assistant->action_import_project->toolTip());
    connect(btn_import, SIGNAL(clicked()), assistant, SIGNAL(importProject()));

    setFocusWidget(contents);
    setContents(contents);
}

KexiMainImportExportPage::~KexiMainImportExportPage()
{
}

// ----

class KexiImportExportAssistant::Private
{
public:
    Private(KexiImportExportAssistant *qq)
     : q(qq)
    {
    }
    
    ~Private()
    {
    }
    
    KexiMainImportExportPage* mainPage() {
        return page<KexiMainImportExportPage>(&m_mainImportExportPage, q);
    }
    
    template <class C>
    C* page(QPointer<C>* p, KexiImportExportAssistant *parent = 0) {
        if (p->isNull()) {
            *p = new C(parent);
            q->addPage(*p);
        }
        return *p;
    }

    QPointer<KexiMainImportExportPage> m_mainImportExportPage;
    
    KexiImportExportAssistant *q;
};

// ----

KexiImportExportAssistant::KexiImportExportAssistant(
    const KAction *action_project_import_export_send_,
    const KAction *action_import_project_,
    QWidget* parent)
 : KexiAssistantWidget(parent)
 , action_project_import_export_send(action_project_import_export_send_)
 , action_import_project(action_import_project_)
 , d(new Private(this))
{
    setCurrentPage(d->mainPage());
    setFocusProxy(d->mainPage());
}

KexiImportExportAssistant::~KexiImportExportAssistant()
{
    delete d;
}
       
void KexiImportExportAssistant::previousPageRequested(KexiAssistantPage* page)
{
    Q_UNUSED(page);
}

void KexiImportExportAssistant::nextPageRequested(KexiAssistantPage* page)
{
    Q_UNUSED(page);
}

void KexiImportExportAssistant::cancelRequested(KexiAssistantPage* page)
{
    Q_UNUSED(page);
    //TODO?
}
