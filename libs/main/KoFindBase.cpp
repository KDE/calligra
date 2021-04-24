/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFindBase.h"

#include "KoFindOptionSet.h"

class Q_DECL_HIDDEN KoFindBase::Private
{
public:
    Private() : currentMatch(0), options(0) { }

    KoFindMatchList matches;
    int currentMatch;
    KoFindOptionSet *options;
};

KoFindBase::KoFindBase(QObject *parent)
    : QObject(parent), d(new Private)
{
}

KoFindBase::~KoFindBase()
{
    delete d->options;
    delete d;
}

const KoFindBase::KoFindMatchList &KoFindBase::matches() const
{
    return d->matches;
}

bool KoFindBase::hasMatches() const
{
    return d->matches.count() > 0;
}

KoFindMatch KoFindBase::currentMatch() const
{
    if (d->matches.count() > 0 && d->currentMatch < d->matches.count()) {
        return d->matches.at(d->currentMatch);
    }
    return KoFindMatch();
}

KoFindOptionSet *KoFindBase::options() const
{
    return d->options;
}

void KoFindBase::setMatches(const KoFindBase::KoFindMatchList &matches)
{
    d->matches = matches;
}

void KoFindBase::setCurrentMatch(int index)
{
    d->currentMatch = index;
}

int KoFindBase::currentMatchIndex()
{
    return d->currentMatch;
}

void KoFindBase::find(const QString &pattern)
{
    clearMatches();
    d->matches.clear();
    findImplementation(pattern, d->matches);

    emit hasMatchesChanged(d->matches.count() > 0);
    if (d->matches.size() > 0) {
        if (d->currentMatch >= d->matches.size()) {
            d->currentMatch = 0;
        }
        emit matchFound(d->matches.at(d->currentMatch));
    } else {
        emit noMatchFound();
    }

    emit updateCanvas();
}

void KoFindBase::findNext()
{
    if (d->matches.count() == 0) {
        return;
    }

    d->currentMatch = (d->currentMatch + 1) % d->matches.count();
    emit matchFound(d->matches.at(d->currentMatch));

    if (d->currentMatch == 0) {
        emit wrapAround(true);
    }

    emit updateCanvas();
}

void KoFindBase::findPrevious()
{
    if (d->matches.count() == 0) {
        return;
    }

    d->currentMatch = (--d->currentMatch) >= 0 ? d->currentMatch : d->matches.count() - 1;
    emit matchFound(d->matches.at(d->currentMatch));

    if (d->currentMatch == d->matches.count() - 1) {
        emit wrapAround(false);
    }

    emit updateCanvas();
}

void KoFindBase::finished()
{
    clearMatches();
    d->matches.clear();
    emit updateCanvas();
}

void KoFindBase::replaceCurrent(const QVariant &value)
{
    if(d->matches.count() == 0) {
        return;
    }

    KoFindMatch match = d->matches.at(d->currentMatch);
    d->matches.removeAt(d->currentMatch);
    if (d->currentMatch < d->matches.count()) {
        replaceImplementation(match, value);
    }

    if(d->matches.count() > 0) {
        emit matchFound(d->matches.at(0));
    } else {
        emit noMatchFound();
    }
    emit updateCanvas();
}

void KoFindBase::replaceAll(const QVariant &value)
{
    foreach(const KoFindMatch &match, d->matches) {
        replaceImplementation(match, value);
    }

    //Intentionally not using clearMatches since we should not clear
    //highlighting here.
    d->matches.clear();
    emit noMatchFound();
    emit updateCanvas();
}

void KoFindBase::clearMatches()
{
    //Intentionally does nothing, only needs to be reimplemented when
    //something needs to be done before clearing the list of matches.
}

void KoFindBase::setOptions(KoFindOptionSet *newOptions)
{
    delete d->options;
    d->options = newOptions;
    d->options->setParent(this);
}
