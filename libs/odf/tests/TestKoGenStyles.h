#ifndef TESTKOGENSTYLES_H
#define TESTKOGENSTYLES_H
/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004-2006 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/

#include <QObject>

class TestKoGenStyles : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testLookup();
    void testLookupFlags();
    void testDefaultStyle();
    void testUserStyles();
    void testWriteStyle();
    void testStylesDotXml();
};

#endif // TESTKOGENSTYLES_H
