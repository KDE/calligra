/* This file is part of the KDE project
SPDX-FileCopyrightText: 2008 Pierre Stirnweiss \pierre.stirnweiss_calligra@gadz.org>

SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoTextSharedSavingData.h"

#include "KoDocumentRdfBase.h"
#include "KoGenChanges.h"

#ifdef SHOULD_BUILD_RDF
#include <Soprano/Soprano>
#endif

#include <QMap>

class Q_DECL_HIDDEN KoTextSharedSavingData::Private
{
public:
    Private(void)
        : changes(nullptr)
    {
    }
    ~Private() = default;

    KoGenChanges *changes;
    QMap<QString, QString> m_rdfIdMapping; //< This lets the RDF system know old->new xml:id
#ifdef SHOULD_BUILD_RDF
    QSharedPointer<Soprano::Model> m_rdfModel; //< This is so cut/paste can serialize the relevant RDF to the clipboard
#endif
    QMap<int, QString> styleIdToName;
};

KoTextSharedSavingData::KoTextSharedSavingData()
    : d(new Private())
{
}

KoTextSharedSavingData::~KoTextSharedSavingData() = default;

void KoTextSharedSavingData::setGenChanges(KoGenChanges &changes)
{
    d->changes = &changes;
}

KoGenChanges &KoTextSharedSavingData::genChanges() const
{
    return *(d->changes);
}

void KoTextSharedSavingData::addRdfIdMapping(const QString &oldid, const QString &newid)
{
    d->m_rdfIdMapping[oldid] = newid;
}

QMap<QString, QString> KoTextSharedSavingData::getRdfIdMapping() const
{
    return d->m_rdfIdMapping;
}

#ifdef SHOULD_BUILD_RDF
void KoTextSharedSavingData::setRdfModel(QSharedPointer<Soprano::Model> m)
{
    d->m_rdfModel = m;
}

QSharedPointer<Soprano::Model> KoTextSharedSavingData::rdfModel() const
{
    return d->m_rdfModel;
}
#endif

void KoTextSharedSavingData::setStyleName(int styleId, const QString &name)
{
    d->styleIdToName.insert(styleId, name);
}

QString KoTextSharedSavingData::styleName(int styleId) const
{
    return d->styleIdToName.value(styleId);
}

QList<QString> KoTextSharedSavingData::styleNames() const
{
    return d->styleIdToName.values();
}
