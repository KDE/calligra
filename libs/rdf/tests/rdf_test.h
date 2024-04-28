/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef RDF_TEST_H
#define RDF_TEST_H

#include <QObject>

class KoTextEditor;
class KoDocumentRdf;
class QString;

class RdfTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testCreateMarkers();
    void testFindMarkers();
    void testFindByName();
    void testEditAndFindMarkers(); // XXX: implement
    void testRemoveMarkers();

private:
    // @return the xml id for the inserted item
    QString insertTableWithSemItem(KoTextEditor &editor, KoDocumentRdf &rdfDoc, const QString name);
};

#endif // RDF_TEST_H
