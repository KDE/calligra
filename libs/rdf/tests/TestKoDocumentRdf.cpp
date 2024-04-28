/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 * SPDX-FileCopyrightText: 2012 C. Boemann <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "TestKoDocumentRdf.h"

#include <QString>
#include <QTest>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QTextTable>
#include <QUuid>

#include <KoBookmark.h>
#include <KoDocumentRdf.h>
#include <KoInlineTextObjectManager.h>
#include <KoRdfSemanticItem.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <KoTextInlineRdf.h>

const QString lorem(
    "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor"
    "incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud"
    "exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\n"
    "Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla"
    "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia"
    "deserunt mollit anim id est laborum.\n");

void TestKoDocumentRdf::testCreate()
{
    KoDocumentRdf *rdfDoc = new KoDocumentRdf();
    Q_ASSERT(rdfDoc->model());
    delete rdfDoc;
}

void TestKoDocumentRdf::testRememberNewInlineRdfObject()
{
    KoDocumentRdf rdfDoc;
    QTextDocument doc;

    QTextCursor cur(&doc);
    KoBookmark bm(cur);
    bm.setName("test");

    KoTextInlineRdf inlineRdf(&doc, &bm);
    inlineRdf.setXmlId(inlineRdf.createXmlId());

    rdfDoc.rememberNewInlineRdfObject(&inlineRdf);

    Q_ASSERT(&inlineRdf == rdfDoc.findInlineRdfByID(inlineRdf.xmlId()));
}

QTEST_MAIN(TestKoDocumentRdf)

#include "TestKoDocumentRdf.moc"
