/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TESTRDF_H
#define TESTRDF_H

#include <QObject>
#include <QtTest>
#include <qtest_kde.h>

class TestRdf : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void basicload();
    void findStatements();
#ifdef KDEPIMLIBS_FOUND
    void foaf();
    void calendarEvents();
#endif
    void locations();
    void prefixMapping();
    void addAndSave();
    void semanticItemViewSite();
    void sopranoTableModel();
    void expandStatementsToIncludeRdfLists();
    void expandStatementsToIncludeOtherPredicates();
    void expandStatementsReferencingSubject();
    void serializeRDFLists();
    void removeStatementsIfTheyExist();
    void KoTextRdfCoreTripleFunctions();
    void createUserStylesheet();

    // test creating, saving and loading a words document with semantic data and markers in the text
    void testRoundtrip();
};

#endif
