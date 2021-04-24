/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef TESTKOODFLOADINGCONTEXT_H
#define TESTKOODFLOADINGCONTEXT_H

#include <QObject>

class TestKoOdfLoadingContext : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testFillStyleStack();
};

#endif /* TESTKOODFLOADINGCONTEXT_H */
