/*
 *  SPDX-FileCopyrightText: 2005 Adrian Page <adrian@pagenet.plus.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KORGBU8COLORSPACETESTER_H
#define KORGBU8COLORSPACETESTER_H

#include <QObject>

class KoRgbU8ColorSpaceTester : public QObject
{
    Q_OBJECT
    void testCompositeOps();
private Q_SLOTS:
    void testBasics();
    void testMixColors();
    void testCompositeOpsWithChannelFlags();
};

#endif
