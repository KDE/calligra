/*
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KIS_DOCUMENT_FACTORY_H
#define KIS_DOCUMENT_FACTORY_H

#include <abstractdocumentfactory.h>

/**
 * @brief The KisDocumentFactory class creates krita image documents.
 *
 * XXX: put this in a plugin of Calligra/Document type
 */
class KisDocumentFactory : public Kasten2::AbstractDocumentFactory
{
    Q_OBJECT
public:
    explicit KisDocumentFactory(QObject *parent = 0);
    virtual ~KisDocumentFactory();

    virtual bool canCreateFromData( const QMimeData* mimeData );
    virtual Kasten2::AbstractDocument* create();
    virtual Kasten2::AbstractDocument* create(const QMap<QString, QVariant> &parameters);
    virtual Kasten2::AbstractDocument* createFromData( const QMimeData* mimeData, bool setModified );
    
};

#endif // KIS_DOCUMENT_FACTORY_H
