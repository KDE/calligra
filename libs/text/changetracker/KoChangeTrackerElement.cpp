/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Pierre Stirnweiss \pierre.stirnweiss_calligra@gadz.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoChangeTrackerElement.h"

#include "TextDebug.h"
#include <kundo2magicstring.h>

#include <QTextDocumentFragment>
#include <QTextFormat>

class Q_DECL_HIDDEN KoChangeTrackerElement::Private
{
public:
    Private() = default;
    ~Private() = default;

    KUndo2MagicString title;
    KoGenChange::Type type;
    QTextFormat changeFormat;
    QTextFormat prevFormat;

    QString creator;
    QString date;
    QString extraMetaData;
    // These two elements are valid for delete changes. Need to move it to a sub-class
    QTextDocumentFragment deleteFragment;

    bool enabled;
    bool acceptedRejected;
    bool valid;
};

KoChangeTrackerElement::KoChangeTrackerElement(const KUndo2MagicString &title, KoGenChange::Type type)
    : d(new Private())
{
    d->title = title;
    d->type = type;
    d->acceptedRejected = false;
    d->valid = true;
}

KoChangeTrackerElement::KoChangeTrackerElement()
    : d(new Private())
{
}

KoChangeTrackerElement::KoChangeTrackerElement(const KoChangeTrackerElement &other)
    : d(new Private())
{
    d->title = other.d->title;
    d->type = other.d->type;
    d->changeFormat = other.d->changeFormat;
    d->prevFormat = other.d->prevFormat;
    d->creator = other.d->creator;
    d->date = other.d->date;
    d->extraMetaData = other.d->extraMetaData;
    d->deleteFragment = other.d->deleteFragment;
    d->enabled = other.d->enabled;
    d->acceptedRejected = other.d->acceptedRejected;
    d->valid = other.d->valid;
}

KoChangeTrackerElement::~KoChangeTrackerElement()
{
    delete d;
}

void KoChangeTrackerElement::setEnabled(bool enabled)
{
    d->enabled = enabled;
}

bool KoChangeTrackerElement::isEnabled() const
{
    return d->enabled;
}

void KoChangeTrackerElement::setAcceptedRejected(bool set)
{
    d->acceptedRejected = set;
}

bool KoChangeTrackerElement::acceptedRejected()
{
    return d->acceptedRejected;
}

void KoChangeTrackerElement::setValid(bool valid)
{
    d->valid = valid;
}

bool KoChangeTrackerElement::isValid() const
{
    return d->valid;
}

void KoChangeTrackerElement::setChangeType(KoGenChange::Type type)
{
    d->type = type;
}

KoGenChange::Type KoChangeTrackerElement::getChangeType() const
{
    return d->type;
}

void KoChangeTrackerElement::setChangeTitle(const KUndo2MagicString &title)
{
    d->title = title;
}

KUndo2MagicString KoChangeTrackerElement::getChangeTitle() const
{
    return d->title;
}

void KoChangeTrackerElement::setChangeFormat(const QTextFormat &format)
{
    d->changeFormat = format;
}

QTextFormat KoChangeTrackerElement::getChangeFormat() const
{
    return d->changeFormat;
}

void KoChangeTrackerElement::setPrevFormat(const QTextFormat &format)
{
    d->prevFormat = format;
}

QTextFormat KoChangeTrackerElement::getPrevFormat() const
{
    return d->prevFormat;
}

bool KoChangeTrackerElement::hasCreator() const
{
    return !d->creator.isEmpty();
}

void KoChangeTrackerElement::setCreator(const QString &creator)
{
    d->creator = creator;
}

QString KoChangeTrackerElement::getCreator() const
{
    return d->creator;
}

bool KoChangeTrackerElement::hasDate() const
{
    return !d->date.isEmpty();
}

void KoChangeTrackerElement::setDate(const QString &date)
{
    d->date = date;
}

QString KoChangeTrackerElement::getDate() const
{
    return d->date;
}

bool KoChangeTrackerElement::hasExtraMetaData() const
{
    return !d->extraMetaData.isEmpty();
}

void KoChangeTrackerElement::setExtraMetaData(const QString &metaData)
{
    d->extraMetaData = metaData;
}

QString KoChangeTrackerElement::getExtraMetaData() const
{
    return d->extraMetaData;
}

bool KoChangeTrackerElement::hasDeleteData() const
{
    return !d->deleteFragment.isEmpty();
}

void KoChangeTrackerElement::setDeleteData(const QTextDocumentFragment &fragment)
{
    d->deleteFragment = fragment;
}

QTextDocumentFragment KoChangeTrackerElement::getDeleteData() const
{
    return d->deleteFragment;
}
