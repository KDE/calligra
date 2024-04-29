/* This file is part of the Calligra project, made with-in the KDE community

   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>
   SPDX-FileCopyrightText: 2013 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KORDFCALENDAREVENTREADER_H
#define KORDFCALENDAREVENTREADER_H

#include <KoRdfSemanticItemFactoryBase.h>

class KoEventSemanticItemFactory : public KoRdfSemanticItemFactoryBase
{
public:
    KoEventSemanticItemFactory();

public: // KoRdfSemanticItemFactoryBase API
    virtual QString className() const;
    virtual QString classDisplayName() const;

    virtual void updateSemanticItems(QList<hKoRdfBasicSemanticItem> &semanticItems, const KoDocumentRdf *rdf, QSharedPointer<Soprano::Model> m);
    virtual hKoRdfBasicSemanticItem createSemanticItem(const KoDocumentRdf *rdf, QObject *parent);
    virtual bool canCreateSemanticItemFromMimeData(const QMimeData *mimeData) const;
    virtual hKoRdfBasicSemanticItem
    createSemanticItemFromMimeData(const QMimeData *mimeData, KoCanvasBase *host, const KoDocumentRdf *rdf, QObject *parent = nullptr) const;

    virtual bool isBasic() const;
};

#endif
