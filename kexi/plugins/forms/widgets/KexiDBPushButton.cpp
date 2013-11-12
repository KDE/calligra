/* This file is part of the KDE project
   Copyright (C) 2013 Oleg Kukharchuk <oleg.kuh@gmail.com>
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

#include "KexiDBPushButton.h"
#include <core/kexiproject.h>
#include <core/KexiMainWindowIface.h>
#include <db/connection.h>
#include <KUrl>
#include <QDebug>

class KexiDBPushButtonPrivate
{
public:
    KexiDBPushButtonPrivate() {}

    KexiFormEventAction::ActionData onClickActionData;
};

KexiDBPushButton::KexiDBPushButton(const QString & text, QWidget * parent)
        : KexiPushButton(text, parent)
        , d(new KexiDBPushButtonPrivate)
{
    setLocalBasePath(KexiMainWindowIface::global()->project()->dbConnection()->data()->dbPath());
}

KexiDBPushButton::~KexiDBPushButton()
{
    delete d;
}

void KexiDBPushButton::setValueInternal(const QVariant& add, bool removeOld)
{
    Q_UNUSED(add)
    Q_UNUSED(removeOld)

    if (KexiPushButton::hyperlinkType() == KexiPushButton::DynamicHyperlink) {
        KexiPushButton::setHyperlink(KexiDataItemInterface::originalValue().toString());
    }
    KUrl url(KexiDataItemInterface::originalValue().toString());
    QFontMetrics f(font());
    QString text;
    QString path = url.pathOrUrl();
    if (url.isLocalFile()){
        QString fileName = url.fileName();
        text = f.elidedText(path.left(path.size() - fileName.size()),
                               Qt::ElideMiddle, width()-f.width(fileName) - 10) + fileName;
    } else {
        text = f.elidedText(path, Qt::ElideMiddle, width() - 10);

    }
    setText(text);
    setToolTip(url.pathOrUrl());
}

QVariant KexiDBPushButton::value()
{
    if (KexiPushButton::hyperlinkType() == KexiPushButton::DynamicHyperlink) {
        return KexiPushButton::hyperlink();
    }
    return QVariant();
}

void KexiDBPushButton::clear()
{
    if (KexiPushButton::hyperlinkType() == KexiPushButton::DynamicHyperlink) {
        KexiPushButton::setHyperlink(QString());
    }
}

bool KexiDBPushButton::cursorAtStart()
{
    return false;
}

bool KexiDBPushButton::cursorAtEnd()
{
    return false;
}

bool KexiDBPushButton::valueIsNull()
{
    if (KexiPushButton::hyperlinkType() == KexiPushButton::DynamicHyperlink) {
        return KexiPushButton::hyperlink().isNull();
    }
    return true;
}

bool KexiDBPushButton::valueIsEmpty()
{
    if (KexiPushButton::hyperlinkType() == KexiPushButton::DynamicHyperlink) {
        return KexiPushButton::hyperlink().isEmpty();
    }
    return true;
}

void KexiDBPushButton::setInvalidState(const QString &displayText)
{
    if (KexiPushButton::hyperlinkType() == KexiPushButton::DynamicHyperlink) {
        setText(displayText);
    }
}

bool KexiDBPushButton::isReadOnly() const
{
    return true;
}

void KexiDBPushButton::setReadOnly(bool readOnly)
{
    Q_UNUSED(readOnly);
}

QString KexiDBPushButton::onClickAction() const
{
    return d->onClickActionData.string;
}

void KexiDBPushButton::setOnClickAction(const QString& actionString)
{
    d->onClickActionData.string = actionString;
}

QString KexiDBPushButton::onClickActionOption() const
{
     return d->onClickActionData.option;
}

void KexiDBPushButton::setOnClickActionOption(const QString& option)
{
     d->onClickActionData.option = option;
}
#include "KexiDBPushButton.moc"
