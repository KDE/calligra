/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _TEST_KO_COLOR_H_
#define _TEST_KO_COLOR_H_

#include <QObject>

class TestKoColor : public QObject
{
    Q_OBJECT
private:
    void testForModel(QString model);

private Q_SLOTS:
    void testSerialization();
    void testConversion();
};

#endif
