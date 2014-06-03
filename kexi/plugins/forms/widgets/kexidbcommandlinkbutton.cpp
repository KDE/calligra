/* This file is part of the KDE project
   Copyright (C) 2012-2013 Oleg Kukharchuk <oleg.kuh@gmail.com>
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jarosław Staniek <staniek@kde.org>

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

#include "kexidbcommandlinkbutton.h"
#include <core/kexiproject.h>
#include <core/KexiMainWindowIface.h>
#include <db/connection.h>
#include <KUrl>
#include <QDebug>

class KexiDBCommandLinkButtonPrivate
{
public:
    KexiDBCommandLinkButtonPrivate() {}

    KexiFormEventAction::ActionData onClickActionData;
};

KexiDBCommandLinkButton::KexiDBCommandLinkButton(const QString &text,
                                                 const QString &description, QWidget * parent)
        : KexiCommandLinkButton(text, description, parent)
        , d(new KexiDBCommandLinkButtonPrivate)
{
    setLocalBasePath(KexiMainWindowIface::global()->project()->dbConnection()->data()->dbPath());
}

KexiDBCommandLinkButton::~KexiDBCommandLinkButton()
{
    delete d;
}

void KexiDBCommandLinkButton::setValueInternal(const QVariant& add, bool removeOld)
{
    Q_UNUSED(add)
    Q_UNUSED(removeOld)

    if (KexiPushButton::hyperlinkType() == KexiPushButton::DynamicHyperlink) {
        KexiPushButton::setHyperlink(KexiDataItemInterface::originalValue().toString());
    }

    KUrl url(KexiDataItemInterface::originalValue().toString());
    setDescription(url.pathOrUrl());
    setToolTip(url.pathOrUrl());
}

QVariant KexiDBCommandLinkButton::value()
{
    if (KexiPushButton::hyperlinkType() == KexiPushButton::DynamicHyperlink) {
        return KexiPushButton::hyperlink();
    }
    return QVariant();
}

void KexiDBCommandLinkButton::clear()
{
    if (KexiPushButton::hyperlinkType() == KexiPushButton::DynamicHyperlink) {
        KexiPushButton::setHyperlink(QString());
    }
}

bool KexiDBCommandLinkButton::cursorAtStart()
{
    return false;
}

bool KexiDBCommandLinkButton::cursorAtEnd()
{
    return false;
}

bool KexiDBCommandLinkButton::valueIsNull()
{
    if (KexiPushButton::hyperlinkType() == KexiPushButton::DynamicHyperlink) {
        return KexiPushButton::hyperlink().isNull();
    }
    return true;
}

bool KexiDBCommandLinkButton::valueIsEmpty()
{
    if (KexiPushButton::hyperlinkType() == KexiPushButton::DynamicHyperlink) {
        return KexiPushButton::hyperlink().isEmpty();
    }
    return true;
}

void KexiDBCommandLinkButton::setInvalidState(const QString &displayText)
{
    if (KexiPushButton::hyperlinkType() == KexiPushButton::DynamicHyperlink) {
        setText(displayText);
    }
}
bool KexiDBCommandLinkButton::isReadOnly() const
{
    return true;
}

void KexiDBCommandLinkButton::setReadOnly(bool readOnly)
{
    Q_UNUSED(readOnly);
}

QString KexiDBCommandLinkButton::onClickAction() const
{
    return d->onClickActionData.string;
}

void KexiDBCommandLinkButton::setOnClickAction(const QString& actionString)
{
    d->onClickActionData.string = actionString;
}

QString KexiDBCommandLinkButton::onClickActionOption() const
{
     return d->onClickActionData.option;
}

void KexiDBCommandLinkButton::setOnClickActionOption(const QString& option)
{
     d->onClickActionData.option = option;
}

#include "kexidbcommandlinkbutton.moc"
