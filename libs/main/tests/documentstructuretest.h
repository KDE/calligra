/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef DOCUMENTSTRUCTURETEST_H
#define DOCUMENTSTRUCTURETEST_H

#include <QObject>

class DocumentStructureTest : public QObject
{
    Q_OBJECT
public:
    DocumentStructureTest();
    ~DocumentStructureTest() override;

private Q_SLOTS:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

    void rootAttributes();
    void predefinedMetaData();
    void multipleDocumentContents();
    void singleDocumentContents();
};

#endif // DOCUMENTSTRUCTURETEST_H
