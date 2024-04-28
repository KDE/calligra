/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_KoSemanticStylesheet_h__
#define __rdf_KoSemanticStylesheet_h__

#include "kordf_export.h"

#include "RdfForward.h"
// Qt
#include <QList>
#include <QObject>
#include <QSharedData>
#include <QString>

class KoSemanticStylesheetPrivate;

/**
 * @short A stylesheet that knows how to format a given KoRdfSemanticItem
 *
 * If you are looking to apply a stylesheet you should use the KoRdfSemanticItemViewSite
 * class. For example:
 *
 * KoRdfSemanticItemViewSite vs( SemanticItemPtr, xmlid );
 * vs.applyStylesheet( canvas, StylesheetPtr );
 *
 * @author Ben Martin <ben.martin@kogmbh.com>
 * @see KoRdfSemanticItem
 * @see KoRdfSemanticItemViewSite
 * @see KoDocumentRdf
 */
class KORDF_EXPORT KoSemanticStylesheet : public QObject, public QSharedData
{
    Q_OBJECT
    KoSemanticStylesheetPrivate *const d;

protected:
    // Restrict who can make us
    friend class KoRdfSemanticItem;
    friend class KoRdfSemanticItemViewSite;

    KoSemanticStylesheet(const QString &uuid, const QString &name, const QString &templateString, const QString &type = "System", bool isMutable = false);

    /**
     * Only called from KoRdfSemanticItemViewSite, this method actually
     * applies the stylesheet to a specific reference to a semantic
     * item in the document.
     */
    void format(hKoRdfSemanticItem obj, KoTextEditor *editor, const QString &xmlid = QString());

public:
    ~KoSemanticStylesheet();
    static QString stylesheetTypeSystem();
    static QString stylesheetTypeUser();

    QString uuid() const;
    QString name() const;
    QString templateString() const;
    QString type() const;
    bool isMutable() const;

    // if (isMutable()) these methods update
    // user stylesheets are mutable, system ones are not
    void name(const QString &v);
    void templateString(const QString &v);

Q_SIGNALS:
    void nameChanging(hKoSemanticStylesheet, const QString &oldName, const QString &newName);
};

#include <QMetaType>
Q_DECLARE_METATYPE(KoSemanticStylesheet *)
#endif
