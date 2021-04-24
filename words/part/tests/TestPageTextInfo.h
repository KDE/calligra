// This file is part of the Calligra project
// SPDX-FileCopyrightText: 2008 Thomas Zander <zander@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef TESTPAGETEXTINFO_H
#define TESTPAGETEXTINFO_H

#include <QObject>
#include <QtTest>
#include <qtest_kde.h>

class TestPageTextInfo : public QObject
{
    Q_OBJECT
private Q_SLOTS: // tests
    void init();
    void testPageNumber();
};

#endif
