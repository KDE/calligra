/* This file is part of the KDE project
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexitextmsghandler.h"

#include "kexi.h"
#include <db/utils.h>
#include <kexiutils/utils.h>

class KexiTextMessageHandler::Private
{
public:
    Private(QString* msgTarget, QString* dTarget);
    ~Private();

    QString *messageTarget, *detailsTarget;
};

KexiTextMessageHandler::Private::Private(QString* msgTarget, QString* dTarget)
    :messageTarget(msgTarget), detailsTarget(dTarget)
{
    messageTarget->clear();
    detailsTarget->clear();
}

KexiTextMessageHandler::Private::~Private()
{

}

KexiTextMessageHandler::KexiTextMessageHandler(QString &messageTarget, QString &detailsTarget)
        : KexiGUIMessageHandler(0)
        ,d(new Private(&messageTarget, &detailsTarget))
{

}

KexiTextMessageHandler::~KexiTextMessageHandler()
{
    delete d;
}

void KexiTextMessageHandler::showMessage(MessageType type,
                                         const QString &title, const QString &details,
                                         const QString& dontShowAgainName)
{
    Q_UNUSED(type);
    Q_UNUSED(dontShowAgainName);
    if (!m_enableMessages)
        return;

    //'wait' cursor is a nonsense now
    KexiUtils::removeWaitCursor();

    QString msg(title);
    if (title.isEmpty())
        msg = i18n("Unknown error");
    msg = "<qt><p>" + msg + "</p>";
    *d->messageTarget = msg;
    *d->detailsTarget = details;
}

