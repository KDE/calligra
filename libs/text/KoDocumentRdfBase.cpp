/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoDocumentRdfBase.h"

#include <KoDocumentResourceManager.h>
#include <KoText.h>

#ifdef SHOULD_BUILD_RDF
#include <Soprano/Soprano>
#endif

#include "TextDebug.h"

KoDocumentRdfBase::KoDocumentRdfBase(QObject *parent)
    : QObject(parent)
{
}

KoDocumentRdfBase::~KoDocumentRdfBase()
{
}

#ifdef SHOULD_BUILD_RDF
QSharedPointer<Soprano::Model> KoDocumentRdfBase::model() const
{
    return QSharedPointer<Soprano::Model>(0);
}
#endif

void KoDocumentRdfBase::linkToResourceManager(KoDocumentResourceManager *rm)
{
    QVariant variant;
    variant.setValue<QObject *>(this);
    rm->setResource(KoText::DocumentRdf, variant);
}

void KoDocumentRdfBase::updateInlineRdfStatements(const QTextDocument *qdoc)
{
    Q_UNUSED(qdoc);
}

void KoDocumentRdfBase::updateXmlIdReferences(const QMap<QString, QString> &m)
{
    Q_UNUSED(m);
}

bool KoDocumentRdfBase::loadOasis(KoStore *store)
{
    Q_UNUSED(store);
    return true;
}

bool KoDocumentRdfBase::saveOasis(KoStore *store, KoXmlWriter *manifestWriter)
{
    Q_UNUSED(store);
    Q_UNUSED(manifestWriter);
    return true;
}

bool KoDocumentRdfBase::completeLoading(KoStore *store)
{
    Q_UNUSED(store)
    return false;
}

bool KoDocumentRdfBase::completeSaving(KoStore *store, KoXmlWriter *manifestWriter, KoShapeSavingContext *context)
{
    Q_UNUSED(store)
    Q_UNUSED(manifestWriter)
    Q_UNUSED(context)
    return false;
}

QStringList KoDocumentRdfBase::idrefList() const
{
    return QStringList();
}
