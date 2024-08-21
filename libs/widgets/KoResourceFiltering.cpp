/*  This file is part of the KDE project

    SPDX-FileCopyrightText: 2013 Sascha Suelzer <s.suelzer@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoResourceFiltering.h"

#include "KoResourceServer.h"

#include <QRegularExpression>
#include <QString>
#include <QStringList>

class Q_DECL_HIDDEN KoResourceFiltering::Private
{
public:
    Private()
        : isTag("\\[([\\w\\s]+)\\]")
        , isExactMatch("\"([\\w\\s]+)\"")
        , searchTokenizer("\\s*,+\\s*")
        , hasNewFilters(false)
        , name(true)
        , filename(true)
        , resourceServer(nullptr)
    {
    }
    QRegularExpression isTag;
    QRegularExpression isExactMatch;
    QRegularExpression searchTokenizer;
    bool hasNewFilters;
    bool name, filename;
    KoResourceServerBase *resourceServer;
    QStringList tagSetFilenames;
    QStringList includedNames;
    QStringList excludedNames;
    QString currentTag;
};

KoResourceFiltering::KoResourceFiltering()
    : d(new Private())
{
}

KoResourceFiltering::~KoResourceFiltering()
{
    delete d;
}

void KoResourceFiltering::configure(int filterType, bool enable)
{
    switch (filterType) {
    case 0:
        d->name = true;
        d->filename = enable;
        break;
    case 1:
        d->name = enable;
        break;
    case 2:
        d->filename = enable;
        break;
    }
}

void KoResourceFiltering::setChanged()
{
    d->hasNewFilters = true;
}

void KoResourceFiltering::setTagSetFilenames(const QStringList &filenames)
{
    d->tagSetFilenames = filenames;
    d->excludedNames.clear();
    d->includedNames.clear();
    setChanged();
}

bool KoResourceFiltering::matchesResource(const QStringList &filteredList, const QStringList &filterList) const
{
    Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive;
    foreach (QString filter, filterList) {
        if (!filter.startsWith('"')) {
            foreach (QString filtered, filteredList) {
                if (filtered.contains(filter, sensitivity)) {
                    return true;
                }
            }
        } else if (d->name) {
            filter.remove('"');
            if (!filteredList.at(0).compare(filter)) {
                return true;
            }
        }
    }
    return false;
}

void KoResourceFiltering::sanitizeExclusionList()
{
    if (!d->includedNames.isEmpty()) {
        foreach (const QString &exclusion, d->excludedNames) {
            if (!excludeFilterIsValid(exclusion))
                d->excludedNames.removeAll(exclusion);
        }
    }
}

QStringList KoResourceFiltering::tokenizeSearchString(const QString &searchString) const
{
    return searchString.split(d->searchTokenizer, Qt::SkipEmptyParts);
}

void KoResourceFiltering::populateIncludeExcludeFilters(const QStringList &filteredNames)
{
    foreach (QString name, filteredNames) {
        QStringList *target;

        if (name.startsWith('!')) {
            name.remove('!');
            target = &d->excludedNames;
        } else {
            target = &d->includedNames;
        }

        if (!name.isEmpty()) {
            if (name.startsWith('[')) {
                QRegularExpressionMatch match = d->isTag.match(name);
                if (match.hasMatch() && d->resourceServer) {
                    name = match.captured(1);
                    (*target) += d->resourceServer->queryResources(name);
                }
            } else if (name.startsWith('"')) {
                QRegularExpressionMatch match = d->isExactMatch.match(name);
                if (match.hasMatch()) {
                    target->push_back(name);
                }
            } else {
                target->push_back(name);
            }
        }
    }
    sanitizeExclusionList();
}

bool KoResourceFiltering::hasFilters() const
{
    return (!d->tagSetFilenames.isEmpty() || !d->includedNames.isEmpty() || !d->excludedNames.isEmpty());
}

bool KoResourceFiltering::filtersHaveChanged() const
{
    return d->hasNewFilters;
}

void KoResourceFiltering::setFilters(const QString &searchString)
{
    d->excludedNames.clear();
    d->includedNames.clear();
    QStringList filteredNames = tokenizeSearchString(searchString);
    populateIncludeExcludeFilters(filteredNames);
    setChanged();
}

bool KoResourceFiltering::presetMatchesSearch(KoResource *resource) const
{
    QList<QString> filteredList;

    QString resourceFileName = resource->shortFilename();
    QString resourceName = resource->name();

    if (d->name) {
        filteredList.push_front(resourceName);
    }

    if (d->filename) {
        filteredList.push_back(resourceFileName);
    }

    if (matchesResource(filteredList, d->excludedNames)) {
        return false;
    }

    if (matchesResource(filteredList, d->includedNames)) {
        return true;
    }

    foreach (const QString &filter, d->tagSetFilenames) {
        if (!resourceFileName.compare(filter) || !resourceName.compare(filter)) {
            return true;
        }
    }

    return false;
}

void KoResourceFiltering::setInclusions(const QStringList &inclusions)
{
    d->includedNames = inclusions;
    setChanged();
}

void KoResourceFiltering::setExclusions(const QStringList &exclusions)
{
    d->excludedNames = exclusions;
    setChanged();
}

bool KoResourceFiltering::excludeFilterIsValid(const QString &exclusion)
{
    foreach (const QString &inclusion, d->includedNames) {
        if ((inclusion.startsWith(exclusion) && exclusion.size() <= inclusion.size())) {
            return false;
        }
    }
    return true;
}

QList<KoResource *> KoResourceFiltering::filterResources(QList<KoResource *> resources)
{
    foreach (KoResource *resource, resources) {
        if (!presetMatchesSearch(resource)) {
            resources.removeAll(resource);
        }
    }
    setDoneFiltering();
    return resources;
}

void KoResourceFiltering::setDoneFiltering()
{
    d->hasNewFilters = false;
}

void KoResourceFiltering::rebuildCurrentTagFilenames()
{
    d->tagSetFilenames = d->resourceServer->queryResources(d->currentTag);
}

void KoResourceFiltering::setCurrentTag(const QString &tagSet)
{
    d->currentTag = tagSet;
    rebuildCurrentTagFilenames();
}

void KoResourceFiltering::setResourceServer(KoResourceServerBase *resourceServer)
{
    d->resourceServer = resourceServer;
}
