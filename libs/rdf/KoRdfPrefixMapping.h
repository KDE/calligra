/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_KoRdfPrefixMapping_h__
#define __rdf_KoRdfPrefixMapping_h__

#include "RdfForward.h"
#include "kordf_export.h"
// Qt
#include <QMap>
#include <QObject>
#include <QSharedPointer>
#include <QString>

class KoRdfPrefixMappingPrivate;

/**
 * @short Supports bidirectional prefix:lname -> uri/lname translation
 *
 * @author Ben Martin <ben.martin@kogmbh.com>
 * @see KoDocumentRdf
 *
 * Both Rdf and XML support namespaces. This class is intended to allow
 * the advanced user to use such namespace mappings in the Calligra suite
 * when dealing with Rdf.
 *
 * For example, to be able to say dc:author for the dublin core author uri
 *
 */
class KORDF_EXPORT KoRdfPrefixMapping : public QObject
{
    Q_OBJECT
    KoRdfPrefixMappingPrivate *const d;

public:
    explicit KoRdfPrefixMapping(KoDocumentRdf *rdf);
    ~KoRdfPrefixMapping();

    /**
     * Convert a URI to a prefix:rest string
     * For example, given:
     * http://www.example.com/foo/bar
     * you might get
     * foo:bar
     * as the return value
     */
    QString URItoPrefexedLocalname(const QString &uri) const;

    /**
     * Opposite of URItoPrefexedLocalname(). Given foo:bar
     * you get http://www.example.com/foo/bar
     */
    QString PrefexedLocalnameToURI(const QString &pname) const;

    /**
     * Lookup the URI associated with a prefix.
     * given foo: you might get http://www.example.com/foo/
     */
    QString prefexToURI(const QString &pname) const;

    /**
     * Insert a new mapping prefix -> uri
     */
    void insert(const QString &prefix, const QString &url);

    /**
     * Delete the mapping for prefix
     */
    void remove(const QString &prefix);

    /**
     * Load the prefix mapping information from the given model.
     *
     * @see save()
     */
    void load(QSharedPointer<Soprano::Model> model);

    /**
     * Save the prefix mapping into the given Rdf model. If there is
     * already a mapping in the model then those triples are deleted
     * and fresh triples inserted to represet the this prefix mapping
     * state.
     *
     * @see load()
     */
    void save(QSharedPointer<Soprano::Model> model, Soprano::Node context) const;

    /**
     * Debug method to capture the data structure in the logs.
     */
    void dump() const;

private:
    friend class KoDocumentRdfEditWidget;
    QMap<QString, QString> mappings() const;
    QString canonPrefix(const QString &pname) const;
};

#endif
