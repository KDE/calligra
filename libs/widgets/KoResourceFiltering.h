
/*  This file is part of the KDE project

    SPDX-FileCopyrightText: 2013 Sascha Suelzer <s.suelzer@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KORESOURCEFILTER_H
#define KORESOURCEFILTER_H

#include "kowidgets_export.h"

#include <QList>

class KoResourceServerBase;
class KoResource;
class QString;

class KOWIDGETS_EXPORT KoResourceFiltering
{
public:
    KoResourceFiltering();
    virtual ~KoResourceFiltering();
    void configure(int filterType, bool enable);
    bool hasFilters() const;
    bool filtersHaveChanged() const;
    void setTagSetFilenames(const QStringList &filenames);
    void setCurrentTag(const QString &tagSet);
    void rebuildCurrentTagFilenames();
    void setResourceServer(KoResourceServerBase *resourceServer);
    void setFilters(const QString &searchString);
    QList<KoResource *> filterResources(QList<KoResource *> resources);
    void setInclusions(const QStringList &inclusions);
    void setExclusions(const QStringList &exclusions);

private:
    void setDoneFiltering();
    bool presetMatchesSearch(KoResource *resource) const;
    void setChanged();
    bool excludeFilterIsValid(const QString &exclusion);
    bool matchesResource(const QStringList &filtered, const QStringList &filterList) const;
    void populateIncludeExcludeFilters(const QStringList &filteredNames);
    void sanitizeExclusionList();
    QStringList tokenizeSearchString(const QString &searchString) const;

    class Private;
    Private *const d;
};

#endif // KORESOURCEFILTER_H
