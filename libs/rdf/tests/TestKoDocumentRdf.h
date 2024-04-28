/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TEST_KO_DOCUMENT_RDF_H
#define TEST_KO_DOCUMENT_RDF_H

#include <QObject>

class KoDocumentRdf;

class TestKoDocumentRdf : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testCreate();
    void testRememberNewInlineRdfObject();
};

#endif // TEST_KO_DOCUMENT_RDF_H
