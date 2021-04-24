/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TESTKOELEMENTREFERENCE_H
#define TESTKOELEMENTREFERENCE_H

#include <QObject>

class TestKoElementReference : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testElementReference();
};

#endif
