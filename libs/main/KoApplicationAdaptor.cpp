/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2006 Fredrik Edemar <f_edemar@linux.se>

   SPDX-License-Identifier: LGPL-2.0-only
*/

#include "KoApplicationAdaptor.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <MainDebug.h>

#include "KoApplication.h"
#include "KoDocument.h"
#include "KoDocumentEntry.h"
#include "KoMainWindow.h"
#include "KoPart.h"
#include "KoView.h"

KoApplicationAdaptor::KoApplicationAdaptor(KoApplication *parent)
    : QDBusAbstractAdaptor(parent)
    , m_application(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

KoApplicationAdaptor::~KoApplicationAdaptor()
{
    // destructor
}

// QString KoApplicationAdaptor::createDocument(const QString &nativeFormat)
//{
//     KoDocumentEntry entry = KoDocumentEntry::queryByMimeType(nativeFormat);
//     if (entry.isEmpty()) {
//         KMessageBox::questionTwoActions(0, i18n("Unknown Calligra MimeType %1. Check your installation.", nativeFormat));
//         return QString();
//     }
//     KoPart *part = entry.createKoPart(0);
//     if (part) {
//         m_application->addPart(part);
//         return '/' + part->document()->objectName();
//     }
//     else {
//         return QString();
//     }
// }

QStringList KoApplicationAdaptor::getDocuments()
{
    QStringList lst;
    QList<KoPart *> parts = m_application->partList();
    foreach (KoPart *part, parts) {
        lst.append('/' + part->document()->objectName());
    }
    return lst;
}

QStringList KoApplicationAdaptor::getViews()
{
    QStringList lst;
    QList<KoPart *> parts = m_application->partList();
    foreach (KoPart *part, parts) {
        foreach (KoView *view, part->views()) {
            lst.append('/' + view->objectName());
        }
    }

    return lst;
}

QStringList KoApplicationAdaptor::getWindows()
{
    QStringList lst;
    QList<KMainWindow *> mainWindows = KMainWindow::memberList();
    if (!mainWindows.isEmpty()) {
        foreach (KMainWindow *mainWindow, mainWindows) {
            lst.append(static_cast<KoMainWindow *>(mainWindow)->objectName());
        }
    }
    return lst;
}
