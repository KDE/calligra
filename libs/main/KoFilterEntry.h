/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2007 David Faure <faure@kde.org>

SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __ko_filter_entry_h__
#define __ko_filter_entry_h__

#include "komain_export.h"
#include <KPluginMetaData>
#include <QExplicitlySharedDataPointer>
#include <QList>
#include <QStringList>

class QObject;
class KoFilter;
class KoFilterChain;
/**
 *  Represents an available filter.
 */
class KOMAIN_TEST_EXPORT KoFilterEntry : public QSharedData
{
public:
    using Ptr = QExplicitlySharedDataPointer<KoFilterEntry>;

    // KoFilterEntry() : weight( 0 ) { m_service = 0; } // for QList
    /**
     * @param loader pluginloader for the filter, KoFilterEntry takes ownership
     */
    explicit KoFilterEntry(const KPluginMetaData &metaData);
    ~KoFilterEntry();

    KoFilter *createFilter(KoFilterChain *chain, QObject *parent = nullptr);

    /**
     *  The imported mimetype(s).
     */
    QStringList import;

    /**
     *  The exported mimetype(s).
     */
    QStringList export_;

    /**
     *  The "weight" of this filter path. Has to be > 0 to be valid.
     */
    unsigned int weight;

    /**
     *  Do we have to check during runtime?
     */
    QString available;

    /**
     *  @return TRUE if the filter can import the requested mimetype.
     */
    bool imports(const QString &_mimetype) const
    {
        return (import.contains(_mimetype));
    }

    /**
     *  @return TRUE if the filter can export the requested mimetype.
     */
    bool exports(const QString &_m) const
    {
        return (export_.contains(_m));
    }

    QString fileName() const;

    /**
     *  This function will query KDED to find all available filters.
     */
    static QList<KoFilterEntry::Ptr> query();

private:
    KPluginMetaData const m_metaData;
};

#endif
