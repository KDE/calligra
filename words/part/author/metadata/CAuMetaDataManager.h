/* This file is part of the KDE project
 * Copyright (C) 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef CAUMETADATAMANAGER_H
#define CAUMETADATAMANAGER_H

#include <QObject>
#include <QString>

#include <RdfForward.h>

class KoSection;
class CAuDocument;

/**
 * Class to manage Authors metadata.
 * It creates needed links to main rdf files.
 */
class CAuMetaDataManager : public QObject
{
    Q_OBJECT

public:
    /// Prefix for all Author related stuff.
    static const QString AUTHOR_PREFIX;
    /// How file with Author data in package should be named.
    static const QString AUTHOR_RDF_FILE_NAME;

    explicit CAuMetaDataManager(CAuDocument *caudoc);

    /// Context for all the Author data.
    static Soprano::Node authorContext();
    /// Calls editor for the metadata associated with specified section.
    void callEditor(KoSection *sec) const;

private:
    CAuDocument *m_doc;

    void registerAuthorRdfFile();
    void registerAuthorRdfPrefix();
    void registerAuthorSemanticItems();
};

#endif //CAUMETADATAMANAGER_H
