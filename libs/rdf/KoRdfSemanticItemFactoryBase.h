/* This file is part of the Calligra project, made with-in the KDE community

   Copyright (C) 2010 KO GmbH <ben.martin@kogmbh.com>
   Copyright (C) 2013 Friedrich W. H. Kossebau <kossebau@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KORDFSEMANTICITEMFACTORYBASE_H
#define KORDFSEMANTICITEMFACTORYBASE_H

#include "kordf_export.h"

// lib
#include "KoRdfSemanticItem.h"

class QMimeData;

#include <QString>


/**
 * A factory for semantic item objects. There should be one for each semantic item class to
 * allow the creation of the inlineObject from that plugin.
 * The factory additionally has information to allow showing a menu entry for user
 * access to the object-type.
 * @see KoInlineObjectRegistry
 */
class KORDF_EXPORT KoRdfSemanticItemFactoryBase
{
public:
    /**
     * Create the new factory
     * @param parent the parent QObject for memory management usage.
     * @param id a string that will be used internally for referencing the variable-type.
     */
    explicit KoRdfSemanticItemFactoryBase(const QString &id);
    virtual ~KoRdfSemanticItemFactoryBase();

public: // API required by KoGenericRegistry
    /**
     * return the id for the semantic item this factory creates.
     * @return the id for the semantic item this factory creates.
     */
    QString id() const;

public: // API to be implemented
    virtual QString className() const = 0;
    virtual QString classDisplayName() const = 0;

    virtual void updateSemanticItems(QList<hKoRdfSemanticItem> &semanticItems, const KoDocumentRdf *rdf, QSharedPointer<Soprano::Model> m) = 0;
    virtual hKoRdfSemanticItem createSemanticItem(const KoDocumentRdf *rdf, QObject *parent) = 0;
    virtual bool canCreateSemanticItemFromMimeData(const QMimeData *mimeData) const = 0;
    virtual hKoRdfSemanticItem createSemanticItemFromMimeData(const QMimeData* mimeData, KoCanvasBase *host,
                                                              const KoDocumentRdf *rdf, QObject *parent = 0) const = 0;
#if 0
    /**
     * Create a new instance of an inline object.
     */
    virtual KoInlineObject *createInlineObject(const KoProperties *properties = 0) const = 0;

    /**
     * Returns the type of object this factory creates.
     * The main purpose is to group plugins per type in, for example, a menu.
     */
    ObjectType type() const;

    /**
     * Return all the templates this factory knows about.
     * Each template shows a different way to create an object this factory is specialized in.
     */
    QList<KoInlineObjectTemplate> templates() const;

    QStringList odfElementNames() const;

    QString odfNameSpace() const;

    void setOdfElementNames(const QString &nameSpace, const QStringList &names);

protected:
    /**
     * Add a template with the properties of a speficic type of object this factory can generate
     * using the createInlineObject() method.
     * @param params The new template this factory knows to produce
     */
    void addTemplate(const KoInlineObjectTemplate &params);
#endif
private:
    class Private;
    Private * const d;
};

#endif
