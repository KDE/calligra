/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KO_DOCUMENT_Rdf_Base_H
#define KO_DOCUMENT_Rdf_Base_H

#include "kotext_export.h"

#include <QMap>
#include <QMetaType>
#include <QObject>
#include <QSharedPointer>
#include <QString>

#include <KoDataCenterBase.h>

class KoDocumentResourceManager;
class QTextDocument;
class KoStore;
class KoXmlWriter;

///**
// * Dummy definition in case Soprano is not available.
// */
namespace Soprano
{
class Model;
}

/**
 * A base class that provides the interface to many RDF features
 * but will not do anything if Soprano support is not built.
 * By having this "Base" class, code can call methods at points
 * where RDF handling is desired and can avoid \#ifdef conditionals
 * because the base class interface is here and will be valid, even
 * if impotent when Soprano support is not built.
 */
class KOTEXT_EXPORT KoDocumentRdfBase : public QObject, public KoDataCenterBase
{
    Q_OBJECT

public:
    explicit KoDocumentRdfBase(QObject *parent = nullptr);
    ~KoDocumentRdfBase() override;

    /**
     * Get the Soprano::Model that contains all the Rdf
     * You do not own the model, do not delete it.
     */
#ifdef SHOULD_BUILD_RDF
    virtual QSharedPointer<Soprano::Model> model() const;
#endif
    virtual void linkToResourceManager(KoDocumentResourceManager *rm);

    virtual void updateInlineRdfStatements(const QTextDocument *qdoc);
    virtual void updateXmlIdReferences(const QMap<QString, QString> &m);

    /**
     * idrefList queries soprano after loading and creates a list of all rdfid's that
     * where found in the manifest.rdf document. This list is used to make sure we do not
     * create more inline rdf objects than necessary
     * @return a list of xml-id's
     */
    virtual QStringList idrefList() const;

    virtual bool loadOasis(KoStore *store);
    virtual bool saveOasis(KoStore *store, KoXmlWriter *manifestWriter);

    // reimplemented in komain/rdf/KoDocumentRdf
    bool completeLoading(KoStore *store) override;
    bool completeSaving(KoStore *store, KoXmlWriter *manifestWriter, KoShapeSavingContext *context) override;
};

Q_DECLARE_METATYPE(KoDocumentRdfBase *)

#endif
